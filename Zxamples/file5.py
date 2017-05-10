from z3 import *
import argparse

parser = argparse.ArgumentParser(description='Auto testing for TARA')
parser.add_argument("-M","--mols", type=int, default=2, help = "number of molecules")
parser.add_argument("-N","--nodes", type=int, default=2, help = "number of nodes")
args = parser.parse_args()
M = args.mols
N = args.nodes


dump = [ [Bool ("d_{}_{}".format(i,j)) for j in range(N)] for i in range(N)]

node = [ [Bool ("n_{}_{}".format(i,k)) for k in range(M)] for i in range(N)]

active_node = [ [Bool ("a_{}_{}".format(i,k)) for k in range(M)] for i in range(N)]

edge = [ [Bool ("e_{}_{}".format(i,j)) for j in range(N)] for i in range(N)]

presence_edge = [ [ [Bool ("e_{}_{}_{}".format(i,j,k)) for k in range(M)] for j in range(N)] for i in range (N)]

active_edge = [ [ [Bool ("b_{}_{}_{}".format(i,j,k)) for k in range(M)] for j in range(N)] for i in range (N)]

r = [ [ [Bool ("r_{}_{}_{}".format(i,j,k)) for k in range(M)] for j in range(N)] for i in range (N)]

p = [ [Bool ("p_{}_{}".format(k,k1)) for k1 in range(M)] for k1 in range(M)]


print node 
print active_node
print M
print N
C0 = [Implies (active_node[i][k], node[i][k]) for i in range(N) for k in range(M)] 

C1 = [Implies(presence_edge[i][j][k],node[i][k]) for i in range(N) for j in range(N) for k in range(M)]
      
C4 = [ Not(p[x][y])  for x in range(M) for y in range(M) if (x < (M/2) and x <= y) or (x > (M/2) and x>=y) ] 

vtr = C0 + C1 + C4

s = Solver()
s.add(vtr)
if s.check() == sat:
    m = s.model()
    p = [ [ m.evaluate(C4[i][j]) for j in range(M) ] for i in range(M) ]
    print_matrix(p)
else:
    print "failed to solve"

