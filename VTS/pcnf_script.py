#!/usr/bin/python

from z3 import *

x, y, z = Bools('x y z')
f =  Function('f', BoolSort(), BoolSort(), BoolSort())

fml = ForAll( x, Implies( Exists ( y, And( x, y) ), f(x,x) == x) )
#fml = Or (Not( And(x,y)), And(y,z))

#g = Goal()
#g.add(fml)

t1 = Tactic('simplify')
t2 = Tactic('nnf')
t3 = Tactic('tseitin-cnf')

#tr = Then(t1,t2)
#trx = Then(t1,t3)

# STEP 1: ~SIMPLIFY ND NNF
tr = Then(Tactic('simplify'),Tactic('nnf'))(fml).as_expr()
#r = tr(fml)
print tr
#exit(0)

# PULL QUANTIFIER OUT....

# STEP 2: SIMPLIFY ND CNF
trx = Then(Tactic('simplify'),Tactic('tseitin-cnf-core'))(tr).as_expr()
#trx = Then(t1,t3)
#rx = trx(tr)
print(trx)
#t = Then (Then(t1,t2), t3)

# ADD ADDITIONAL QUANTIFIERS FOR AUX VARIABLES......
