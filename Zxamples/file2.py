from z3 import *

x = Int('x')
y = Int('y')

# Simplifying the expression:
#print simplify(x+y + 2*x + 3)
#print simplify (x <y + x +2)
#print simplify (And (x +1 >= 3, x**2 + y**2 + 2 >=5))

#print x **2 + y **2 >= 1
#set_option(html_mode=True)
#print x **2 + y **2 >= 1

# Functions for traversing expressions.
n = x + y >= 3
print "num args: ", n.num_args()
print "children: ", n.children()
print "1st child:", n.arg(0)
print "2st child:", n.arg(1)
print "operator: ",n.decl()
print "operator name: ", n.decl().name()

# Solving nonlinear polynomial constraints.
solve(x**2 + y**2 == 3, x**3 == 2)
#fail to solve

# Works over Real
x = Real('x')
y = Real('y')
solve(x**2 + y**2 > 3, x**3 + y < 5)
solve(x**2 + y**2 == 3, x**3 == 2)

set_option(precision=30)
print "Solving, and displaying results with 30 decimal places"
solve(x**2 + y**2 == 3, x**3 == 2)

# Mistake 1/3 is a python integer, nt a Z3 rational number
# Create a Z3 rational
print 1/3        # Output: 0
print RealVal(1)/3 # 1/3
print Q(1,3)       #1/3

x = Real('x')
print x + 1/3   # print prints string
print x + Q(1,3)
print x + "1/3"
print x + 0.25

# Rational number with decimal notation
x = Real('x')
solve(3**x == 1)  # Unable to solve

#solve(3*x == 1)
#set_option(rational_to_decimal=True)
#solve(3*x == 1)

#set_option(precision=30)
#solve(3*x == 1)

# System is unsatisfiable.
x = Real('x')
solve(x > 4, x < 0)

# # means comments print always prints a String.
# Z3Py does not support comments that span more than one line.

# BOOLeAN LOGIC
# Z3 Boolean operators
# Z3 supports Boolean operators:
# And, Or, Not, Implies (implication), If (if-then-else). Bi-implications are represented using equality ==. The
p = Bool('p')
q = Bool('q')
r = Bool('r')
solve(Implies(p,q), r == Not(q), Or(Not(p),r))

# The Python Boolean constants True and False can be used to build Z3 Boolean expressions.
p = Bool('p')
q = Bool('q')
print And(p,q,True)
print simplify(And(p,q,True))
print simplify(And(p,False))

# A combination of polynomial and Boolean constraints
p = Bool('p')
x = Real('x')
solve(Or(x < 5, x >10), Or(p,x**2 == 2), Not(p))

# adding or in front makes it and -> Or transition. 
solve (Or(x<5,x>10))
solve (And(x<5,x>10))

# SOLVERS
x = Int('x')
y = Int('y')
s = Solver()
print s

s.add(x>10,y == x +2)
print s
print "solving constraints in the solver s..."
print s.check()

print "create a new scope"
s.push()
s.add(y<11)
print s
print "solving updated set of constraints..."
print s.check()


solve (Or(x<5,x>10))

print "restoring the state"
s.pop()
print s
print "solving the restored set of constraints.."
print s.check()
print "adding the no solution clasue"
s.add(x>10, x <9)
print s
print s.check()
print "adding the unsolvable B.S"
s.add(3**x == 1)  # Unable to solve
print s
print s.check()

# Unknown case
# Z3 can solve nonlinear polynomial constraints, but 2**x is not a polynomial.
x = Real('x')
s = Solver()
s.add(2**x == 3)
print s.check()

# Traverse the constraints asserted into a solver.
x = Real('x')
y = Real('y')
s = Solver()
s.add(x>1,y>1,Or(x+y > 3, x - y <2))
print "asserted constraints..."
for c in s.assertions():
    print c

# Collect performance statistics for the check method.
print s.check()
print "statistics for the last check method..."
print s.statistics()
# Traversing statistics
for k,v in s.statistics():
    print "%s : %s" % (k,v)

x,y,z = Reals('x y z')
s = Solver()
s.add(x>1,y>1,x+y > 3, z - x < 10)
print s.check()

m = s.model()
print "x = %s" % m[x]

# model directly shown by using solve
#solve(x>1,y>1,x+y > 3, z - x < 10)

print "traversing the model"
#for i in m:
#    print m[i]

for d in m.decls():
    print" %s = %s" % (d.name(), m[d])



