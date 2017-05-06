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

#f = (dump(0,2) and dump(3,0))


# F_0
# b_ij > \/_k b_ijk
F00 = True
for i in range(1,N):
    for j in range(1,N):
        rhs = False
        for k in range(1,M):
            rhs = Or( edge(i,j,k), rhs )
        F00 = Implies( real_edge(i,j), rhs )
print F00


# F_0:  b_ijk -> e_ijk   
F0 = True
for i in range(1,N):
    for j in range(1,N):
        for k in range(1,M):
            F0 = And( Implies(active_edge(i,j,k), edge(i,j,k)), F0 )
print F0


# F_1
# reachability condition; fixed point issues??
B1 = False
for i in range(1,N):
    for j in range(1,N):
        if j == i:
            continue
        for k in range(1,M):
            # if (r(i,j,k) == False):
            #         continue
            rhs = edge(i,j,k)
            for l in range(1,N):
                if i == l or j == l:
                    continue  
                rhs = Or(And(r(l,j,k), edge(i,l,k)), rhs)
            B1 = Implies( r(i,j,k), rhs )

print B1
                
# F_1_1
# stability condition 
# e_ijk -> r_jik
for i in range(1,N):
    for j in range(1,N):
        if j == i:
            continue
        for k in range(1,M):
            Implies(edge(i,j,k), r(j,i,k))

# F_2
B2 = False
B22 = False
for i in range(1,N):
    for j in range(1,N):
        for k in range(1,M):
            B2 = Or (B2, edge(i,j,k)) 
            for k1 in range(1,M):
                B22 = Or (B22, And (active_edge(i,j,k), active_node(j,k1), pair_matrix(k,k1)))
print B22


# F_3
B3 = False
B33 = False
for i in range(1,N):
    for j in range(1,N):
        for k in range(1,M):
            B3 = Or(B3, edge(i,j,k)) 
            for k1 in range(1,M):
                B33 = Or(B33, And(active_edge(i,j,k), active_node(j,k1), pair_matrix(k,k1)))
print B33

##const_c = F_0 +  F_1_1

s = Solver()
s.add(B1,B2,B22, B33)
print s.check()

if s.check() == sat:
    m = s.model()
    print m
    

