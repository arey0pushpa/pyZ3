# from __future__ import print_function
from z3 import *
import argparse

# def printf(str, *args):
#     print(str % args, end='')

#----------------------------------------------------
# input parsing
# input number of nodes and molecules
parser = argparse.ArgumentParser(description='Auto testing for TARA')
parser.add_argument("-M","--mols", type=int, default=5, help = "number of molecules")
parser.add_argument("-N","--nodes", type=int, default=3, help = "number of nodes")
#parser.add_argument("-E","--edges", type=int, default=3, help = "number of edges")
args = parser.parse_args()
M = args.mols
N = args.nodes
#E = args.edges
# printf( "Molecules : %d Nodes : %d\n", M, N )


#----------------------------------------------------
# Constraint generation 

def dump(i,j):
    s = "d_" + str(i) + "_" + str(j)
    return Bool( s )

# c(i,k) node with kth molecule present
def node(i,k):
    s = "n_" + str(i) + "_k" + str(k)
    return Bool( s )

# c(i,k) node with kth molecule active
def active_node(i,k):
    s = "a_" + str(i) + "_k" + str(k)
    return Bool( s )

# e(i,j) edge between node i and j
def real_edge(i,j):
    s = "e_" + str(i) + "_" + str(j)
    return Bool( s )

# e(i,j,k) edge with kth molecule present
def edge(i,j,k):
    s = "e_" + str(i) + "_" + str(j) + "_k" + str(k)
    return Bool( s )

# b(i,j,k) edge with kth molecule active
def active_edge(i,j,k):
    s = "b_" + str(i) + "_" + str(j) + "_k" + str(k)
    return Bool( s )

# PAIR MATRIX p(k,k'): Matrix has a non entry at k,k1 position.
def p(k,k1):
    s = "p_k" + str(k) + "_k" + str(k1)
    return Bool( s )

# REACHABLE: node j is reachable from node i with kth molecule 
#            present on the taken path. 
def r(i,j,k):
    s = "r_" + str(i) + "_" + str(j) + "_k" + str(k)
    return Bool( s )

# ---------------------------------------------------------------

# Few additional condition that needs to be met.

#1. label(E) subset  label(N)
# e_ijk -> aik
C1 = True
for i in range(N):
    for j in range(N):
        for k in range(M):
            C1 = And (Implies(edge(i,j,k),node(i,k)),C1)

C2 = True
for i in range(N):
    for j in range(N):
        for k in range(M):
            C2 = And (Implies(edge(i,j,k),node(j,k)),C2)

#2. Self edges not allowed. 
# not e_ii  

for i in range(N):
    C3 = Not(real_edge(i,i))


#3. Multiple(parallel) edges are allowed between two nodes. 
# But we restrict it to two.
# At most two implementation: \/_{x,y,z} ~(x=y,y=z,z=x)
#C4 = True 
#for x in range(N):
#    for y in range(N):
#        for z in range(N):
##            edge(
# Need to change the defination of the function to Int

#4. Condition on p_kk' : 
# if (k,k' belongs to same half of the M * M matrix == 0
# else (k,k') = nondet_Bool()

# \/_{x<M/2,y>=M/2} !p(x,x) and !p(y,y)
M1 = M/2
C4 = True
for x in range(M):
    for y in range(M):
        if (x < M1 and x <= y) or (x>M1 and x>=y):
            C4 = And(Not (p(x,y)),C4)

#5. Activitu on the node.

C5 = True 
for i in range(N):
    for k in range(M):
        C5 = And (Implies (active_node(i,k),C5)) 

# ----------------------------------------------------------------

# MAIN Constraints:

# F_0
# e_ij -> \/_k b_ijk  ?? or e_ijk ??
# at evry edge there is an active molecules which is present.
F00 = True, 
for i in range(N):
    for j in range(N):
        rhs = False
        for k in range(M):
            rhs = Or( edge(i,j,k), rhs )          # Change it to active_edge(i,j,k)
        F00 = Implies( real_edge(i,j), rhs )
print F00


# F_0:  b_ijk -> e_ijk   
# If molecule is active on an edge then it should be present on the edge. \newline

F0 = True
for i in range(N):
    for j in range(N):
        for k in range(M):
            F0 = And( Implies(active_edge(i,j,k), edge(i,j,k)), F0 )
print F0


# F_1
# reachability condition; recursive definition. 
# fixed point issues?? Check!
F11 = False
for i in range(N):
    for j in range(N):
        if j == i:
            continue
        for k in range(M):
            # if (r(i,j,k) == False):
            #         continue
            rhs = edge(i,j,k)
            for l in range(N):
                if i == l or j == l:
                    continue  
                rhs = Or(And(r(l,j,k), edge(i,l,k)), rhs)
            F11 = Implies( r(i,j,k), rhs )
print F11

F1 = True                
# F_1_1
# stability condition 
# e_ijk -> r_jik
for i in range(N):
    for j in range(N):
        if j == i:
            continue
        for k in range(M):
           F1 = And( Implies(edge(i,j,k), r(j,i,k)), F1)


# F_2: Fusion rules:
# /\_{i,j} ( \/_k e_{i,j,k}) -> \/_{k,k'} (b_{i,j,k} and a'_{j,k'} and p_{k,k'}) 
# and /\_k b_{i,j,k} -> not(\/_{j' != j} (\/_k'' a'_{j',k''} and p_{k,k''}))  
F2 = True
for i in range(N):
    for j in range(N):
        rhs = False
        for k in range(M):
            rhs = Or(edge(i,j,k),rhs) 
        
        for k1 in range(M): 
            for k2 in range(M):
                lhs = And(active_edge(i,j,k1),active_node(i,k1),p(k1,k2)) 
        F2 = And (Implies(rhs,lhs) , F2)
print F2

F22 = True
for i in range(N):
    for j in range(N):
        rhs = False
        for k in range(M):
            rhs = Or(active_edge(i,j,k),rhs)
            lhs = False
            for j1 in range(N):
                if j == j1:
                    continue
                for k11 in range(M):
                    lhs = Or(And (active_node(j1,k11),p(k,k11)), lhs)
        F22 = And (Implies(rhs, Not(lhs)), F22)

# Create Solver and add constraints in it.
s = Solver()
s.add(C1,C2,C3,C4,F0,F00,F1,F11,F2,F22)
print s.check()

if s.check() == sat:
    m = s.model()
 #   e = [ [ m.evaluate(real_edge[i][j]) for j in range(M) ]
          #            for i in range(M) ]
#    p = [ [ m.evaluate(p[i][j]) for j in range(M) ] for i in range(M)]
 #   print p
 #   print r
    print m
else:
    print "failed to solve"

    

