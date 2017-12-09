#!/usr/bin/python

from z3 import *

def qpull(e, seen):
    if is_quantifier(e):
        if e.is_forall(): 
            var_list = []
            for i in range(e.num_vars()):
                c = Const(e.var_name(i) + "", e.var_sort(i))
                #var_list.append( e.var_name(i))
                quant_set.append(('A', c))
                print quant_set
                #var_list.append(e.var_name(i))
           #print var_list
            #print 'the cyclone is coming'
            #print e.body() 
            #exit(0)
            return qpull(e.body(), seen)
        
        else:
            var_list = []
            for i in range(e.num_vars()):
                c = Const(e.var_name(i) + "", e.var_sort(i))
                #var_list.append( e.var_name(i))
                quant_set.append(('E', c))
                #quant_set.append(E,[])
                #var_list.append(e.var_name(i))
            #print e.body()
            return qpull(e.body(), seen)
    
    # The expression can be an Boolean.
    if is_bool(e):
# Handle And: Return And (rest formula)
        if (is_and(e)):
            #print('I was here')
            n = e.num_args()
            arg_list = []
            for i in range(n):
                #print (e.arg(i))
                arg_list.append( qpull(e.arg(i), seen))
            #print arg_list
            return And(arg_list)
        
# Handle And: Return And (Neg(inl), Neg(inr))
        if (is_or(e)):
            n = e.num_args()
            arg_list = []
            for i in range(n):
                #print (e.arg(i))
                arg_list.append( qpull(e.arg(i), seen))
            #print arg_list
            return Or(arg_list)
       
# Handle NOT: return Not(Const)
        if (is_not(e)):
            for i in e.children():
                return Not(i)
# Handle variables
        if (is_var(e)):
            return e
        
# Handle the constant case        
        if(is_const(e)):
            return e
# Handle the variable case.  
# if(is_const(e)):
# return e

quant_set = [] 
x, y, z = Bools('x y z')
f =  Function('f', BoolSort(), BoolSort(), BoolSort())

#fml = ForAll( x, Implies( Exists ( y, And( x, y) ), f(x,x) == x) )
fml = ForAll (x, ForAll (y, ForAll (z, Or (And(x,y), And(y,z)))))

seen = {}
#g = Goal()
#g.add(fml)

t1 = Tactic('simplify')
t2 = Tactic('nnf')
t3 = Tactic('tseitin-cnf')

#tr = Then(t1,t2)
#trx = Then(t1,t3)

# STEP 1: ~SIMPLIFY ND NNF -- IMPLIES ELIM AND NNF PROP
tr = Then(Tactic('simplify'),Tactic('nnf'))(fml).as_expr()
#r = tr(fml)
print tr
#exit(0)

# CHECK THE QUNATIFIERS THAT THE SUBEXPRESSIONS HAVE AND PULL THE VARIABLE TO THE TREE ONE LEVEL A
#  REQUIRE RENAMING THE VARIABLE ...

# PULL QUA1NTIFIER OUT
nnf_fml = qpull(tr, seen)

print 'The resultant formula after bs is : \t'
print nnf_fml

# STEP 2: SIMPLIFY ND CNF
trx = Then(Tactic('simplify'),Tactic('tseitin-cnf'))(nnf_fml).as_expr()
#trx = Then(t1,t3)
#rx = trx(tr)
print(trx)
print quant_set
#exit(0)
#t = Then (Then(t1,t2), t3)
#def create_formula(quant_set):
formula = ''
for i in quant_set:
    if i[0] == 'A':
        formula = formula + 'ForAll( ' +  str(i[1]) + ', '
    else:
        formula = formula + 'Exists( ' + str(i[1]) + ', '
    #for i in quant_set:
    #    if i[0] == 'A':
    #        ForAll( i[1], + 

print formula
build_formula = formula + str(trx) 
print build_formula
        
# ADD ADDITIONAL QUANTIFIERS FOR AUX VARIABLES......

