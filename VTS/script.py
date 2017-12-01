#!/usr/bin/python

# Script  changes the input given qbf formula  into a skolemize function.
from z3 import *

def visitor(e):
    yield e
    if is_app(e):
        for ch in e.children():
            for e in visitor(ch):
                yield e
    if is_quantifier(e):
        for e in visitor(e.body()):
            yield e
        return

def reverse_enum(L):
   for index in reversed(xrange(len(L))):
      yield index, L[index]

x, y, z = Bools( 'x y z' )

# Step 1 : Convert the QBF formula into all quantifiers in front and propositional  at the end.

#fml = ForAll( x, Exists( y, And( Or( Not(x), y), Or( Not(y), x)))) 
fml = ForAll( x, Exists( y, And (And( Or( Not(x), y), Or( Not(y), x)), Exists(z, And (Or( Not(x), z), Or(y, Not(z))))) )) 

#fml = Or( And( Not(x), y), And( Not(y), x))
#fml = Implies( Implies (x, y), Or(y,z)) 
#seen = {}

#index = 0
qbf_list = []
for e in visitor(fml):
    qbf_list.append(e)
#    print 'subformula seen is:' + str (index + 1) + '\t' + str(e)

# Now we have list of every subformula of the given formula.
#print qbf_list
prop_for_list = []
quan_list = []
var_set = [] # Create a tuple the variable and it's type (sort).
for e in reversed(qbf_list):
    if(is_quantifier(fml)):
        # You might need a children() rather than body.
        prop_for_list.insert(e.body())
        #quan_list.insert(0,e)
        if(fml.is_forall()):
            quan_list.append('ForAll')
            num_var = fml.num_vars()
            for i in range(num_var):
                var_set.append(fml.var_name(i))
                # We need type also.
                # var_set_sort.append(fml.var_sort(i))
            
        # Similar for Exists ....

# Traverse list in reverse direction.
#for i,e in the reverse_enum(qbf_list):
#    if  is_quantifier(e):
#        print i 
#        print e + '\n'


exit(0)
# Step 2: Convert The Resultant Formula Into CNF
g = Goal()
g.add(fml)

## t is a tactic that reduces a Boolean problem into propositional CNF
t = Then('simplify', 'tseitin-cnf')
subgoal = t(g)
print subgoal
assert len(subgoal) == 1

## Try to solve the formula using qsat
#t = Then('simplify','qe','qsat')
#r = t(g)
## r contains only one subgoal
#print r
#
#s = Solver()
#s.add(r[0])
#print s.check()
## Model for the subgoal
#print s.model()
## Model for the original goal
#print r.convert_model(s.model())
#
#print '\nPrinting CNF formulas:'
### Traverse each clause of the first subgoal
#for c in subgoal[0]:
#    print "children: ", c.children()
#    print "1st child:", c.arg(0)
#    print "operator: ", c.decl()
#    print "op name:  ", c.decl().name()
#    print c




