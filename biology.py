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
parser.add_argument("-Q","--pedges", type=int, default=2, help = "max no.of parallel edges btw two nodes")
parser.add_argument("-V","--variation", type=int, default=1, help = "model of the biological system")
parser.add_argument("-C","--connected", type=int, default=3, help = "graph connectivity want to check")
args = parser.parse_args()

M = args.mols
N = args.nodes
Q = args.pedges
V = args.variation
C = args.connected

bag = []
bag.append ('1. Boolean function on nodes and Boolean function on the edges.') 
bag.append ('2. Only Boolean function on the edges.') 
bag.append ('3. Boolean function on nodes and SNARE-SNARE inhibition on edges.')
bag.append ('4. Only SNARE-SNARE inhibition on edges.')
bag.append ('5. Only Boolean function on nodes.')
bag.append ('6. No regulation at all.')

def print_variation(): 
    print ' '
    for i in bag:
        print i 
    print ' '
    print 'Running the chosen variation... \n' 

def wall(v):
    print bag[v-1]
    print 'Use -V option to change the regulated variation.' 
    print_variation()

print 'Default/choosen vesicular traffic system is regulated by ...'
for i in range(6):
    if V == i:
        wall(V)
        break

#----------------------------------------------------
# Constraint generation 
dump = [ [ [Bool ("d_{}_{}_{}".format(i,j,q)) for q in range(Q)] for j in range(N)] for i in range(N)]

node = [ [Bool ("n_{}_{}".format(i,k)) for k in range(M)] for i in range(N)]

active_node = [ [Bool ("a_{}_{}".format(i,k)) for k in range(M)] for i in range(N)]

edge = [ [ [ Bool ("e_{}_{}_{}".format(i,j,q)) for q in range(Q)] for j in range(N)] for i in range(N)]

presence_edge = [ [ [ [Bool ("e_{}_{}_{}_{}".format(i,j,q,k)) for k in range(M)] for q in range(Q)] for j in range(N)] for i in range (N)]

active_edge = [ [ [ [Bool ("b_{}_{}_{}_{}".format(i,j,q,k))  for k in range(M)] for q in range(Q)] for j in range(N)] for i in range (N)]

r = [ [ [ [Bool ("r_{}_{}_{}_{}".format(i,j,k,z)) for z in range(N-1)] for k in range(M)] for j in range(N)] for i in range (N)]

p = [ [Bool ("p_{}_{}".format(k,k1)) for k1 in range(M)] for k in range(M)]

r1 = [ [Bool ("r1_{}_{}".format(i,j)) for j in range(N)] for i in range(N)]

sorts = [BoolSort() for m in range(M)]

f_n = [Function ("an_{}".format(m), *sorts) for m in range(M)] 
f_e = [Function ("ae_{}".format(m), *sorts) for m in range(M)] 

# Few additional condition that needs to be met.

starttime = time.time()

# Function: no regulation on the node.
# The present molecules at nodes are all active.
def f_nn():
    A_list = []
    for i in range(N):
        for k in range(M):
            lhs = node[i][k] == active_node[i][k]
            A_list.append(lhs)
    A0 = And(A_list)

# No regulation on edge.
# The present molecules on the edge are all active.
def f_ne():
    A_list = []
    for i in range(N):
        for j in range(N):
            if i == j:
                continue
            for q in range(Q):
                for k in range(M):
                    lhs = presence_edge[i][j][q][k] == active_edge[i][j][q][k]
                    A_list.append(lhs)
    A1 = And(A_list)

# Activity of a moolecule k on a node/edge is  
# defined as a Boolean function of presence of 
# Other molecule present on that node/edge.
# A0. active_node[k] =  f_n[k](\/_{k1 != k} node(k1)) 
def f_bn():
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
            A_list.append(l)
    A0 = And( A_list )

# Activity of the molecules on the edge is driven by the 
# chosen boolean function. 
# A1. active_edge[k] = f_e[k](\/_{k1 != k} presence_edge(k1))
def f_be():
    s = []
    A_list = []
    for i in range(N):
        for j in range(N):
            if i == j:
                continue
            for q in range(Q):
                for k in range(M):
                    del s[:]
                    for k1 in range(M):
                        if k1 == k:
                            continue
                        s.append(presence_edge[i][j][q][k1])
                    l  =  Implies(presence_edge[i][j][q][k], active_edge[i][j][q][k] == f_e[k](*s)) 
                    A_list.append(l)
    A1 = And( A_list )

# Inhibition of the edges are driven by the pairing matrix.
# And(p[k][k1] -> edge[k]) -> active[k]
def f_se():
    A_list = []
    for i in range(N):
        for j in range(N):
            if i == j:
                continue
            for q in range(Q):
                for k in range(M):
                    l = True
                    for k1 in range(M):
                        if k == k1:
                            continue
                        l = And (Implies( p[k][k1], presence_edge[i][j][q][k1]),l)
                    l1 = Implies( l, Not(active_edge[i][j][q][k]))
                    l2 = Implies ( Not(l), active_edge[i][j][q][k])
                    A_list.append(And(l1,l2))
    A1 = And(A_list)

A0 = True 
A1 = True
def regulation(v):
    if v == 1:
        f_bn() 
        f_be()
    elif v == 2:
        f_nn()
        f_be()
    elif v == 3:
        f_bn()
        f_se()
    elif v == 4:
        f_n()
        f_se()
    elif v == 5:
        f_bn()
        f_ne()
    else:
        f_nn()
        f_ne()

regulation(V)

a = time.time() - starttime 
print "A1 took", str(a)


#1. label(E) subset label(N)
# e_ijk -> aik
# e_ijk -> ajk

# -- Updated.
# e_ijk -> a_ik and a_jk
C1 = True
for i in range(N):
    for j in range(N):
        if i == j :
            continue
        for q in range(Q):
            for k in range(M):
                C1 = And (Implies( presence_edge[i][j][q][k], And( node[i][k], node[j][k]) ),C1)


#2. Self edges not allowed. 
# not e_ii  
C2 = True
for i in range(N):
    for q in range(Q):
        C2 = And(Not(edge[i][i][q]), C2)


#3. Multiple(parallel) edges are allowed between two nodes. 
# But we restrict it to two.

#-- Update:
# Implemented explicitely using another dimension in edge [:Q].
#C3 = True

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

c = time.time() - starttime - a
print "C0-C5 Took", str(c)

# ----------------------------------------------------------------

# MAIN Constraints:

# F_0
# \/_k e_ijk -> e_ij  at evry edge there is an active molecules which is present.
# Involve equality and we'll might  it while doing validity check.
F0 = True
for i in range(N):
    for j in range(N):
        if j == i:
            continue
        for q in range(Q):
            rhs = False
            for k in range(M):
                rhs = Or( presence_edge[i][j][q][k], rhs )
            F0 = And (Implies(rhs, edge[i][j][q]),F0)
#print F0


# F1:  b_ijk -> e_ijk   
# If molecule is active on an edge then it should be present on the edge. \newline
F1 = True
F1_list = []
for i in range(N):
    for j in range(N):
        if j == i:
            continue
        for q in range(Q):
            for k in range(M):
                F1_list.append( Implies(active_edge[i][j][q][k], presence_edge[i][j][q][k]) )
F1 = And( F1_list )
#print F1

f = time.time() - starttime - c
print "F0-F1 took ", str(f)

# ---- Rule change proposed----
# for_all i,j,q,k : e(i,j,q,k) -> exists(r(j,i,k,z)) 
# for every molecule present on a edge comes back in a cycle.
F3 = True 
A_list = []
for i in range(N):
    for j in range(N):
	if i == j:
	    continue
        for q in range(Q):
            for k in range(M):
                lhs = False
                for z in range(N-1):
                    lhs = Or(r[j][i][k][z], lhs)
                l  = Implies(presence_edge[i][j][q][k],lhs)
                A_list.append(l)
F3 = And(A_list)
# print F3

#F2 New reachability and stability condition-----
# States reachability definition: 
# nodes i,j is reachable with kth molecule in z steps if 
# there is an edge between i'' and j (i != i'') with k present on that edge.
# and i'' is reachable from i in z steps.

#-- Update with multiple edges.
# for_all i,j,k,z : if z = 0 -> [r(i,j,k,0) -> exists q : (e(i,j,q,k)) -> ]  
#                   else: r(i,j,k,z) -> exists l,q (e(i,l,q,k) and r(l,q,k,z-1)) 

#--- Old specifiction.
# /\_{i,j,k,p, i!=j} r(i,j,k,p) -> \/_{l!=i || j}  e(i,l,k) and r(l,j,k-1)

# Have also added one length reachability with F2. 
# i,j is reachaable in one step if there is a direct edge between them.
# r_{i,j,k,1} -> e_{i,j,k} : In our case 1 is 0.
F2 = True
A_list = []
for i in range(N):
    for j in range(N):
	if i == j:
	    continue
        for k in range(M):
            for z in range(N-1):
                pe = False
                if z == 0: 
                    for q1 in range(Q):
                        pe = Or( presence_edge[i][j][q1][k], pe)
                    A_list.append( Implies(r[i][j][k][0], pe) ) 
                else:
                    lhs = False
                    for l in range(N):
                        if i == l or j == l:
                            continue
                        for q1 in range(Q):
                            pe = Or( presence_edge[i][l][q1][k], pe)
                        lhs = Or(And ( r[l][j][k][z-1],pe), lhs)   
                    w  = Implies( r[i][j][k][z],lhs)
                    A_list.append(w)
F2 = And(A_list)
#print F2

ff = time.time() - starttime 
print "F2-F3 took ", str(ff)

# F4: Fusion rules:
# For an edge to be valid, at least one
# SNARE pair on the vesicle and target compartment must be active, and have a
# non-zero entry in the pairing matrix

# -- Updated specification.
# forall_{i,j,q} e(i,j,q) -> exists_{k,k'}: b(i,j,q,k) and a(j,k') and p(k,k')

# /\_{i,j} ( \/_k e_{i,j,k}) -> \/_{k,k'} (b_{i,j,k} and a'_{j,k'} and p_{k,k'}) 
# and /\_k b_{i,j,k} -> not(\/_{j' != j} (\/_k'' a'_{j',k''} and p_{k,k''}))  
F4 = True
A_list = []
for i in range(N):
    for j in range(N):
	if i == j:
	    continue
        for q in range(Q):
	    lhs = False    
            for k in range(M):
	        for k1 in range(M): 
                    if k == k1:
                        continue
	            lhs = Or (  And( active_edge[i][j][q][k], And ( active_node[j][k1],p[k][k1]) ), lhs)  
            w =  Implies(edge[i][j][q],lhs)
            A_list.append(w)
F4 = And(A_list)
#print F4

# Fusion respects the graph structure by the vesicle (edge) under consideration, 
# fusing only with its target node and not with any other node.

# --Updated specification with multiple edges.
# forall_{i,j,q,k} b(i,j,q,k) -> ~ [exits_{j',k"}: a(j',k") and p(k,k")]
F5 = True
A_list = []
for i in range(N):
    for j in range(N):
	if i == j:
	    continue
        for q in range(Q):
            for k in range(M): 
                lhs = False
	        for j1 in range(N):
                    if j == j1:
                        continue
	            for k11 in range(M):
                        if k == k11:
                            continue
		        lhs = Or(And ( active_node[j1][k11], p[k][k11]), lhs)
	        w = Implies( active_edge[i][j][q][k], Not(lhs))
                A_list.append(w)
F5 = And(A_list)
#print F5

#fff = time.time() - starttime - ff
#print "F2-F3 took ", str(fff)

starttime = time.time()

#----3 Connectivity --------
# Summation of d_{i,j} == 2

# -- Trivial implementaion in Haskell. 
# At Most two
# [Or x | x = Or (d1[i] and d1[j] and d1[k]),   i <- [0..L], j <- [x+1..L-1] , k <- [j+1,L]]
# At least two
# [Or(x) | x = d1[i] and d1[j] , i <- [0..L], j <- [0..L] , i!=j]
#D1 = [Or(And(d1[x],d1[y]), False) for x in range(L)  y in range(L) x != y]

# -- Coolest one: encoding in CBMC.
#D1 = 0
#for i in range(N):
#    for j in range(N):
#        D1 = dump[i][j] + D1 

# In z3Py. 
# First step: Need flatening.
d1 = [dump[i][j][q] for i in range(N) for j in range(N) for q in range(Q)]

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

#def at_least(u):
#    for i in range(L):
#        for j in range(i+1,L):
#            D0 = Implies ( d1[i][j]  
#            
#
#def exactly_u(u):
#    at_least(u)
#    at_most(u)
#
#exactly_n(C-1)

# At least 2
D1 = False
for i in range(L):
  for j in range(i+1,L):
    D1 = Or( And( d1[i], d1[j]), D1)


# Only present edges can be dropped.
D2 = True
for i in range(N):
    for j in range(N):
        if i == j:
            continue
        for q in range(Q):
            D2 = And ( Implies (dump[i][j][q], edge[i][j][q]), D2) 

#print C1

# ---- ---- --- 

# Graph becomes disconnected.
# Ensure that there is no path between some nodes i,j 
#                 in the underlying undirected graph. 

# ~ [forall_{i,j} r1(i,j) Or r1(j,i)]
D4 = True
for i in range(N):
    for j in range(N):
        if i == j:
            continue
        rijji = Or(r1[i][j],r1[j][i])
        D4 = And( rijji, D4) 
D4 = Not(D4)

print ' '
#print D4

# forall_{i,j} r'(i,j) -> exists_{i'!=i}: [r'(i',j) and e(i,i') and ~d(i,i')]

# --Updated specification.
# New reachability definition for grph connectedness: dReachable
# Node i,j are reachable either if there is a direct edge and its not dropped. 
# Or there is an node i' such that, there is a direct edge between i,i' which is not droped
# and i' and j is dReachable.

# forall_{i,j} r'(i,j) -> exists_{q} [e(i,j,q) and not d(i,j,q)] Or  exists_{i'!=i}: [r'(i',j) and [exists_{q}: e(i,i',q) and ~d(i,i',q)] ]   
# do we need length reach?
D3 = True
A_list = []
for i in range(N):
    for j in range(N):
        if i == j:
            continue
        rhs = False
        bhs = False
        for q in range(Q):
            bhs = Or( And( edge[i][j][q], Not(dump[i][j][q]) ), bhs) 
        for l in range(N):
            if i == l or j == l:
                continue
            for q in range(Q):
                rhs = Or( And( edge[i][l][q], Not(dump[i][l][q])), rhs)
            rhs = And( rhs, r1[l][j])
        w = Implies( r1[i][j], Or (bhs , rhs) )
        A_list.append(w)
D3 = And(A_list)
print D3

# Initial D3 u have encoded.
# do we need length reach?
#D3 = True
#for i in range(N):
#    for j in range(N):
#        rhs = And( edge[i][j], Not(dump[i][j]) )
#        for l in range(N):
#            if i == l:
#                continue
#            rhs = Or( And(And(edge[i][l],Not(dump[i][l])),r1[l][j] ), rhs) 
#        D3 = And( Implies( r1[i][j], rhs ), D3)
#

# HAVE TO CHECK THE EDGE in case of q = 0 ?? 

#ddd = time.time() - starttime - fff
#`print "D3 took", str(ddd)

# sys.exit()

#Init = (presence_edge[0][1][0] == True)
#Init2 = (p[0][3] == False)
#Init3 = (p[1][2] == False)
# Create Solver and add constraints in it.

s = Solver()
s.add(A0,A1,C1,C2,C4,C5, F0,F1,F2,F3,F4,F5, D0,D1,D2,D3,D4)
print "solving...\n"
#print "Printing the assertions..."
#for c in s.assertions():
#    print c
print s.check()
#print "\nPrinting the statistics..."
#print s.statistics() 
#print "\n"

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
            for q in range(Q):
                for k in range(M):
                    if q == 0:
                        if is_true(m[presence_edge[i][j][q][k]]):
                            label = str(k)
                            color = "black"
                            if is_true(m[active_edge[i][j][q][k]]):
                                color = "green"
                                for k1 in range(M):
                                    if is_true(m[active_node[j][k1]]) and is_true(m[p[k][k1]]):
                                        color = "red"
                                        break
                            dfile.write( str(i) + "-> " + str(j) + "[label=" + label +",color=" + color +"]" +"\n" )
                    if q == 1:
                        if is_true(m[presence_edge[i][j][q][k]]):
                            label = str(k)
                            color = "yellow"
                            if is_true(m[active_edge[i][j][q][k]]):
                                color = "pink"
                                for k1 in range(M):
                                    if is_true(m[active_node[j][k1]]) and is_true(m[p[k][k1]]):
                                            color = "blue"
                                            break
                            dfile.write( str(i) + "-> " + str(j) + "[label=" + label +",color=" + color +"]" +"\n" )

    dfile.write("}\n")

if s.check() == sat:
    m = s.model()
#    print "Printing the model..."
#    for d in m.decls():
#        print "{} = {}".format(d.name(), m[d])
    print m
    r = [ [ m[p[i][j]] for j in range(M) ]
          for i in range(M) ]
    s = [[ [ [m[active_edge[i][j][q][k]] for k in range (M)] for q in range(Q)] for j in range(N) ] for i in range(N) ]
    t = [ [ [m[edge[i][j][q]]  for q in range(Q)] for j in range(N) ]
          for i in range(N) ]
#    print "\n Pairing matrix p[{}][{}] = ".format(M,M)
#    print_matrix(r)
#    print "\n Edge  e[{}][{}] = ".format(N,N)
#    print t
    dump_dot( "/tmp/bio.dot", m )
else:
    print "failed to solve"


