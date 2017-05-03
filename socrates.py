from z3 import *

Object = DeclareSort('Object')

Human = Function('Human', Object,BoolSort())
Mortal = Function('Mortal', Object,BoolSort())

socrates = Const('socrates', Object)

x = Const('x',Object)

axioms = [ForAll([x], Implies(Human(x), Mortal(x))),
        Human(socrates)]

s = Solver()
s.add(axioms)

print(s.check())

s.add(Not(Mortal(socrates)))

print(s.check())
