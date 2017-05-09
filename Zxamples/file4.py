from z3 import *

# In Z3, expressions, sorts and declarations are called ASTs. 
# ASTs are directed acyclic graphs.

x = Int('x')
y = Real('y')
print(x + 1).sort()
print(x + 1).sort()
print(x >= 1).sort()

# The function eq(n1, n2) returns True if n1 and n2 are the same AST. This is a structural test.

x,y = Ints('x y')
print eq(x+y, x+y)

print(x+1).hash()
print(1+x).hash()
print x.sort().hash()


x = Int('x')
print "is expres: ", is_expr(x)
n = x + 1
print "is exp: ", is_app(n)
print "decl: ", n.decl()
print "num args: ", n.num_args()
for i in range(n.num_args()):
    print "arg(", i, ") ->",n.arg(i)

# Declerations have names

x = Int('x')
x_d = x.decl()
print "is_expr:",is_expr(x_d)
print "is_func_decl(x_d):",is_func_decl(x_d)
print "x_d.name(): ",x_d.name()
print "x_d.range(): ",x_d.range()
print "x_d.arity(): ",x_d.arity()

#print "x_d.decl(): ",x_()

#x_d() create an application with 0 argument using x_d
print "eq(x_d(),x):",eq(x_d(),x)
print "\n"

# f is a function from (Int,real) to Bool

f = Function('f',IntSort(),RealSort(),BoolSort())
print "f.name()",f.name()
print "f.range():",f.range()
print "f.arity():",f.arity()

for i in range(f.arity()):
    print "domain(",i,")",f.domain(i)

#f(x,x) creates an application with 2 arguments using f.

print f(x,x)
print eq(f(x,x).decl(),f)




