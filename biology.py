#!/usr/bin/python

from z3 import *
import argparse

#----------------------------------------------------
# input parsing
# input number of nodes and molecules
parser = argparse.ArgumentParser(description='Auto testing for TARA')
parser.add_argument("-M","--mols", type=int, default=4, help = "number of molecules")
parser.add_argument("-N","--nodes", type=int, default=2, help = "number of nodes")
args = parser.parse_args()
M = args.mols
N = args.nodes

#----------------------------------------------------
# Constraint generation 
dump = [ [Bool ("d_{}_{}".format(i,j)) for j in range(N)] for i in range(N)]

node = [ [Bool ("n_{}_{}".format(i,k)) for k in range(M)] for i in range(N)]

active_node = [ [Bool ("a_{}_{}".format(i,k)) for k in range(M)] for i in range(N)]

edge = [ [Bool ("e_{}_{}".format(i,j)) for j in range(N)] for i in range(N)]

presence_edge = [ [ [Bool ("e_{}_{}_{}".format(i,j,k)) for k in range(M)] for j in range(N)] for i in range (N)]

active_edge = [ [ [Bool ("b_{}_{}_{}".format(i,j,k)) for k in range(M)] for j in range(N)] for i in range (N)]

r = [ [ [ [Bool ("r_{}_{}_{}_{}".format(i,j,k,z)) for z in range(N-1)] for k in range(M)] for j in range(N)] for i in range (N)]

p = [ [Bool ("p_{}_{}".format(k,k1)) for k1 in range(M)] for k in range(M)]

r1 = [ [[Bool ("r1_{}_{}_{}".format(i,j,z)) for z in range(N-1)]for j in range(N)] for i in range(N)]

sorts = [IntSort() for m in range(M)]

act_node = [Function ("an_{}".format(m), *sorts) for m in range(M)] 
act_edge = [Function ("ae_{}".format(m), *sorts) for m in range(M)] 

#for k in range(M):
#    f = Function('f', IntSort(), BoolSort())

# Few additional condition that needs to be met.

#1. label(E) subset  label(N)
# e_ijk -> aik
# e_ijk -> ajk
C0 = True
C1 = True
for i in range(N):
    for j in range(N):
        if i == j :
            continue
        for k in range(M):
            C0 = And (Implies(presence_edge[i][j][k],node[i][k]),C0)
            C1 = And (Implies(presence_edge[i][j][k],node[j][k]),C1)

#2. Self edges not allowed. 
# not e_ii  
C2 = True
for i in range(N):
    C2 = And(Not(edge[i][i]), C2)


#3. Multiple(parallel) edges are allowed between two nodes. 
# But we restrict it to two.
C3 = True

#4. Condition on p_kk' : 
# \/_{x<M/2,y>=M/2} !p(x,y) and !p(x,y)

C4 = True
for x in range(M):
    for y in range(M):
        if (((x < M/2) and (y < M/2)) or ((x>=M/2) and (y >=M/2))):
            C4 = And( Not(p[x][y]),C4 )


#5. Activity on the node.
C5 = True 
for i in range(N):
    for k in range(M):
        C5 = And (Implies (active_node[i][k], node[i][k]), C5) 
#print C5
# ----------------------------------------------------------------

# MAIN Constraints:

# F_0
# \/_k e_ijk -> e_ij  at evry edge there is an active molecules which is present.
# Involve equality and we'll require it while doing validity check.
F0 = True
for i in range(N):
    for j in range(N):
        if j == i:
            continue        
        rhs = False
        for k in range(M):
            rhs = Or( presence_edge[i][j][k], rhs )
        F0 = And (Implies(rhs, edge[i][j]),F0)
#print F0



# F1:  b_ijk -> e_ijk   
# If molecule is active on an edge then it should be present on the edge. \newline
F1 = True
for i in range(N):
    for j in range(N):
        if j == i:
            continue
        for k in range(M):
            F1 = And( Implies(active_edge[i][j][k], presence_edge[i][j][k]), F1 )
#print F1

 
#F2 New reachability and stability condition-----
# States reachability definition: nodes i,j is reachable with kth moleculein z steps if i'' is reachable from i in z steps and there is an edge between i'' and j with k present on that edge.
# /\_{i,j,k,p, i!=j} r_{i,j,k,p} -> \/_{l!=i || j} r_{i,l,k,p-1} and e_{l,j,k}

# Have also added one length reachability with F2. 
# r_{i,j,k,1} -> e_{i,j,k} : In our case 1 is 0.
F2 = True
for i in range(N):
    for j in range(N):
	if i == j:
	    continue
        for k in range(M):
            lhs = False
            for z in range(N-1):
                if z == 0: 
                    F2 = And (Implies(r[i][j][k][0],presence_edge[i][j][k]),F2) 
                else:
                    for l in range(N):
                        if i == l or j == l:
                            continue
                        lhs = Or(And (r[i][l][k][z-1],presence_edge[l][j][k]),lhs)   
                    F2 = And (Implies (r[i][j][k][z],lhs), F2)                    
#print F2

# ---- Rule change proposed----
F3 = True 
for i in range(N):
    for j in range(N):
	if i == j:
	    continue
        for k in range(M):
            lhs = False
            for z in range(N-1):
                lhs = Or(r[j][i][k][z], lhs)
            F3 = And (Implies (presence_edge[i][j][k],lhs),F3)
#print F3


# F4: Fusion rules:
# /\_{i,j} ( \/_k e_{i,j,k}) -> \/_{k,k'} (b_{i,j,k} and a'_{j,k'} and p_{k,k'}) 
# and /\_k b_{i,j,k} -> not(\/_{j' != j} (\/_k'' a'_{j',k''} and p_{k,k''}))  
F4 = True
for i in range(N):
    for j in range(N):
	if i == j:
	    continue
	lhs = False    
        for k in range(M):
   #         rhs = Or(presence_edge[i][j][k],rhs)        
	    for k1 in range(M): 
                if k == k1:
                    continue
	        lhs = Or (And(active_edge[i][j][k],active_node[j][k1],p[k][k1]), lhs)  
        F4 = And (Implies(edge[i][j],lhs), F4)
#print F4

F5 = True
for i in range(N):
    for j in range(N):
	if i == j:
	    continue
        for k in range(M): 
            lhs = False
	    for j1 in range(N):
                if j != j1:
	            for k11 in range(M):
                        if k == k11:
                            continue
		        lhs = Or(And (active_node[j1][k11],p[k][k11]), lhs)
	    F5 = And (Implies(active_edge[i][j][k], Not(lhs)), F5)
#print F5

#----3 Connectivity --------

# Summation of d_{i,j} == 2
#D1 = 0
#for i in range(N):
#    for j in range(N):
#        D1 = dump[i][j] + D1 
#F6 = D1 == 2
#F6 = [Or(dump[i][j] == dump[j][k] 


# Make directed graph undirected
# /\_{i,j} r1_{i,j} Or r1_{j,i} 
F7 = True
for i in range(N):
    for j in range(N):
        for z in range(N-1):
            F7 = And( (r1[i][j][z],r1[j][i][z]), F7) 

# Removal of Sum_d does't disconnects the graph.
F8 = True
for i in range(N):
    for j in range(N):
        lhs = False
        for l in range(N):
            if i == l:
                continue
            lhs = Or(And(r1[i][j], And(edge[i][l],Not(d[i][j]))), lhs) 
        F8 = And (Implies(r1[i][j],lhs),F8)

#--- Activity as a function of other molecules presence------

for k in range(M):
    f = Function('f', IntSort(), BoolSort())
#
F6 = True


#Init = (presence_edge[0][1][0] == True)
#Init2 = (p[0][3] == False)
#Init3 = (p[1][2] == False)
# Create Solver and add constraints in it.

s = Solver()
s.add(C0,C1,C2,C3,C4,C5,F0,F1,F2,F3,F4,F5)
print "solving...\n"
print "Printing the assertions..."
for c in s.assertions():
    print c
print s.check()
print "\nPrinting the statistics..."
print s.statistics() 
print "\n"


def dump_dot( filename, m ) :
    dfile = open(filename, 'w')
    dfile.write("digraph prog {\n")
    for i in range(N):
        node_vec = str(i)+":"
        for k in range(M):
            if is_true(m[node[i][k]]) :
                node_vec = node_vec + "1"
            elif is_false(m[node[i][k]]):
                node_vec = node_vec + "0"
            if is_true(m[active_node[i][k]]) :
                node_vec = node_vec + "-"
        dfile.write( str(i) + "[label=\"" + node_vec + "\"]\n")
        for j in range(N):
            if i == j:
                continue
            for k in range(M):
                if is_true(m[presence_edge[i][j][k]]):
                    label = str(k)
                    color = "black"
                    if is_true(m[active_edge[i][j][k]]):
                        color = "green"
                        for k1 in range(M):
                            if is_true(m[active_node[j][k1]]) and is_true(m[p[k][k1]]):
                                color = "red"
                                break
                    dfile.write( str(i) + "-> " + str(j) +
                                 "[label=" + label +",color=" + color +"]" +"\n" )
    dfile.write("}\n")
    
#    for k2 in range(M):
#        print "\t"+str(k2),
#    print "\n"
#    for k1 in range(M):
#        for k2 in range(M):
#            if is_true(m[p[k1][k2]]):
#                print "\t0",
#            else:
#                print "\t1",
#        print "\n"

if s.check() == sat:
    m = s.model()
    print "Printing the model..."
    for d in m.decls():
        print "{} = {}".format(d.name(), m[d])
    #print m
    r = [ [ m[p[i][j]] for j in range(M) ]
          for i in range(M) ]
    s = [[ [ m[active_edge[i][j][k]] for k in range (M)] for j in range(N) ] for i in range(N) ]
    t = [ [ m[edge[i][j]] for j in range(N) ]
          for i in range(N) ]
    print "\n Pairing matrix p[{}][{}] = ".format(M,M)
    print_matrix(r)
    print "\n Edge  e[{}][{}] = ".format(N,N)
    print t
#    print "\n Activity on edge a[{}][{}][{}] = ".format(N,N,M)
#    print_matrix(s)
    dump_dot( "/tmp/bio.dot", m )
else:
    print "failed to solve"


