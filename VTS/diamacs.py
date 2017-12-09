#!/usr/bin/python

# THINGS TO DO :
# 1. IMPLICATION ELIMINATION AND EQUIVALENCE ELIMINATION.
# 2. NNF TRANSFORMATION.
# 3. PULL OUT QUANTIFIERS [IMPROVE IT ADD BASIC TRANSFORMATION AND RENAMING
# 4. CONVERT THE MATRIX INTO CNF.
# 5. PUT QUANTIFIERS BACK AND ADD EXITENCE QUANTIFIERS TO NEW VARIABLE.
# 6. CREATE A QDIAMACS MAPPING.

# CURRENT PROBLEMS :
# 1. CHECK WHETHER THESE [X,Y] ARE VARIABLES OR CONSTANTS.
# 2. HANDLE FUNCTIONS.

from z3 import *

def nnf(e, seen, sign):
    if is_quantifier(e):
        if e.is_forall(): 
            var_list = []
            for i in range(e.num_vars()):
                c = Const(e.var_name(i) + "", e.var_sort(i))
                if sign == False:
                    quant_set.append(('E', c ))
                else:
                    #print 'im in 1st quant'
                    quant_set.append(('A', c ))
                    #print quant_set
            return nnf( e.body(), seen, sign ) 
        
        else:
            var_list = []
            for i in range(e.num_vars()):
                c = Const(e.var_name(i) + "", e.var_sort(i))
                var_list.append(c)
                if sign == True:
                    quant_set.append(('E', c ))
                else:
                    quant_set.append(('A', c ))
            return nnf( e.body(), seen, sign ) 
    
    # The expression can be an Boolean.
    if is_bool(e):
    # Handle And: Return And (rest formula)
        if (is_and(e)):
            n = e.num_args()
            arg_list = []
            for i in range(n):
                #print (e.arg(i))
                arg_list.append( nnf(e.arg(i), seen, sign))
            if sign == False:
                #print 'Going for the print arg_list'
                #print arg_list
                #print is_var(arg_list[0])
                #print is_const(arg_list[0])
                #exit(0)
                return Or(arg_list)
            else:
                return And(arg_list)
        
    # Handle And: Return And (Neg(inl), Neg(inr))
        if (is_or(e)):
            n = e.num_args()
            arg_list = []
            for i in range(n):
                #print (e.arg(i))
                arg_list.append( nnf(e.arg(i), seen, sign))
            if sign == False:
                return And(arg_list)
            else:
                return Or(arg_list)
       
    # Handle NOT: return Not(Const)
        if (is_not(e)):
            if sign == True:
                sign = False
            else: 
                sign = True
            for i in e.children():
                return nnf(i, seen, sign) 
    
    # Handle variables
        if (is_var(e)):
            #print 'Can someone gets heres :)'
            if sign == True:
                return e
            else:
                return Not(e)
        
# Handle the constant case        
        if(is_const(e)):
            #print 'Mitroon... :)'
            if sign == True:
                return e
            else:
                return Not(e)

# Basic nnf is commented out ...#####
#def nnf(e, seen, sign):
#    if is_quantifier(e):
#        if e.is_forall(): 
#            var_list = []
#            for i in range(e.num_vars()):
#                c = Const(e.var_name(i) + "", e.var_sort(i))
#                var_list.append(c)
#            if sign == False:
#                return Exists(var_list, nnf(e.body(), seen, sign))
#            else:
#                return ForAll( var_list, nnf( e.body(), seen, sign ) )
#        
#        else:
#            var_list = []
#            for i in range(e.num_vars()):
#                c = Const(e.var_name(i) + "", e.var_sort(i))
#                var_list.append(c)
#            if sign == False:
#                return ForAll(var_list, nnf(e.body(), seen, sign))
#            else:
#                return Exists(var_list, nnf(e.body(), seen, sign))
#    
#    # The expression can be an Boolean.
#    if is_bool(e):
#    # Handle And: Return And (rest formula)
#        if (is_and(e)):
#            n = e.num_args()
#            arg_list = []
#            for i in range(n):
#                #print (e.arg(i))
#                arg_list.append( nnf(e.arg(i), seen, sign))
#            if sign == False:
#                return Or(arg_list)
#            else:
#                return And(arg_list)
#        
#    # Handle And: Return And (Neg(inl), Neg(inr))
#        if (is_or(e)):
#            n = e.num_args()
#            arg_list = []
#            for i in range(n):
#                #print (e.arg(i))
#                arg_list.append( nnf(e.arg(i), seen, sign))
#            if sign == False:
#                return And(arg_list)
#            else:
#                return Or(arg_list)
#       
#    # Handle NOT: return Not(Const)
#        if (is_not(e)):
#            if sign == True:
#                sign = False
#            else: 
#                sign = True
#            for i in e.children():
#                return nnf(i, seen, sign) 
#    
#    # Handle variables
#        if (is_var(e)):
#            print 'Can someone gets heres :)'
#            return e
#        
## Handle the constant case        
#        if(is_const(e)):
#            if sign == True:
#                return e
#            else:
#                return Not(e)
# Handle the variable case.  
# if(is_const(e)):
# return e

x, y, z = Bools('x y z')
sign = True
seen = {}
quant_set = [] 

f =  Function('f', BoolSort(), BoolSort(), BoolSort())

#fml = ForAll( x, ForAll ( y, And( x, y) ) )
#fml = ForAll (x, ForAll (y, ForAll (z, Or (And(x,y), And (y,z)) )))
fml = ForAll (x, Exists (y,  And(x,y) ))

#g = Goal()
#g.add(fml)

t1 = Tactic('simplify')
t2 = Tactic('nnf')
t3 = Tactic('tseitin-cnf')

#tr = Then(t1,t2)
#trx = Then(t1,t3)

# STEP 1: ~SIMPLIFY ND NNF -- IMPLIES ELIM AND NNF PROP
#tr = Tactic('simplify')(fml).as_expr()
tr = Then(Tactic('simplify'),Tactic('nnf'))(fml).as_expr()
#r = tr(fml)
print tr
exit(0)

# NNF DOES: BOTH NNF TRANSFORMTAION AND PULL QUANTIFIERS OUT
nnf_fml = nnf(tr, seen, sign)
print nnf_fml
#print is_quantifier(terror)
#exit(0)

# CHECK THE QUNATIFIERS THAT THE SUBEXPRESSIONS HAVE AND PULL THE VARIABLE TO THE TREE ONE LEVEL A
#  REQUIRE RENAMING THE VARIABLE ...

# PULL QUA1NTIFIER OUT
#nnf_fml = nnf(terror, seen)

#print 'The resultant formula after bs is : \t'
#print nnf_fml

# STEP 2: SIMPLIFY ND CNF
trx = Then(Tactic('simplify'),Tactic('tseitin-cnf'))(nnf_fml).as_expr()
#trx = Then(t1,t3)
#rx = trx(tr)
print(trx)
print quant_set

#exit(0)
#t = Then (Then(t1,t2), t3)

# FILL THE QUANTIFIERS BACK AND ALSO TAKE CARE OF ADDITIONAL EXISTENTIAL QUANTIFIER
# ADDED FOR AUX VARIABLES......
#def create_formula(quant_set, trx):
#    i = quant_set[0]
#    if len(quant_set) > 1:
#        if i[0] == 'A':
#            return ForAll ( i[1] + "" , create_formula(quant_set.pop(0), trx))
#        else:
#            return Exists ( i[1] + "" , create_formula(quant_set.pop(0), trx))
#    else:
#        if i[0] == 'A':
#            return ForAll ( i[1] + "", trx )
#        else:
#            return Exists ( i[1] + "", trx )
#
#build_formula =  create_formula(quant_set, trx)
#print build_formula

# QDIAMAC TRANSFORMATION .... EASY
# QDIAMACS FORMAT :
# 1. The Preamble
#  1.1 Comments
#       c This is an example of a comment line.
#  1.2 Problem line
#      p FORMAT VARIABLES CLAUSES
#  2. The Clauses
#     The variables are assumed to be numbered from 1 up to n
#     The negated version is represented by -i.
#     s, e and VAR_list: ForAll / Exists
#     Each clauses is terminated by value 0.
# Example :
#  p cnf 4 2
#  e 1 2 3 4 0
#  -1  2 0
#   2 -3 -4 0
# ALGO :
