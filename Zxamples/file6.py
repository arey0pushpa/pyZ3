# from __future__ import print_function
from z3 import *
import argparse

# def printf(str, *args):
#     print(str % args, end='')

#----------------------------------------------------
# input parsing
# input number of nodes and molecules
parser = argparse.ArgumentParser(description='Auto testing for TARA')
parser.add_argument("-M","--mols", type=int, default=4, help = "number of molecules")
parser.add_argument("-N","--nodes", type=int, default=2, help = "number of nodes")
#parser.add_argument("-E","--edges", type=int, default=3, help = "number of edges")
args = parser.parse_args()
M = args.mols
N = args.nodes

#
## PAIR MATRIX p(k,k'): Matrix has a non entry at k,k1 position.
def p(k,k1):
    s = "p_k" + str(k) + "_k" + str(k1)
    return Bool( s )

# \/_{x<M/2,y>=M/2} !p(x,y) and !p(x,y)
M1 = M/2
C4 = True
for x in range(M):
    for y in range(M):
        if (x < M1 and x <= y) or (x>M1 and x>=y):
            C4 = And( Not(p(x,y)),C4 )


C5 = False
for x in range(M):
    for y in range(M):
		C5 = Or((p(x,y)),C5)
# Create Solver and add constraints in it.
s = Solver()
s.add(C4,C5)

if s.check() == sat:
    m = s.model()    
    r = [ [ m[p(i,j)] for j in range(M) ]
          for i in range(M) ]
    print_matrix(r)

    print ("yes")
else:
    print "failed to solve"
 
