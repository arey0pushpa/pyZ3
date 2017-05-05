from z3 import *

#ArITHMATIC
# Z3 automatically add coersion to the conversions.
x = Real('x')
y = Int('y')

a,b,c = Reals('a b c')
s,r = Ints('s r')

print x + y + 1 + (a + s)
print ToReal(y) + c
a = ToReal(y)
print a

# All basic opertaor.
a,b,c = Ints('a b c')
d,e = Reals('d e')
solve(a > b + 2,
        a == 2 * c + 10,
        c + b <= 1000,
        d >= e)

# Simple transformations on Z3 expressions.
x ,y = Reals('x y')
# Sum of monomial
t = simplify((x + y)**3, som=True)
print t

# power operator
t = simplify(t, mul_to_power=True)
print t

print simplify(x== y+2,':arith-lhs',True)
print simplify(x== y+2, arith_lhs=True)

print "\nAll available options:"
help_simplify()

x,y = Reals('x y')
solve(x + 10000000000000000000000 == y, y > 20000000000000000)

print Sqrt(2) + Sqrt(3)
print simplify(Sqrt(2) + Sqrt(3))
print simplify(Sqrt(2) + Sqrt(3)).sexpr()
print (x + Sqrt(y) * 2).sexpr()

# Machine Arithmetic
x = BitVec('x',16)
y = BitVec('y',16)
print x + 2
#Interbally 
print (x+2).sexpr()

print simplify(x + y -1) # -1 iss equal to 65535 ?

# Bitvector constants
a = BitVecVal(-1,16)
b = BitVecVal(65535,16)
print simplify(a==b)

a = BitVecVal(-1,32)
b = BitVecVal(65535,32)
print simplify(a==b)

x,y = BitVecs('x y', 32)
solve(x + y == 2, x >0 , y>0)

solve(x & y == ~y)
solve(x<0)

solve(ULT(x,0))

x,y = BitVecs('x y',32)
solve(x >> 2 == 3)
solve (x << 2 == 3) 
solve(x << 2 == 24)


# FUNCTIONS:
# Z3 have no side-effects and are total.
# Z3 is based on first-order logic.

# uninterpreted integer constants (aka variables) x, y
x = Int('x')
y = Int('y')
f = Function('f', IntSort(),IntSort())
s = Solver()
s.add(f(f(x)) == x, f(x) == y, x !=y)
print s.check()
m = s.model()
print "f(f(x)) = ", m.evaluate(f(f(x)))
print "f(x) =" ,m.evaluate(f(x))

# SAT and VALIDITY
# Validity is about finding a proof of a statement; satisfiability is about finding a solution to a set of constraints. 
p,q = Bools('p q')

# LIST CMPREHENSION

print [x+1 for x in range(1,5)]
# Create lists containing 5 integer var

X = [Int ('x%s' % i) for i in range(5)]
Y = [Int ('y%s' % i) for i in range(5)]
Z = [Bool ('z%s' % i) for i in range(5)]

print X
print Y
print Z

Z[3] = 3 > 1
X[1] = 10 + 11 -1
print X
print Z


# Create a List contatining X[i] + Y[i]
X_plus_Y= [X[i] + Y[i] for i in range(5)]
print X_plus_Y

X_gt_Y= [X[i] > Y[i] for i in range(5)]
print X_gt_Y

print And(X_gt_Y)

# Create a 3 * 3 matrix of integer variables.
X = [[Int ("x_%s_%s" %(i+1,j+1)) for j in range(3)] for i in range(3)]
pp(X)
print X

# Z3Py also provides functions for creating vectors of Boolean, Integer and Real variables. These functions are implemented using list comprehensions.

X = IntVector('x',5)
Y = RealVector('y',5)
P = BoolVector('p',5)

print X
print Y
print P
print [y**2 for y in Y]
print Sum([y**2 for y in Y])

d,a,t,v_i,v_f = Reals('d a t v_i v_f')
b = Bool('b')
equations = [d== v_i * t + (a*t**2)/2,
        v_f == v_i +a*t
        ]
print "kinematic equations"
print equations
print equations[1]
print (d == v_i * t + (a*t**2)/2)

## Boolean variable always getting eval. Not sure how to avaoid this.

problem = [
        v_i == 30,
        v_f == 0,
        a == -8
        ]
print problem

solve(equations + problem)

equations = [a == 10 * t,
             d == 20 * t
            ]
problem = [
        a == 10,
        d == 10
        ]
print problem
solve(equations + problem) # We got no solution !
# This way is fantastic.

# BITVECTOR TRICKS:
x = BitVec ('x',32)
powers = [2 ** i for i in range(32)]
fast = And(x !=0 , x & (x-1) == 0)
slow = Or([x == p for p in powers])
print fast
prove(fast == slow)

print "trying to prove buggy version"
fast = x & (x -1) == 0
prove (fast == slow)

x = BitVec ('x',32)
y = BitVec ('x',32)

trick = (x ^ y) < 0
opposite = Or(And(x < 0,y>= 0),
        And(x>=0, y<0))
prove (trick == opposite)


# PUZZLE - DOG CAT MOUSE
d,c,m = Ints('d c m')
solve(d >= 1,
        c >= 1,
        m >= 1,
        c + d +  m == 100,
        1500 *d + 100 *c + 25 *m == 10000)

d,c,m = Reals('d c m')
solve(d >= 1,
        c >= 1,
        m >= 1,
        c + 15*d + 0.25 * m == 100)


# SUDOKU

# Represent Every matrix point.
X = [[Int("x_%s_%s" % (i+1,j+1)) for j in range(9)] for i in range(9)]

# Each cell contains a value 
cells_c = [And (1 <= X[i][j], X[i][j] <= 9) for i in range(9) for j in range(9)]

rows_c = [Distinct(X[i]) for i in range(9)]

cols_c = [Distinct([X[i][j] for i in range(9)]) for j in range(9)]

sq_c = [Distinct ([X(3 * i0 + i) [3 *j0 +j] 
    for i in range(3) for j in range(3)]) for i0 in range(3) for j0 in range(3)]

sudoku_c = cells_c + rows_c + cols_c + sq_c 

instance = ((0,0,0,0,9,4,0,3,0),
            (0,0,0,5,1,0,0,0,7),
            (0,8,9,0,0,0,0,4,0),
            (0,0,0,0,0,0,2,0,8),
            (0,6,0,2,0,1,0,5,0),                                                  (1,0,2,0,0,0,0,0,0),                                                  (0,7,0,0,0,0,5,2,0),
            (9,0,0,0,6,5,0,0,0),                                                  (0,4,0,9,7,0,0,0,0))

instance_c = [If(instance[i][j] == 0,
    True,
    X[i][j] == instance[i][j])
    for i in range(9) for j in range(9)]

s = Solver()
s.add(sudoku_c + instance_c)
if s.check() == sat:
    m =s.model()
    r = [[m.evaluate(X[i][j]) for j in range(9)] for i in range(9)]
    print_matrix(r)
else:
    print "filed to solve"


