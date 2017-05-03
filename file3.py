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

