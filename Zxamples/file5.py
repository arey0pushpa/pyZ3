from z3 import *
import argparse

parser = argparse.ArgumentParser(description='Auto testing for TARA')
parser.add_argument("-M","--mols", type=int, default=4, help = "number of molecules")
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

p = [ [Int ("p_{}_{}".format(k,k1)) for k1 in range(M)] for k1 in range(M)]

         
C0 = [Implies (active_node[i][k], node[i][k]) for i in range(N) for k in range(M)] 

C1 = [Implies(presence_edge[i][j][k],node[i][k]) for i in range(N) for j in range(N) for k in range(M)]

#C4 = [ p[x][y]== 1 if x == 0 else p[x][y] == 10 for x in range(M) for y in range(M)]


C4 = [ If(p[i][j] == 1, 
                  True, 
                  p[i][j] == False) 
               for i in range(M) for j in range(M) ]


#C5 = [ p[x][y]  if x == y else Not(p[x][y]) for x in range(M) for y in range(M)] 
#C6 = [ And(p[x][y])  for x in range(M) for y in range(M)] 
#C7 = [ Not(p[x][y])  for x in range(M) for y in range(M)] 

vtr =  C4 

s = Solver()
s.add(vtr)

if s.check() == sat:
    m = s.model()
    r = [ [ m[p[i][j]] for j in range(M) ] 
		for i in range(M) ]
    #p = [  1 if is_true(m[C4[i][j]]) else 0 for j in range(M) for i in range(M)]
    print_matrix(r)
else:
    print "failed to solve"

