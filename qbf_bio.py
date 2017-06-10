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

for i in range(6):
    if V == i + 1:
        print ' '
        print 'Default/choosen vesicular traffic system is regulated by ...',
        print bag[i]
        print ' '
        print 'Use -V option to change the regulated variation.' 
        for i in bag:
            print i 
        print ' '
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


# Basic constraint on nodes and edge.
C0 = ForAll ([0<=i<N,0<=j<N,0<=k<M], Implies ( presence_edge[i][j][k], node[i][k]) )

C1 = ForAll ([i,j,k], Implies ( presence_edge[i][j][k], node[j][k]) )

C2 = ForAll (i, Not( presence_edge[i][i]) )

C2_List = []
for i in N:
    C_List.append( Not( presence_edge[i][i]) )
C2 = And( C2_List )

#C4 =  

s = Solver()
s.add(C0,C1,C2)
print "solving...\n"
print s.check()
 



