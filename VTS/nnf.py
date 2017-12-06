#!/usr/bin/python
# Script  changes the input given qbf formula  into a skolemize function.

from __future__ import print_function
from z3 import *
import types

def is_implies(a):
    return is_app_of(a, Z3_OP_IMPLIES)

def nnf(e, seen, sign):
    #if e in seen:
        # Return the same result. ?
     #   return
    #seen[e] = True
    # The expression can be an application.
#    if is_app(e):
#        for ch in e.children():
#            for e in nnf(ch, seen, sign):
#                yield e
#        return 
#
    # The expression can be an qunatifier.
    if is_quantifier(e):
        if e.is_forall():
            var_list = []
            for i in range(e.num_vars()):
                var_list.append(e.var_name(i))
            if sign == False:
                return Exists(var_list, nnf(e.body(), seen, sign))
            else:
                #print (e.body())
                #exit(0)
                return ForAll(var_list, nnf(e.body(), seen, sign))
        
        else:
            var_list = []
            for i in range(e.num_vars()):
                var_list.append(e.var_name(i))
            if sign == False:
                return ForAll(var_list, nnf(e.body(), seen, sign))
            else:
                return Exists(var_list, nnf(e.body(), seen, sign))
    
    # The expression can be an Boolean.
    if is_bool(e):
        # Hnadle And: Return Or (Neg(inl), Neg(inr))
        if (is_and(e)):
            n = e.num_args()
            arg_list = []
            for i in range(n):
                #print (e.arg(i))
                arg_list.append(nnf(e.arg(i), seen, sign))
            if sign == False:
                return Or(arg_list)
            else:
                return And(arg_list)
#                yield e
#                for ch in e.children():
#                    for e in nnf(ch, seen, sign):
#                        yield e
        
        # Hnadle And: Return And (Neg(inl), Neg(inr))
        if (is_or(e)):
            n = e.num_args()
            arg_list = []
            for i in range(n):
                #print (e.arg(i))
                arg_list.append(nnf(e.arg(i), seen, sign))
            if sign == False:
                return And(arg_list)
            else:
                return Or(arg_list)
       
       # Handle NOT:change the sign of the rest of the formula.
        if (is_not(e)):
            #print(e.children())
            #exit(0)
            if sign == True:
                sign = False
            else: 
                sign = True
            #return 
            #x = e.children()
            #if(is_const(e.children())):
            #if(is_var(e.children())):
            for i in e.children():
                print(i) 
            return nnf(e.children(), seen, sign) 
            #if type(e.children()) is list:
            #    print ('Mandir wahi banayenge')
            #return nnf(e.children(), seen, sign)
            #print (wwe)
        
       # Handle the variable case.  
        if(is_const(e)):
           # print('I was here')
            if sign == True:
                return e
            else:
                return Not(e)

def impl_elim(e, seen):
    if e in seen:
        return
    seen[e] = True
    if is_quantifier(e):
        if e.is_forall():
            var_list = []
            for i in range(e.num_vars()):
                var_list.append(e.var_name(i))
                return ForAll(var_list, impl_elim(e.body(), seen))
        
        else:
            var_list = []
            for i in range(e.num_vars()):
                var_list.append(e.var_name(i))
                return Exists(var_list, impl_elim(e.body(), seen))
    
    # The expression can be an Boolean.
    if is_bool(e):
        # Hnadle And: Return Or (Neg(inl), Neg(inr))
        if (is_and(e)):
            print('I was here')
            n = e.num_args()
            arg_list = []
            for i in range(n):
                print (e.arg(i))
                arg_list.append( impl_elim(e.arg(i), seen))
                return And(arg_list)
        
        # Hnadle And: Return And (Neg(inl), Neg(inr))
        if (is_or(e)):
            n = e.num_args()
            arg_list = []
            for i in range(n):
                print (e.arg(i))
                arg_list.append( impl_elim(e.arg(i), seen))
                return Or(arg_list)
       
       # Handle NOT:change the sign of the rest of the formula.
        if (is_not(e)):
            return (Not( impl_elim( e.children(), seen) ))

       # Handle the implication.
        if(is_implies(e)):
            return ( Or( Not(impl_elim (e.arg(0), seen), impl_elim(e.arg(1),seen))))
       
       # Handle the variable case.  
        if(is_const(e)):
                return e

#def impl_eli(fml, seen):
#def reverse_enum(L):
#   for index in reversed(xrange(len(L))):
#      yield index, L[index]

x, y, z = Bools( 'x y z' )

sign = Bool('sign')
sign = True
#print(sign)
# Step 1 : Convert the QBF formula into all quantifiers in front and propositional  at the end.
#fml = Or(x, y)
fml = Not(x)
#fml = ForAll (x, ForAll (y, And(x,y) ) )
#fml = ForAll( x, Exists( y, And( Or( Not(x), y), Or( Not(y), x)))) 
#fml = ForAll( x, Exists( y, And (And( Or( Not(x), y), Or( Not(y), x)), Exists(z, And (Or( Not(x), z), Or(y, Not(z))))) )) 

#fml = Or( And( Not(x), y), And( Not(y), x))
#fml = Implies( Implies (x, y), Or(y,z)) 
seen = {}

#index = 0
#qbf_list = []
#impl_elim_fml = impl_elim (fml,seen)
cnf_formula = nnf(fml, seen, sign)
print(cnf_formula)

#for e in nnf(fml, seen, sign):
#    if is_const(e) and e.decl().kind == Z3_OP_UNINTERPRETED:
#        print("Variable", e)
#    else:
#        print(e)
#        print('AAoo_lolita\n')
#    qbf_list.append(e)
#    print 'subformula seen is:' + str (index + 1) + '\t' + str(e)

# Now we have list of every subformula of the given formula.
#print qbf_list
#prop_for_list = []
#quan_list = []
#var_set = [] # Create a tuple the variable and it's type (sort).
#for e in reversed(qbf_list):
#    if(is_quantifier(fml)):
        # You might need a children() rather than body.
#        prop_for_list.insert(e.body())
        #quan_list.insert(0,e)
#        if(fml.is_forall()):
#            quan_list.append('ForAll')
#            num_var = fml.num_vars()
#            for i in range(num_var):
#                var_set.append(fml.var_name(i))
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
p, q, r = Bools('p q r')
fml =  Or (And(p, q), Not( Or(q, r)))
g = Goal()
g.add(fml)

## t is a tactic that reduces a Boolean problem into propositional CNF
t = Then('simplify', 'tseitin-cnf')
subgoal = t(g)
print(subgoal)
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




