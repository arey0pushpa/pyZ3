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

def node(i,k):
    s = "n_" + str(i) + "_k" + str(k)
    return Bool( s )

def active_node(i,k):
    s = "a_" + str(i) + "_k" + str(k)
    return Bool( s )

def real_edge(i,j):
    s = "e_" + str(i) + "_" + str(j)
    return Bool( s )

def edge(i,j,k):
    s = "e_" + str(i) + "_" + str(j) + "_k" + str(k)
    return Bool( s )

def active_edge(i,j,k):
    s = "b_" + str(i) + "_" + str(j) + "_k" + str(k)
    return Bool( s )

def pair_matrix(k,k1):
    s = "p_k" + str(k) + "_k" + str(k1)
    return Bool( s )

def r(i,j,k):
    s = "r_" + str(i) + "_" + str(j) + "_k" + str(k)
    return Bool( s )


# F_0
# b_ij > \/_k b_ijk
F00 = True
for i in range(N):
    for j in range(N):
        rhs = False
        for k in range(M):
            rhs = Or( edge(i,j,k), rhs )
        F00 = Implies( real_edge(i,j), rhs )
print F00


# F_0:  b_ijk -> e_ijk   
F0 = True
for i in range(N):
    for j in range(N):
        for k in range(M):
            F0 = And( Implies(active_edge(i,j,k), edge(i,j,k)), F0 )
print F0


# F_1
# reachability condition; fixed point issues??
B1 = False
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
            B1 = Implies( r(i,j,k), rhs )
print B1

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
B2 = False
B22 = False
for j in range(N):
    for j1 in range(N):
        if j1 == j:
            continue
        for i in range(N):
            for i1 in range(N):
                for k in range(M):
                    for k1 in range(M):
            B2 = Or (B2, edge(i,j,k)) 
            for k1 in range(1,M):
                B22 = Or (B22, And (active_edge(i,j,k), active_node(j,k1), pair_matrix(k,k1)))
print B22



s = Solver()
s.add(B1,B2,B22, B33)
print s.check()

if s.check() == sat:
    m = s.model()
    print m
    

