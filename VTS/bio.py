#!/usr/bin/python

from z3 import *
print z3.__file__
import argparse

import time

#----------------------------------------------------
# input parsing
# input number of nodes and molecules
parser = argparse.ArgumentParser(description='Auto testing for TARA')
parser.add_argument("-M","--mols", type=int, default=4, help = "number of molecules")
parser.add_argument("-N","--nodes", type=int, default=2, help = "number of nodes")
parser.add_argument("-Q","--pedges", type=int, default=2, help = "max no.of parallel edges btw two nodes")
parser.add_argument("-V","--variation", type=int, default=1, help = "model of the biological system")
parser.add_argument("-C","--connected", type=int, default=3, help = "graph connectivity want to check")
#parser.add_argument("-E","--edges", type=int, default=3, help = "graph connectivity want to check")



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
edge = [ [ [ Bool ("e_{}_{}_{}".format(i,j,q)) for q in range(Q)] for j in range(N)] for i in range(N)]

setE = [ edge[i][j][q] for i in range(N) for j in range(N) for q in range(Q) if i != j]

dump1 = [ [ [Bool ("d1_{}_{}_{}".format(i,j,q)) for q in range(Q)] for j in range(N)] for i in range(N)]
dump2 = [ [ [Bool ("d2_{}_{}_{}".format(i,j,q)) for q in range(Q)] for j in range(N)] for i in range(N)]

setDump1 =  [ dump1[i][j][q] for i in range(N) for j in range(N) for q in range(Q) if i != j]
setDump2 =  [ dump2[i][j][q] for i in range(N) for j in range(N) for q in range(Q) if i != j]

r1 = [ [Bool ("r1_{}_{}".format(i,j)) for j in range(N)] for i in range(N)]
r2 = [ [Bool ("r2_{}_{}".format(i,j)) for j in range(N)] for i in range(N)]

setR1 = [ r1[i][j] for i in range(N) for j in range(N) if i != j]
setR2 = [ r2[i][j] for i in range(N) for j in range(N) if i != j]

node = [ [Bool ("n_{}_{}".format(i,k)) for k in range(M)] for i in range(N)]
setN = [ node[i][k] for i in range(N) for k in range(M)]

active_node = [ [Bool ("a_{}_{}".format(i,k)) for k in range(M)] for i in range(N)]

presence_edge = [ [ [ [Bool ("e_{}_{}_{}_{}".format(i,j,q,k)) for k in range(M)] for q in range(Q)] for j in range(N)] for i in range (N)]
setPresentE = [ presence_edge[i][j][q][k] for i in range(N) for j in range(N) for k in range(M) for k in range(Q) if i != j] 

active_edge = [ [ [ [Bool ("b_{}_{}_{}_{}".format(i,j,q,k)) for k in range(M)] for q in range(Q)] for j in range(N)] for i in range (N)]

r = [ [ [ [Bool ("r_{}_{}_{}_{}".format(i,j,k,z)) for z in range(N-1)] for k in range(M)] for j in range(N)] for i in range (N)]

p = [ [Bool ("p_{}_{}".format(k,k1)) for k1 in range(M)] for k in range(M)]

sorts = [BoolSort() for m in range( M )]

f_n = [Function ("an_{}".format(m), *sorts) for m in range(M)] 

f_e = [Function ("ae_{}".format(m), *sorts) for m in range(M)] 

st = time.time()
 

# REGULATION MECHANISM VARIATION #####


# Regulation : No regulation on the node.
# The present molecules at nodes are all active.
def f_nn():
    A_list = []
    for i in range(N):
        for k in range(M):
            lhs = node[i][k] == active_node[i][k]
            A_list.append(lhs)
    return And(A_list)

# Regulation : No regulation on the edge.
# The present molecules on the edge are all active.
def f_ne():
    A_list = []
    for i in range(N):
        for j in range(N):
            if i == j:
                continue
            for q in range(Q):
                for k in range(M):
                    lhs = presence_edge[i][j][q][k] == active_edge[i][j][q][k]
                    A_list.append(lhs)
    return And(A_list)

# Regulation : Boolean on Node. 
# Activity of a moolecule k on a node is a Boolean function of presence of 
# Other molecule present on that node/edge.
# A0. active_node[k] =  f_n[k](\/_{k1 != k} node(k1)) 
def f_bn():
   # print "I am here!!"
    s = []
    A_list = []
    for k in range(M):
        f = f_n[k]
        for i in range(N):
            del s[:]
          #  for t in QVars:
          #      s.append( t )
            for k1 in range(M):
                if k1 == k:
                    continue
                s.append( node[i][k1])
            f_app = f(s)
            l = Implies( node[i][k], active_node[i][k] == f_app )
            A_list.append(l)
    return And( A_list )


# Regulation : Boolean on Edge. 
# Activity of the molecules on the edge is driven by the 
# chosen boolean function. 
# A1. active_edge[k] = f_e[k](\/_{k1 != k} presence_edge(k1))
def f_be():
    s = []
    A_list = []
    for i in range(N):
        for j in range(N):
            if i == j:
                continue
            for q in range(Q):
                for k in range(M):
                    del s[:]
                    f = f_e[k]
                   # for t in QVars:
                   #     s.append( t )
                    for k1 in range(M):
                        if k1 == k:
                            continue
                        s.append(presence_edge[i][j][q][k1])
                    l  =  Implies(presence_edge[i][j][q][k], active_edge[i][j][q][k] == f(s)) 
                    A_list.append(l)
    return And( A_list )

# Regulation : SNARE-SNARE Inhibition. 
# Inhibition of the edges are driven by the pairing matrix.
# If there is a molecule k present on the edge(i,j) and with non-zero pairing matrix
# the evry pairing matrix elements are present on that node makes that molecule inactive.
# else its active.
# { e(i,j,k) and (exists_k': p(k,k') and [p(k,k') -> e(i,j,k')] )) } -> ~ b(i,j,k)   
# { e(i,j,k) and ~(exists_k': p(k,k') and [p(k,k') -> e(i,j,k')] )) } -> b(i,j,k)   

def f_se():
    A_list = []
    for i in range(N):
        for j in range(N):
            if i == j:
                continue
            for q in range(Q):
                for k in range(M):
                    lhs = presence_edge[i][j][q][k]
                    l1 = False
                    l2 = True
                    for k1 in range(M):
                        if k1 == k:
                            continue
                        l1 = Or( p[k][k1], l1)
                        l2 = And( Implies( p[k][k1], presence_edge[i][j][q][k1]) , l2)   
                    l = Implies ( And( lhs, And( l1,l2)), Not( active_edge[i][j][q][k]) )
                    ll = Implies ( And ( lhs, Not (And ( l1,l2)) ), active_edge[i][j][q][k]) 
                    A_list.append(And ( l, ll) )
    return And (A_list)

# Setting Activity Bits --------------------------

A0 = True 
A1 = True

if V == 1:
    A0 = f_bn() 
    A1 = f_be()
    #print A0
    #print A1
    #exit(0)
elif V == 2:
    A0 = f_nn()
    A1 = f_be()
    #print A0
    #print A1
elif V == 3:
    A0 = f_bn()
    A1 = f_se()
    #print A1
    #exit(0)
elif V == 4:
    A0 = f_nn()
    A1 = f_se()
elif V == 5:
    A0 = f_bn()
    A1 = f_ne()
else:
    A0 = f_nn()
    A1 = f_ne()

#xx = time.time() - st
#print "Building A's took...", str(xx)
#st = time.time()
# print A0
# print A1


# Add constraints one by one ------------

# SET OF BASIC CONSTRAINTS V1-V6 ####

# Constraint V1 ----------------------
# V1 : For an edge to exist it should have one molecule present.
# Basic intuition: \/_k e_ijk -> e_ij  
# Implementation: /\_ijq ( (\/_m e_ijqm) -> e_ijq) 
# DANGER :: Involve equality and we'll might it while doing validity check.
V1 = True
for i in range(N):
    for j in range(N):
        if j == i:
            continue
        for q in range(Q):
            rhs = False
            for k in range(M):
                rhs = Or( presence_edge[i][j][q][k], rhs )
            V1 = And (Implies (rhs, edge[i][j][q]), V1)
#print V1

# Constraint V2 ----------------------
# V2 : If molecule is active on an edge then it should be present on the edge.
# Basic intuition :  b_ijk -> e_ijk   
# Implementation : /\_ijqm b_ijqm -> e_ijqm 
V2 = True
F1_list = []
for i in range(N):
    for j in range(N):
        if j == i:
            continue
        for q in range(Q):
            for k in range(M):
                F1_list.append( Implies(active_edge[i][j][q][k], presence_edge[i][j][q][k]) )
V2 = And( F1_list )
#print V2

# Constraint V3 ----------------------
# V3 : A mmolecule should be present to be active.
# Implementation: /\_i,m (a_im -> n_im) 
V3 = True 
A1_list = []
for i in range(N):
    for k in range(M):
        l = Implies ( active_node[i][k], node[i][k])
        A1_list.append(l) 
V3 = And (A1_list)
#print V3

# Constraint V4 -------------------------
# V4 : The edge label are subset of the source and target.
# Basic Intution: e_ijk -> a_ik and a_jk
# Implementation: /\_i,j,q,m (e_ijqm -> (n_im and n_jm))
V4 = True
for i in range(N):
    for j in range(N):
        if i == j :
            continue
        for q in range(Q):
            for k in range(M):
                V4 = And (Implies( presence_edge[i][j][q][k], And( node[i][k], node[j][k]) ), V4)

# Constraint V5 -------------------------
# V5: Self edges not allowed. 
# Basic Intuition: ~ e_ii  
# Implemenatation: /\_i,q (not e_i,i,q) 
V5 = True
for i in range(N):
    for q in range(Q):
        V5 = And( Not(edge[i][i][q]), V5)


# Constraint V6 ------------------------
# V6: Only Q R entry has possible non zero entry.  
# Basic intuition: \/_{x<M/2,y>=M/2} !p(x,y) and !p(y,x)
# Implementation: /\_( (x < M/2 and y < M/2) or (x >= M/2 and y >= M/2))
V6 = True
for x in range(M):
    for y in range(M):
        if ( ((x < M/2) and (y < M/2)) or ((x>=M/2) and (y >=M/2)) ):
            V6 = And (Not(p[x][y]), V6)


# WELL FUSED CONSTRAINTS ###### 

# Constraint V7 ----------------------
# Each edge must fuse with its destination node.
# Fuse : There should be an active pair corresponding to pairing matrix 
#        on the edge and target node.
# Implementation : /\_ijq e_ijq -> \/_m,m' (b_ijqm and a_jm' and p_mm') 
V7 = True
A_list = []
for i in range(N):
    for j in range(N):
	if i == j:
	    continue
        for q in range(Q):
	    lhs = False    
            for k in range(M):
	        for k1 in range(M): 
                    if k == k1:
                        continue
	            lhs = Or ( And (active_edge[i][j][q][k], And (active_node[j][k1],p[k][k1])), lhs)  
            w =  Implies (edge[i][j][q], lhs)
            A_list.append(w)
V7 = And(A_list)
#print V7


# Constraint V8 ----------------------
# V8 : Edge should not be able to potentially fuse with any node other than
#      than its target node.
# Implementation: /\_ijqk b_ijqk -> not (/\_(j' != k", m'') a_j'm" and p_mm" )
V8 = True
A_list = []
for i in range(N):
    for j in range(N):
	if i == j:
	    continue
        for q in range(Q):
            for k in range(M): 
                lhs = False
	        for j1 in range(N):
                    if j == j1:
                        continue
	            for k11 in range(M):
                        if k == k11:
                            continue
		        lhs = Or (And ( active_node[j1][k11], p[k][k11]), lhs)
	        w = Implies( active_edge[i][j][q][k], Not(lhs))
                A_list.append(w)
V8 = And(A_list)

# STABILITY CONDITION #########

# Constraint R1 ----------------------
# R1 : Encode Reachability 
# " nodes i,j is reachable with kth molecule in z steps if 
#   there is an edge between i'' and j (i != i'') with k 
#   present on that edge  and i'' is reachable from i in z steps.
# Implemetation : /\_ijmp r_ijmp -> (\/_q e_ijqm or \/_i!=i' (\/_q e_ii'qm and r_i'jmp-1) )
R1 = True
A_list = []
for i in range(N):
    for j in range(N):
	if i == j:
	    continue
        for k in range(M):
            for z in range(N-1):
                pe = False
                if z == 0: 
                    for q1 in range(Q):
                        pe = Or( presence_edge[i][j][q1][k], pe)
                    A_list.append( Implies(r[i][j][k][0], pe) ) 
                else:
                    lhs = False
                    for l in range(N):
                        if i == l or j == l:
                            continue
                        for q1 in range(Q):
                            pe = Or (presence_edge[i][l][q1][k], pe)
                        lhs = Or (And ( r[l][j][k][z-1],pe), lhs)   
                    w  = Implies (r[i][j][k][z], lhs)
                    A_list.append(w)
R1 = And(A_list)
#print R1


# Constraint R2 ----------------------
# Encode stability using the reachability variables. 
# If there is an m-edge between i and j, there is m reachble path from j and i.
# Implementation: /\_i,j,m (\/_q e_ijqm) -> r_ijqU
A_list = []
for i in range(N):
    for j in range(N):
	if i == j:
	    continue
        for k in range(M):
            lhs = False
            rhs = False
            for q in range(Q):
                rhs = Or (presence_edge[i][j][q][k], rhs)
            for z in range(N-1):
                lhs = Or (r[j][i][k][z], lhs)
            l  = Implies (rhs, lhs)
            A_list.append(l)
R2 = And(A_list)
#print R2


#fss = time.time() - st
#print "Building F0-F3's took...", str(fss)
#st = time.time()


# CONNNECTIVITY CONSTRAINTS ####### 

# Constraint D1 ----------------------
# D1: Only present edges can be dropped.
# Implementation: d1_ijq -> e_ijq 
D1_list = []
for i in range(N):
    for j in range(N):
        if i == j:
            continue
        for q in range(Q):
            D1_list.append( Implies (dump1[i][j][q], edge[i][j][q]) )
D1_edge_exists = And (D1_list)

#--------------------------
# D11 : For second set of variables.
D1_list = []
for i in range(N):
    for j in range(N):
        if i == j:
            continue
        for q in range(Q):
            D1_list.append( Implies (dump2[i][j][q], edge[i][j][q]) )
D11_edge_exists = And( D1_list )


# Constraint D2 ----------------------

# 1. Flattening the array. Avoid i == j. 
     # For the First D 
     # For the 3 connected.
d1 = []
for i in range(N):
    for j in range(N):
        if i == j:
            continue
        for q in range(Q):
            d1.append (dump1[i][j][q])
L = len(d1)

# 2. K drops from the graph.
oneList = []
for i in range(L):
    oneList.append(1)

z = zip (d1, oneList)

D2_drops_are_k_minus_1 = PbEq (z, C-1)

#print D2
 
#-------------------------------
# D22 : For second set of variables.
# For the 4 connected.
# Flattening the array. Avoid i == j. 
# For the Second D 
d2 = []
for i in range(N):
    for j in range(N):
        if i == j:
            continue
        for q in range(Q):
            d2.append (dump2[i][j][q])
L = len(d2)

oneList = []
for i in range(L):
    oneList.append(1)
print oneList

z = zip (d2, oneList)

D22_drops_are_k = PbEq (z, C)

#print D22

# Constraint D3 ----------------------
# D3 : New reachability for the connectivity check.
# Implementation :  /\_ij [ (\/_q eijq and not d_ijq) or (\/_i!=i' r'_i'j and \/_q (e_ii'q and not d_ii'q) ) ] -> \/_ij r'_ij 
#D3 = True
A_list = []
for i in range(N):
    for j in range(N):
        if i == j:
            continue
        bhs = False
        for q in range(Q):
            bhs = Or( And (edge[i][j][q], Not(dump1[i][j][q]) ), bhs) 
        rhs = False
        for l in range(N):
            if i == l or j == l:
                continue
            for q in range(Q):
                rhs = Or( And( edge[i][l][q], Not (dump1[i][l][q]) ), rhs)
            rhs = And( rhs, r1[l][j])
        # w = Implies( r1[i][j], Or (bhs , rhs) )
        w = Implies( Or(bhs , rhs), r1[i][j] )
        A_list.append(w)
D3_1_reachability = And(A_list)


#------------------------------------------
# Reachability for the second set of variables.
A_list = []
for i in range(N):
    for j in range(N):
        if i == j:
            continue
        bhs = False
        for q in range(Q):
            bhs = Or( And( edge[i][j][q], Not(dump2[i][j][q]) ), bhs) 
        rhs = False
        for l in range(N):
            if i == l or j == l:
                continue
            for q in range(Q):
                rhs = Or( And( edge[i][l][q], Not(dump2[i][l][q])), rhs)
            rhs = And (rhs, r2[l][j])
        # w = Implies( r2[i][j], Or (bhs , rhs) )
        w = Implies (Or (bhs , rhs), r2[i][j] )
        A_list.append(w)
D33_2_reachability = And(A_list)
#print D33


# Constraint D4 ----------------------
# D4 : Graph remains connected after k-1 drops.
# There are unreachable pairs of nodes in the underlying undirected graph.
D4 = True
D4_list = []
for i in range(N):
    for j in range(N):
        if i == j:
            continue
        rijji = Or (r1[i][j], r1[j][i])
        D4_list.append( rijji )
D4_1_all_connected = And( D4_list )

#D4 = Not(D4)
#print D4

# ---------------------
# D44 : For second set of variables.
# D4 : Graph becomes disconnected after k drops.
D44 = True
D44_list = []
for i in range(N):
    for j in range(N):
        if i == j:
            continue
        rijji = Or (r2[i][j], r2[j][i])
        D44_list.append( rijji )
D44 = And( D44_list )
D44_2_some_disconnected = Not(D44)



# first one - for all drops ( D1_edge_exists /\ D2_drops_are_k_minus_1  /\ exists reachVars (D3_1_reachability) -> D4_1_all_connected ) 
is_reach = Exists( setR1, And(D3_1_reachability, D4_1_all_connected) )

k_min_1_connected = ForAll( setDump1,
                            Implies( And( D1_edge_exists, D2_drops_are_k_minus_1), is_reach ))

is_reach = Exists( setR2, And(D33_2_reachability, D44_2_some_disconnected) )

k_not_connected = Exists( setDump2, And( D11_edge_exists, D22_drops_are_k, is_reach ) )


connectivity = And( k_min_1_connected, k_not_connected )
k_not_connected = Exists( setDump2, And( D11_edge_exists, D22_drops_are_k, is_reach ) )

connectivity = And( k_min_1_connected, k_not_connected )

#dx = time.time() - st
#print "D0-D3 Building took", str(dx)

# Fix some edges in the Graph.
#Init = (edge[1][0][0] == True)
#Xf = (dump1[1][0][0] == True)
#Init2 = (p[0][3] == False)
#Init3 = (p[1][2] == False)
# Create Solver and add constraints in it.

s = Solver()

s.add( Exists( setE, And( ForAll( setDump1, Implies( And( D1_edge_exists, D2_drops_are_k_minus_1), Exists( setR1, And( D3_1_reachability, D4_1_all_connected)) ) ), ForAll (setDump2, Implies( And (D11_edge_exists, D22_drops_are_k ), Exists( setR2, And( D33_2_reachability, D44_2_some_disconnected) ))) ) ))
    
#s.add( Exists( setE, connectivity) )

# Updated check for qbf formula. Suff condition.
#rst = And (D1, D3)
#rest = And (D1, D11, D3, D33)
#kconn =  ForAll (setDump1, Implies (D2, Exists (setR1, D4)) )  
#print kconn 
#exit(0)
#notk1conn = ForAll (setDump2, Implies (D22, Exists (setR2, D44)) )  
#wwe = And (kconn, notk1conn, rest)

# Sufficient condition check
#s.add(wwe)
#xxx = Exists (setE, wwe) 
#s.add (Exists (setE, wwe) )

#xxx =  (Exists (setE, ForAll (setDump1, And (Implies (D2, Exists (setR1, D4)) , D1, D3) ) ) )  

#xxx = Exists (setE, And (ForAll (setDump1, And (Implies (D2, Exists (setR1, D4))) ) ,  ForAll (setDump2, And (Implies (D22, Exists (setR2, D44))) ), D1, D11, D3, D33) )  
#print xxx
#exit(0)

# This cause Core dump.
#s.add (Exists (setE, ForAll (setDump1, And (Implies (D2, Exists (setR1, D4)) , D1, D3) ) ) ) 

# BUT This Does't not ----
#s.add( Exists (setE, And (ForAll (setDump1, And (Implies (D2, Exists (setR1, D4))) ),  ForAll (setDump2, And (Implies (D22, Exists (setR2, D44))) ), D1, D11, D3, D33) )  )

# s.add( Exists( setE, ForAll (setDump1, (ForAll (setDump2, Exists (setR1, Exists (setR2 , And ( Implies (D2, D4), Implies (D22, D44),  D1, D11, D3, D33) )  )) )  ) ))

# NOISE WITH EDGES:
# s.add( Exists ( setE, ForAll(setN, ForAll( setPresentE, ForAll (setDump1, (ForAll (setDump2, Exists (setR1, Exists (setR2 , And ( Implies (D2, D4), Implies (D22, D44),  D1, D11, D3, D33, V1, V4, V5) )  )) )  ) ) ) ) )

# Neccessary condition Check.
#s.add (A0, A1, V1, V2, V3, V4, V5, V6, V7, V8, R1, R2, D1, D2, D3, D4)

print "solving...\n"
#st = time.time()
print s.check()

#b = time.time() - st 
#print "Solving took...", str(b + fbx)

# Printing the Graph ##########
def dump_dot( filename, m ) :
    print "dumping dot file: ", filename
    dfile = open(filename, 'w')
    dfile.write("digraph prog {\n")
    for i in range(N):
        node_vec = str(i)+":"
        for k in range(M):
            if is_true(m[node[i][k]]) :
                node_vec = node_vec + "1"
            elif is_false(m[node[i][k]]):
                node_vec = node_vec + "0"
            if is_true(m[active_node[i][k]]) :
                node_vec = node_vec + "-"
        dfile.write( str(i) + "[label=\"" + node_vec + "\"]\n")
        for j in range(N):
            if i == j:
                continue
            for q in range(Q):
                for k in range(M):                    
                    if q == 0:
                        style = "solid"
                       # if is_true( m[dump1[i][j][q]] ):
                       #     style="dashed"
                        if is_true(m[presence_edge[i][j][q][k]]):
                            label = str(k)
                            color = "black"
                            if is_true(m[active_edge[i][j][q][k]]):
                                color = "green"
                                for k1 in range(M):
                                    if is_true(m[active_node[j][k1]]) and is_true(m[p[k][k1]]):
                                        color = "red"
                                        break
                            dfile.write( str(i) + "-> " + str(j) + "[label=" + label +",color=" + color + ",style=" + style + "]" +"\n" )
                    
                    if q == 1:
                        style = "solid"
                     #   if is_true( m[dump1[i][j][q]] ):
                     #       style="dashed"
                        if is_true(m[presence_edge[i][j][q][k]]):
                            label = str(k)
                            color = "yellow"
                            if is_true(m[active_edge[i][j][q][k]]):
                                color = "pink"
                                for k1 in range(M):
                                    if is_true(m[active_node[j][k1]]) and is_true(m[p[k][k1]]):
                                            color = "blue"
                                            break
                            dfile.write( str(i) + "-> " + str(j) + "[label=" + label +",color=" + color + ",style=" + style + "]" +"\n" )

    dfile.write("}\n")

if s.check() == sat:
    m = s.model()
    #print m
    #print "Printing the model..."
    for d in m.decls():
        print "{} = {}".format(d.name(), m[d])
#    print 
    xc = [ [ m[p[i][j]] for j in range(M) ] for i in range(M) ]
    s = [[ [ [m[active_edge[i][j][q][k]] for k in range (M)] for q in range(Q)] for j in range(N) ] for i in range(N) ]
    t = [ [ [m[edge[i][j][q]]  for q in range(Q)] for j in range(N) ] for i in range(N) ]
    print 'Printing the pairing matrix'
    print_matrix(xc)
    print '\n printing the dropped edges: \n '
    d = [ [ [m [dump1[i][j][k]] for k in range(Q)] for j in range(N)] for i in range(N)]
    print d
   # print ' '
    #print t
    dump_dot( "/tmp/bio.dot", m )
else:
    print "failed to solve"
    
    
# CADET TRANSFORMATION #####

# Stage 1: Convert The Formula Into CNF
#g = Goal()
#g.add (V1, V2, V3, V4, V5, V6, V7, V8, R1, R2, D1, D2, D3, D4) 
##print g
## t is a tactic that reduces a Boolean problem into propositional CNF
#t = Then('simplify', 'tseitin-cnf')
#subgoal = t(g)
#assert len(subgoal) == 1


## Traverse each clause of the first subgoal
#for c in subgoal[0]:
#        #print "children: ", c.children()
#        #print "1st child:", c.arg(0)
#        #print "operator: ", c.decl()
#        #print "op name:  ", c.decl().name()
#        print c

# Stage 2 : Convert CNF to QDIMAC
# Accessing the structure of a Z3 expression via the API
# Check goal.cpp goal::display_dimacs

# Use PicoSat and Plingeling (fmv.jku.at/lingeling) 
