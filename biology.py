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

print dump(0,2) 

print dump(3,2) 

# F_0

# at_least_one = False
# for i in range (1,N):
#    at_least_one = at_least_one or dump(i,i)

# F_0:  e_ijk -) e_ijk   
for i in range(1,N):
    for j in range(1,N):
        for k in range(1,M):
            Implies(active_edge(i,j,k), edge(i,j,k))
# Not(active_edge(i,j,k)) Or edge(i,j,k)


# F_1
B1,B2, B22, B3, B33 = Bools('B1 B2 B22 B3 B33') 
B1 = False
B2 = False
B22 = False
B3 = False
B33 = False
for i in range(1,N):
    for j in range(1,N):
        for k in range(1,M):
            if (r(i,j,k) == False):
                    continue
            for l in range(1,N):
                if(i == l):
                    continue  
                B1 = Or(And(r(l,j,k), edge(i,l,k)), B1)
                print B1
                #B1 = B1 or B2                
                
# F_1_1
for i in range(1,N):
    for j in range(1,N):
        for k in range(1,M):
            Implies(edge(i,j,k), r(j,i,k))
# F_2
for i in range(1,N):
    for j in range(1,N):
        for k in range(1,M):
            B2 = Or (B2, edge(i,j,k)) 
            for k1 in range(1,M):
                B22 = Or (B22, And (active_edge(i,j,k), active_node(j,k1), pair_matrix(k,k1)))
                print B22

#(not B2 or B22)  


# F_3
for i in range(1,N):
    for j in range(1,N):
        for k in range(1,M):
            B3 = Or(B3, edge(i,j,k)) 
            for k1 in range(1,M):
                B33 = Or(B33, And(active_edge(i,j,k), active_node(j,k1), pair_matrix(k,k1)))
            print B33
#        B33 = not(B33)

##const_c = F_0 +  F_1_1
#bio_c = B1 and (not(B2) or B22) and (not(B3) or not(B33))

s = Solver()
s.add(B1 and B2 and B22 and B33)
#s.add(bio_c)
if s.check() == sat:
#    m = s.model()

##solve(dump(0,2) or not dump(2,0))

