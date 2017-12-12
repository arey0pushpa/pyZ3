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
import os

# Wrapper for allowing Z3 ASTs to be stored into Python Hashtables. 
class AstRefKey:
    def __init__(self, n):
        self.n = n
    def __hash__(self):
        return self.n.hash()
    def __eq__(self, other):
        return self.n.eq(other.n)
    def __repr__(self):
        return str(self.n)

def askey(n):
    assert isinstance(n, AstRef)
    return AstRefKey(n)

#def get_vars(f):
#    r = set()
#    if (is_const(f) or is_var(f)): 
#        for c in f.children():
#            collect(c)
#    collect(f)
#    return r

def nnf( e, seen, sign ):
    index = 1
    if is_quantifier(e):
        if e.is_forall(): 
            var_list = []
            for i in range( e.num_vars() ):
                c = Const( e.var_name(i), e.var_sort(i) )
                var_list.append( ( c, index ) )
                index = index + 1 
            if sign == False:
                if (quant_set != [] and quant_set[-1][0] == 'E'):
                        quant_set[-1][1] = quant_set[-1][1] + var_list  
                else:
                    quant_set.append(( 'E', var_list ) )
            else:
                if (quant_set != []  and quant_set[-1][0] == 'A'):
                    quant_set[-1][1] = quant_set[-1][1] + var_list  
                else:
                    quant_set.append( ( 'A', var_list ) )
            return nnf( e.body(), seen, sign )
        else:
            var_list = []
            for i in range( e.num_vars() ):
                c = Const( e.var_name(i), e.var_sort(i) )
                var_list.append( ( c, index ) )
                index = index + 1
            if sign == True:
                if (quant_set != [] and quant_set[-1][0] == 'E'):
                        quant_set[-1][1] = quant_set[-1][1] + var_list  
                else:
                    quant_set.append(( 'E', var_list ) )
            else:
                if (quant_set != [] and quant_set[-1][0] == 'A'):
                    quant_set[-1][1] = quant_set[-1][1] + var_list  
                else:
                    quant_set.append( ( 'A', var_list ) )
            return nnf( e.body(), seen, sign )
        
    # The expression can be an Boolean.
    if is_bool(e):
    # Handle And: Return And (rest formula)
        if (is_and(e)):
            n = e.num_args()
            arg_list = []
            for i in range(n):
                arg_list.append( nnf(e.arg(i), seen, sign))
            if sign == False:
                return Or(arg_list)
            else:
                return And(arg_list)
        
    # Handle And: Return And (Neg(inl), Neg(inr))
        if (is_or(e)):
            n = e.num_args()
            arg_list = []
            for i in range(n):
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

# Build formula at the end by combining the quantifiers.
def create_formula(quant_set, trx):
    i = quant_set[0]
    if len(quant_set) > 1:
        if i[0] == 'A':
            return ForAll(i[1], create_formula(quant_set[1:], trx))
        else:
            return Exists(i[1], create_formula(quant_set[1:], trx))
    else:
        if i[0] == 'A':
            return ForAll(i[1], trx )
        else:
            return Exists(i[1], trx )


x, y, z = Bools('x y z')
sign = True
seen = {}
quant_set = []

f =  Function('f', BoolSort(), BoolSort(), BoolSort())

#fml = ForAll( x, ForAll ( y, And( x, y) ) )
fml = ForAll (x, ForAll (y, ForAll (z, Or ( And(x,y), And (y,z)) )))
#fml = ForAll (x, Exists (y,  And ( And(x,y), ForAll(z, Or( And(x,y), And(y,z))))))

#g = Goal()
#g.add(fml)

t1 = Tactic('simplify')
t2 = Tactic('nnf')
t3 = Tactic('tseitin-cnf')

#tr = Then(t1,t2)
#trx = Then(t1,t3)

# STEP 1: ~SIMPLIFY ND NNF -- IMPLIES ELIM AND NNF PROP
tr = Tactic('simplify')(fml).as_expr()
#tr = Then(Tactic('simplify'),Tactic('nnf'))(fml).as_expr()
#r = tr(fml)
print 'Impl_elimd : ' + str(tr)
#exit(0)

# NNF DOES: BOTH NNF TRANSFORMTAION AND PULL QUANTIFIERS OUT
nnf_fml = nnf(tr, seen, sign)
print 'NNfd : ' + str(nnf_fml)
#print is_quantifier(terror)
#exit(0)

var_present = get_vars(nnf_fml)
print var_present

# CHECK THE QUNATIFIERS THAT THE SUBEXPRESSIONS HAVE AND PULL THE VARIABLE TO THE TREE ONE LEVEL A
#  REQUIRE RENAMING THE VARIABLE ...

# PULL QUA1NTIFIER OUT
#nnf_fml = nnf(terror, seen)

#print 'The resultant formula after bs is : \t'
#print nnf_fml

# ADDITIONAL SETUP WHERE EVERY DISTINCT VARIABLES IN A DICTIONARY.

# STEP 2: SIMPLIFY ND CNF
#trx = Then(Tactic('simplify'),Tactic('tseitin-cnf'))(nnf_fml).as_expr()
#var_added_with_present = get_vars(trx) 
#print var_added_with_present 

#trx = Then(t1,t3)
#rx = trx(tr)
#print 'CNfd : ' + str(trx)
#print quant_set


t = Then('simplify', 'tseitin-cnf')
subgoal = t(nnf_fml)
assert len(subgoal) == 1

# Traverse each clause of the first subgoal
cnf_list = []
for c in subgoal[0]:
        #print "children: ", c.children()
        #print "1st child:", c.arg(0)
        #print "operator: ", c.decl()
        #print "op name:  ", c.decl().name()
        cnf_list.append(c)
        #print c
# Prints cnf list of the formula
print 'CNfd : ' + str(cnf_list)
#print cnf_list
#print get_var_index(cnf_list[0])

#exit(0)
#t = Then (Then(t1,t2), t3)

# FILL THE QUANTIFIERS BACK AND ALSO TAKE CARE OF ADDITIONAL EXISTENTIAL QUANTIFIER
# ADDED FOR AUX VARIABLES..... try freshVar().
var_set = []
for i in quant_set:
    var_set = var_set + i[1]

print quant_set

# CHECK ADDITIONAL VARIABLES PRESENT 
#print get_vars(trx)
new_set = var_set 
# DO IT AFTER ADDING TO A DICTIONARY...

# ADD ADITIONAL VARIABLES DUE TO TSETING ...
dict_set = {k : v for k,v in  var_set}
#dict_set = dict( zip( var_list, index_list))
print dict_set
#print var_list


#build_formula =  create_formula(quant_set, trx)
#print build_formula

# QDIAMAC TRANSFORMATION .... 
q_var =  len(new_set)
num_clause = len(cnf_list)

#if os.path.exists("myfile.qdimacs"):
#    f = file("myfile.qdimacs", "r+")
#else:
#    f = file("myfile.qdimacs", "w")
with open('myfile.qdimacs', 'r+') as f:
    f.seek(0)
    text = 'c This is a QDIMACS output '
    f.write(text)
    f.write("\n")
    text = 'p' + ' ' +  'cnf'  + ' ' + str(q_var) + ' ' + str(num_clause) + '\n'
    f.write(text)
    bit = True
    for q in quant_set:
        if q[0] == 'A':
            f.write('a' + ' ')
            for i in range( len( q[1] ) ):
                f.write( str( dict_set[ q[1][i][0] ] ) + ' ') 
            f.write(str(0) + '\n')
        else:
            f.write('e' + ' ')
            for i in range(q[1]):
                f.write( str( dict_set[ q[1][i][0] ] ) + ' ') 
            f.write(str(0) + '\n')

    for e in cnf_list:
        if is_or(e): 
            n = e.num_args()
            for i in range(n):
                if is_not( e.arg(i) ):
                    f.write('-' + str( dict_set[ e.arg(i) ] ) )   
                else:
                    f.write( str( dict_set[ e.arg(i) ] ) ) 
            f.write(str(0) + '\n')
        else:
            if is_not( d[ e.arg(i) ] ):
                f.write('-' + str( dict_set[ e.arg(i) ] ) ) 
            else:
                f.write( str( dict_set[ e.arg(i) ] ) ) 
            f.write(str(0) + '\n')
    f.truncate()

#f.write('p' + ' ' +  'cnf'  + ' ' + str(q_var) + ' ' + str(num_clause))
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
