#!/usr/bin/python

from z3 import *
import argparse

#----------------------------------------------------
# input parsing
# input number of nodes and molecules
parser = argparse.ArgumentParser(description='Auto testing for TARA')
parser.add_argument("-M","--mols", type=int, default=12, help = "number of molecules")
parser.add_argument("-N","--nodes", type=int, default=3, help = "number of nodes")
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

r = [ [ [Bool ("r_{}_{}_{}".format(i,j,k)) for k in range(M)] for j in range(N)] for i in range (N)]

p = [ [Bool ("p_{}_{}".format(k,k1)) for k1 in range(M)] for k1 in range(M)]


# Initial Constraints :

#1. label(E) subset  label(N)
# e_ijk -> aik # e_ijk -> ajk

C0 = [Implies(presence_edge[i][j][k],node[i][k]) for i in range(N) for j in range(N) for k in range(M)]
C1 = [Implies(presence_edge[i][j][k],node[j][k]) for i in range(N) for j in range(N) for k in range(M)]


#2. Self edges not allowed. 
# not e_ii  

C2 = [Not(edge[i][i]) for i in range(N)]

#3. Multiple(parallel) edges are allowed between two nodes. 
# But we restrict it to two.

C3 = True

#4. Condition on p_kk' : first and last cube is empty.
# \/_{x<M/2,y>=M/2} !p(x,y) and !p(x,y)

C4 = [ Not(p[x][y])  for x in range(M) for y in range(M) if (x < (M/2) and x <= y) or (x > (M/2) and x>=y) ] 
print C4

#5. Activity on the node.
# if molecule is active on the node then its present. a(i,k) -> n(i,k) 

C5 = [Implies (active_node[i][k], node[i][k]) for i in range(N) for k in range(M)] 


# MAIN Constraints:

# F_0
# at evry edge there is an active molecules which is present. # \/_k e_ijk -> e_ij 

F0 = [Implies(rhs, edge(i,j) ) for i in range(N) for j in range(N) if j != i else continue]


F00 = True, 
for i in range(N):
    for j in range(N):
        if j == i:
            continue        
        rhs = False
        for k in range(M):
            rhs = Or( presence_edge(i,j,k), rhs )
        F00 = Implies(  rhs, edge(i,j) )
# print F00


# F_0:  b_ijk -> e_ijk   
# If molecule is active on an edge then it should be present on the edge. \newline

F0 = True
for i in range(N):
    for j in range(N):
        for k in range(M):
            F0 = And( Implies(active_edge(i,j,k), presence_edge(i,j,k)), F0 )
# print F0


# F_1
# reachability condition; recursive definition. 
# fixed point issues?? Check!
F11 = True
for i in range(N):
    for j in range(N):
        if j == i:
            continue
        for k in range(M):
            rhs = edge(i,j,k)
            for l in range(N):
                if i == l or j == l:
                    continue  
                rhs = Or(And(r(l,j,k), presence_edge(i,l,k)), rhs)
            F11 = And( Implies( r(i,j,k), rhs ), F11 )
# print F11

F1 = True                
# F_1_1
# stability condition 
# e_ijk -> r_jik
for i in range(N):
    for j in range(N):
        if j == i:
            continue
        for k in range(M):
           F1 = And( Implies(presence_edge(i,j,k), r(j,i,k)), F1)


# F_2: Fusion rules:
# /\_{i,j} ( \/_k e_{i,j,k}) -> \/_{k,k'} (b_{i,j,k} and a'_{j,k'} and p_{k,k'}) 
# and /\_k b_{i,j,k} -> not(\/_{j' != j} (\/_k'' a'_{j',k''} and p_{k,k''}))  
F2 = True
for i in range(N):
    for j in range(N):
        if i == j:
            continue
        rhs = False
        for k in range(M):
            rhs = Or(presence_edge(i,j,k),rhs)        
        for k1 in range(M): 
            for k2 in range(M):
                lhs = And(active_edge(i,j,k1),active_node(i,k1),p(k1,k2)) 
        F2 = And (Implies(rhs,lhs) , F2)
# print F2

F22 = True
# for i in range(N):
#     for j in range(N):
#         if i == j:
#             continue
#         rhs = False
#         for k in range(M):
#             rhs = Or(active_edge(i,j,k),rhs)
#             lhs = False
#             for j1 in range(N):
#                 if j == j1:
#                     continue
#                 for k11 in range(M):
#                     lhs = Or(And (active_node(j1,k11),p(k,k11)), lhs)
#         F22 = And (Implies(rhs, Not(lhs)), F22)

Init = (edge(0,1,0) == True)

# Create Solver and add constraints in it.
s = Solver()
s.add(C1,C2,C3,C4,F0,F00,F1,F11,F2,F22,Init)
print "solving...\n"
print s.check b
print "done\n"


def dump_dot( filename, m ) :
    dfile = open(filename, 'w')
    dfile.write("digraph prog {\n")
    for i in range(N):
        node_vec = ""
        for k in range(M):
            if is_true(m[node(i,k)]) :
                node_vec = node_vec + "1"
            else:
                node_vec = node_vec + "0"
            if is_true(m.evaluate(active_node(i,k))) :
                node_vec = node_vec + "-"
        dfile.write( str(i) + "[label=\"" + node_vec + "\"]\n")
        for j in range(N):
            if i == j:
                continue
            for k in range(M):
                if is_true(m[presence_edge(i,j,k)]):                
                    dfile.write( str(i) + "-> " + str(j) + "[label=" + str(k) +"]" +"\n" )
    dfile.write("}\n")
    for k2 in range(M):
        print "\t"+str(k2),
    print "\n"
    for k1 in range(M):
        for k2 in range(M):
            if is_true(m[p(k1,k2)]):
                print "\t0",
            else:
                print "\t1",
        print "\n"

if s.check() == sat:
    m = s.model()
    print m
    dump_dot( "/tmp/bio.dot", m )
else:
    print "failed to solve"

    

