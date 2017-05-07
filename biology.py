# from __future__ import print_function
from z3 import *
import argparse

# def printf(str, *args):
#     print(str % args, end='')

#----------------------------------------------------
# input parsing
# input number of nodes and molecules
parser = argparse.ArgumentParser(description='Auto testing for TARA')
parser.add_argument("-M","--mols", type=int, default=3, help = "number of molecules")
parser.add_argument("-N","--nodes", type=int, default=3, help = "number of nodes")
parser.add_argument("-E","--edges", type=int, default=3, help = "number of edges")
args = parser.parse_args()
M = args.mols
N = args.nodes
E = args.edges
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
            C1 = And (Implies(egde(i,j,k),node(i,k)),C1)

#2. Self edges not allowed. 
# not e_ii  

for i in range(N):
    B2 = Not(edge(i,i))

#3. Multiple(parallel) edges are allowed between two nodes. 
# But we restrict it to two.

#4. Condition on p_kk' : 
# we have two options 1. build the matrix as M * M or
# 2. divide into two matrix M(qp) {p1} M(pq) {p2} each is M/2 size
# I implemented as 2nd.  p1_kk' , P2_kk' but now u cant say k , k' you have to specify which k and k'?  
# To avoid breaking current setup : using 1

# if (k,k' belongs to same half of the M * M matrix == 0
# else (k,k') = nondet_Bool()
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

# F_2: Fusion Rules 
# for_all(j,j'){j != j'} ( exists(k,k') p[k][k'] and (b[i][j][k] and a[j][k'] != b[i'][j'][k] and a[j'][k']))
#F2 = False
#for j in range(N):
#    for j1 in range(N):
#        if j1 == j:
#            continue
#        for i in range(N):
#            for i1 in range(N):
#                for k in range(M):
#                    for k1 in range(M):
#                       F2 =  And (p(k,k1), And(active_edge(i,j,k),active_node(j,k1) And(active_edge(i1,j1,k), active_node(j1,k1))), F2) 
#print F2

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
                lhs = And(active_edge(i,j,k),active_node(i,k),p(k,k1)) 
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
#s.add(B2,B22, B33)
print s.check()

if s.check() == sat:
    m = s.model()
    print m
    

