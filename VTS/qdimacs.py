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

#def askey(n):
#    assert isinstance(n, AstRef)
#    return AstRefKey(n)
#
#def get_vars(f):
#    r = set()
#    def collect(f):
#        if is_var(f):
#            r.add(f)
#        if is_const(f): 
#            if f.decl().kind() == Z3_OP_UNINTERPRETED and not askey(f) in r:
#                r.add(askey(f))
#        else:
#            for c in f.children():
#                collect(c)
#    collect(f)
#    return r
def traverse_cnf( l ):
    r = set()
    def collect(e):
        if is_bool(e):
            if ( is_or(e) ):
                n = e.num_args()
                for i in range(n):
                    collect( e.arg(i) )
            if ( is_not(e) ):
                collect( e.arg(0) )
                #print e.arg(0)
                #exit(0)
            if ( is_var(e) ):
                r.add( e )
           # HANDLE ONLY BOUNDED CASE.
            if( is_const(e) ):
                r.add( e )
    for e in l :
        collect(e)
    return r

def traverse(e):
    r = set()
    def collect(e):
        if is_bool(e):
            if ( is_and(e) ):
                n = e.num_args()
                for i in range(n):
                    collect( e.arg(i) )
            if ( is_or(e) ):
                n = e.num_args()
                for i in range(n):
                    collect( e.arg(i) )
            if ( is_not(e) ):
                collect( e.arg(0) )
                #print 'i was here'
                #for i in e.children():
                #    return collect( i ) 
            if ( is_var(e) ):
                r.add( e )
           # HANDLE ONLY BOUNDED CASE.
            if( is_const(e) ):
                r.add( e )
    collect(e)
    #print r
    return r

def nnf( e, seen, sign ):
    global index
    if is_quantifier(e):
        if e.is_forall(): 
            var_list = []
            for i in range( e.num_vars() ):
                c = Const( e.var_name(i), e.var_sort(i) )
                #print index
                #exit(0)
                var_list.append( ( c, index ) )
                index = index + 1 
                #print var_list
                #print ' '
            if sign == False:
                if (quant_set != [] and quant_set[-1][0] == 'E'):
                    for i in var_list:
                        quant_set[-1][1].append(i)
                    #quant_set[-1][1] = quant_set[-1][1] + var_list  
                else:
                    quant_set.append(( 'E', var_list ) )
            else:
                if (quant_set != []  and quant_set[-1][0] == 'A'):
                    print quant_set[-1][1]
                    #print var_list
                    for i in var_list:
                        quant_set[-1][1].append(i)
                        #print quant_set
                    #quant_set[-1][1] = quant_set[-1][1] + var_list  
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
                    for i in var_list:
                        quant_set[-1][1].append(i)
                     #   quant_set[-1][1] = quant_set[-1][1] + var_list  
                else:
                    quant_set.append(( 'E', var_list ) )
            else:
                if (quant_set != [] and quant_set[-1][0] == 'A'):
                    for i in var_list:
                        quant_set[-1][1].append(i)
                    #quant_set[-1][1] = quant_set[-1][1] + var_list  
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

def is_implies(a):
    return is_app_of(a, Z3_OP_IMPLIES)

# Handle the case of the EQ:
def impl_elim( e ):
    if is_quantifier(e):
        var_list = []
        if e.is_forall(): 
            for i in range( e.num_vars() ):
                c = Const( e.var_name(i), e.var_sort(i) )
                var_list.append( c )
            return ForAll (var_list, impl_elim ( e.body() ) )
        else:
            for i in range( e.num_vars() ):
                c = Const( e.var_name(i), e.var_sort(i) )
                var_list.append( c )
            return Exists (var_list, impl_elim ( e.body() ) )
        
    # The expression can be an Boolean.
    if is_bool(e):
        if (is_and(e)):
            n = e.num_args()
            arg_list = []
            for i in range(n):
                arg_list.append( impl_elim( e.arg(i) ))
            return And(arg_list)
        
    # Handle And: Return And (Neg(inl), Neg(inr))
        if (is_or(e)):
            n = e.num_args()
            arg_list = []
            for i in range(n):
                arg_list.append( impl_elim (e.arg(i)) )
            return Or(arg_list)
       
    # Handle NOT: return Not(Const)
        if (is_not(e)):
            for i in e.children():
                return Not ( impl_elim (i) ) 

    # Handle EQ: return And( Implies (e.children(0), e.children(1)) , 
    #                        Implies (e.children(1), e.children(0) ) 
        if(is_eq(e)):
            #for i in e.children():
            #    print i
            chld = e.children()
            #print children(1)
            #exit(0)
            return And( impl_elim ( Implies ( chld[0], chld[1] ) ), 
                        impl_elim ( Implies ( chld[1], chld[0] ) ) )
            
        if(is_implies(e)):
            chld = e.children()
            return Or ( Not( impl_elim ( chld[0] ) ), impl_elim ( chld[1] ) )  
    # Handle variables
        if (is_var(e)):
            #print 'Can someone gets here :)'
            return e
# Handle the constant case
        if(is_const(e)):
            #print 'Mitroon... :)'
            return e

x, y, z = Bools('x y z')
sign = True
seen = {}
quant_set = []
index = 1

# BIG ASSUMPTION... EVERY VARIABLE IS BOUND ....
f =  Function('f', BoolSort(), BoolSort(), BoolSort())
#fml = ForAll ( x, ForAll ( y , x == y) ) 

# HANDLE IF SOMETHING VAPOURIZE IN AIR...
#fml = ForAll (x, ForAll( y, ForAll ( z, And ( Or( x, y), Or( x, Not(x), z)) )))
#fml = ForAll( x, ForAll ( y, And( x, y) ) )
#fml = ForAll (x, ForAll (y, ForAll (z, Or ( And(x,y), And (y,z)) )))
#fml = ForAll (x, Exists (y,  And ( And(x,y), ForAll(z, Or( And(x,y), And(y,z))))))

fml = ForAll( x, ForAll ( y, Exists (z, Or( z == x,  z == y) )))
#fml = ForAll( x, ForAll ( y, Exists (z, z == Or (x, y) ) ) )
#g = Goal()
#g.add(fml)

t1 = Tactic('simplify')
t2 = Tactic('nnf')
t3 = Tactic('tseitin-cnf')

#tr = Then(t1,t2)
#trx = Then(t1,t3)

# STEP 1: ~SIMPLIFY ND NNF -- IMPLIES ELIM AND NNF PROP
trx = Tactic('simplify')(fml).as_expr()
#tr = Then(Tactic('simplify'),Tactic('nnf'))(fml).as_expr()
#r = tr(fml)

tr = impl_elim( trx )
print 'Impl_elimd : ' + str(tr)
#exit(0)

# NNF DOES: BOTH NNF TRANSFORMTAION AND PULL QUANTIFIERS OUT
nnf_fml = nnf(tr, seen, sign)
print 'NNfd : ' + str(nnf_fml)

nnf_vars = traverse( nnf_fml )
print 'nnf_vars : '+ str( nnf_vars )

def find_index(i):
    return get_var_index( i ) 
sorted_vars = sorted( nnf_vars, key = find_index, reverse = True ) 
#print sorted_vars
# ADDITIONAL SETUP WHERE EVERY DISTINCT VARIABLES IN A DICTIONARY.
var_set = []
for i in quant_set:
    var_set = var_set + i[1]
#print var_set
var_const_map = dict (zip( sorted_vars, var_set ) )
#print var_const_map

# CHECK THE QUNATIFIERS THAT THE SUBEXPRESSIONS HAVE AND PULL THE VARIABLE TO THE TREE ONE LEVEL A
#  REQUIRE RENAMING THE VARIABLE ...

# STEP 2: SIMPLIFY ND CNF
#trx = Then(Tactic('simplify'),Tactic('tseitin-cnf'))(nnf_fml).as_expr()

# take second element for sort
#trx = Then(t1,t3)
#rx = trx(tr)
#print 'CNfd : ' + str(trx)
#print quant_set

t = Then('simplify', 'tseitin-cnf')
subgoal = t( nnf_fml )
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

print '\nCNfd : ' + str(cnf_list)
cnf_vars = traverse_cnf( cnf_list ) 
print 'cnf_vars : ' + str( cnf_vars ) 

var_diff =  nnf_vars ^ cnf_vars 
#print var_diff
#exit(0)

var_list = []
if (len(var_diff) != 0):
    val_map = len(var_set) + 1
    for v in var_diff:
        tup = ( v, val_map )
        val_map = val_map + 1
        var_list.append( tup )
        var_const_map[v] = (v, len( var_const_map ) + 1 )
    if (quant_set != [] and quant_set[-1][0] == 'E'):
        for i in var_list:
            quant_set[-1][1].append(i)
    else:
        quant_set.append( ('E', var_list) )

# JUST A MAPPING FROM VAR(I) TO X, Y, Z IS REQUIRED...
# DO IT AFTER ADDING TO A DICTIONARY...

# ADD ADITIONAL VARIABLES DUE TO TSETING ...
#dict_set = {k : v for k,v in  var_set}
#dict_set = dict( zip( var_list, index_list))
#print 'dictionary set is : ' + str( dict_set )
print 'var const map is : ' + str ( var_const_map )
#print var_list
#build_formula =  create_formula(quant_set, trx)
#print build_formula

# QDIAMAC TRANSFORMATION .... 
q_var =  len(var_set)
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
    print quant_set
    #exit(0)
    for q in quant_set:
        if q[0] == 'A':
            f.write('a' + ' ')
            for i in range( len( q[1] ) ):
                #print q[1][i][1]
                f.write( str( q[1][i][1]  ) + ' ') 
            f.write(str(0) + '\n')
        else:
            f.write('e' + ' ')
            for i in range( len (q[1]) ):
                #print q[1][i][1]
                f.write( str( q[1][i][1] ) + ' ') 
            f.write(str(0) + '\n')

    for e in cnf_list:
        if is_or(e): 
            n = e.num_args()
            for i in range(n):
                if is_not( e.arg(i) ):
                    wvar = var_const_map[e.arg(i).arg(0)][1]
                    f.write('-' + str( wvar ) + ' '  )  
                else:
                    wvar = var_const_map[e.arg(i)][1]
                    #f.write( str( dict_set[ e.arg(i) ] ) ) 
                    f.write( str( wvar ) + ' ' ) 
            f.write(str(0) + '\n')
        else:
            if is_not( e ):
                wvar = var_const_map[e.arg(0)][1]
                f.write('-' + str( wvar ) + ' ')
                #f.write('-' + str( dict_set[ e.arg(i) ] ) ) 
            else:
                wvar = var_const_map[e][1]
                f.write( str( wvar ) + ' ') 
                #f.write( str( dict_set[ e.arg(i) ] ) ) 
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
