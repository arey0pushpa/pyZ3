import sys
import itertools
from z3 import *

# Create solver
s = Solver()

# Create Z3 integer variables for matrix cells
#cells = [ [ Int("z_%s_%s" % (i+1, j+1)) for j in range(9) ] for i in range(9) ]
#pp(cells)

cells = [ [Int("z_{}_{}".format(i+1,j+1)) for j in range(9)] for i in range(9) ]

# Add sudoku problem instance constraints
# We'll work on it :)

# Add cell constraints
for y in range(9):
    for x in range(9):
        s.add(And(1 <= cells[x][y], cells[x][y] <=9))

# Add column constrints
for x in range(9):
    s.add(Distinct(cells[x]))

# Add row constrints
for y in range(9):
    s.add(Distinct([cells[x][y] for x in range(9)]))


# add group constraints
for y in range(0,9,3):
    for x in range(0,9,3):
        s.add(Distinct([cells[x+i][y+j] for i,j in itertools.product(range(3),range(3))]))


if s.check() == sat:
    m = s.model()
    for y in range(9):
        print "".join([str(m.evaluate(cells[x][y])) for x in range(9)])
else:
    print "failed to solve"


#pp(cells)
#
#
## One dimentional matrix 
## If you do i + 1 it complains and if u do (i+1) it stays fine.
#grid = [Int('x_%s' % (i+1)) for i in range(9)]
#
#grid1 = [Int('x_{}'.format(i + 1)) for i in range(9)]
#print grid1
#pp (grid1)
#
#
## Two dimentional matrix
#
#X = [ [ Int("x_%s_%s" % (i+1, j+1)) for j in range(3) ]  for i in range(3) ]
#print X
#
#grid2 = [ [Int("x_{}_{}".format(i,j)) for j in range(3)] for i in range(3)]
#pp (grid2)
#
## Three dimentional:
#grid3 = [ [ [Int("x_{}_{}_{}".format(i,j,k)) for k in range(3)] for j in range(3)] for i in range(3)]
#
##print grid3
#
#pp(grid3)
#
#
