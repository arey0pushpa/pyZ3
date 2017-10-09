#!/usr/bin/python

from z3 import *

x, y = Reals('x y')
g = Goal()
#g.add(x > 0, y > 0, x == y + 2)
print g
g.add(Or(x < 0, x > 0), x == y + 1, y < 0)
#t1 = Tactic('simplify')
#t2 = Tactic('solve-eqs')
#t = Then(t1, t2)

t = Tactic('split-clause')
r = t(g)
for g in r:
    print g

#print t(g)
describe_tactics()


