#!/usr/bin/python

from z3 import *
import argparse
import time

#----------------------------------------------------
# input parsing
# input number of nodes and molecules
parser = argparse.ArgumentParser(description='Auto testing for TARA')
parser.add_argument("-M","--mols", type=int, default=32, help = "number of molecules")
parser.add_argument("-N","--nodes", type=int, default=8, help = "number of nodes")
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

r1 = [ [Bool ("r1_{}_{}".format(i,j)) for j in range(N)] for i in range(N)]

sorts = [BoolSort() for m in range(M)]

f_n = [Function ("an_{}".format(m), *sorts) for m in range(M)] 
f_e = [Function ("ae_{}".format(m), *sorts) for m in range(M)] 

# Few additional condition that needs to be met.

starttime = time.time()

# Activity of a moolecule k on a node/edge is  
# defined as a Boolean function of presence of 
# Other molecule present on that node/edge.
# A0. active_node[k] =  f_n[k](\/_{k1 != k} node(k1)) 
A0 = True
s = []
A_list = []
for k in range(M):
    f = f_n[k]
    for i in range(N):
        del s[:]
        for k1 in range(M):
            if k1 == k:
                continue
            s.append( node[i][k1] )
        f_app = f(s)
        l = Implies( node[i][k], active_node[i][k] == f_app )
        # l = Implies( node[i][k], active_node[i][k] == f(*s) )
        A_list.append(l)
A0 = And( A_list )
#print A0

a =  time.time() - starttime
print "A0 took", str(a)

# A1. active_edge[k] = f_e[k](\/_{k1 != k} presence_edge(k1))
A1 = True
s = []
A_list = []
for i in range(N):
    for j in range(N):
        if i == j:
            continue
        for k in range(M):
            del s[:]
            for k1 in range(M):
                if k1 == k:
                    continue
                s.append(presence_edge[i][j][k1])
            l  =  Implies(presence_edge[i][j][k], active_edge[i][j][k] == f_e[k](*s)) 
            A_list.append(l)
A1 = And( A_list )

a1 = time.time() - starttime - a
print "A1 took", str(a1)

#print A1
#sys.exit()

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

c = time.time() - starttime - a1
print "C0-C5 Took", str(c)

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
F1_list = []
for i in range(N):
    for j in range(N):
        if j == i:
            continue
        for k in range(M):
            # F1 = And( Implies(active_edge[i][j][k], presence_edge[i][j][k]), F1 )
            F1_list.append( Implies(active_edge[i][j][k], presence_edge[i][j][k]) )
F1 = And( F1_list )
#print F1

f = time.time() - starttime - c
print "F0-F1 took ", str(f)

 
#F2 New reachability and stability condition-----
# States reachability definition: nodes i,j is reachable with kth moleculein z steps if i'' is reachable from i in z steps and there is an edge between i'' and j with k present on that edge.
# /\_{i,j,k,p, i!=j} r_{i,j,k,p} -> \/_{l!=i || j} r_{i,l,k,p-1} and e_{l,j,k}

# Have also added one length reachability with F2. 
# r_{i,j,k,1} -> e_{i,j,k} : In our case 1 is 0.
F2 = True
A_list = []
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
                    w  = Implies (r[i][j][k][z],lhs)
                    A_list.append(w)
F2 = And(A_list)
#print F2

# ---- Rule change proposed----
F3 = True 
A_list = []
for i in range(N):
    for j in range(N):
	if i == j:
	    continue
        for k in range(M):
            lhs = False
            for z in range(N-1):
                lhs = Or(r[j][i][k][z], lhs)
            l  = Implies (presence_edge[i][j][k],lhs)
            A_list.append(l)
F3 = And(A_list)
#print F3

ff = time.time() - starttime - f
print "F2-F3 took ", str(ff)

# F4: Fusion rules:
# /\_{i,j} ( \/_k e_{i,j,k}) -> \/_{k,k'} (b_{i,j,k} and a'_{j,k'} and p_{k,k'}) 
# and /\_k b_{i,j,k} -> not(\/_{j' != j} (\/_k'' a'_{j',k''} and p_{k,k''}))  
F4 = True
A_list = []
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
        w =  Implies(edge[i][j],lhs)
        A_list.append(w)
F4 = And(A_list)
#print F4

F5 = True
A_list = []
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
	    w = Implies(active_edge[i][j][k], Not(lhs))
            A_list.append(w)
F5 = And(A_list)
#print F5

fff = time.time() - starttime - ff
print "F2-F3 took ", str(fff)

starttime = time.time()
#----3 Connectivity --------

# Summation of d_{i,j} == 2
#D1 = 0
#for i in range(N):
#    for j in range(N):
#        D1 = dump[i][j] + D1 

# Need flatening.
d1 = [dump[i][j] for i in range(N) for j in range(N)]
L = len(d1)

#print L

# At most 2
D0 = True
D_list = []
for i in range(L):
  for j in range(i+1, L-1):
    lhs = And(d1[i],d1[j])
    for k in range(j+1, L):
      x = Implies (lhs,Not(d1[k]))
      D_list.append(x)
D0 = And(D_list)      
      
#print D0

# At least 2
D1 = False
for i in range(L):
  for j in range(i+1,L):
    D1 = Or(And(d1[i],d1[j]),D1)

#print C1

# At Most two
# Haskell 
# [Or x | x = Or (d1[i] and d1[j] and d1[k]),   i <- [0..L], j <- [x+1..L-1] , k <- [j+1,L]]

# At least two
# [Or(x) | x = d1[i] and d1[j] , i <- [0..L], j <- [0..L] , i!=j]
#D1 = [Or(And(d1[x],d1[y]), False) for x in range(L)  y in range(L) x != y]


# Make directed graph undirected
# /\_{i,j} r1_{i,j} Or r1_{j,i} 
D2 = True
for i in range(N):
    for j in range(N):
        rijji = Or(r1[i][j],r1[j][i])
        D2 = And( rijji, D2) 
D2 = Not(D2)

dd = time.time() - starttime 
print "D0-D2 took", str(dd)

# do we need length reach?
D3 = True
for i in range(N):
    for j in range(N):
        rhs = And( edge[i][j], Not(dump[i][j]) )
        for l in range(N):
            if i == l:
                continue
            rhs = Or( And(And(edge[i][l],Not(dump[i][l])),r1[l][j] ), rhs) 
        D3 = And( Implies( r1[i][j], rhs ), D3)

ddd = time.time() - starttime - dd
print "D3 took", str(ddd)

sys.exit()

#Init = (presence_edge[0][1][0] == True)
#Init2 = (p[0][3] == False)
#Init3 = (p[1][2] == False)
# Create Solver and add constraints in it.

s = Solver()
s.add(A0,A1, C0,C1,C2,C3,C4,C5, F0,F1,F2,F3,F4,F5, D0,D1,D2,D3)
print "solving...\n"
print "Printing the assertions..."
#for c in s.assertions():
#    print c
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


