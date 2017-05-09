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
