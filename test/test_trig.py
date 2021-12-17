from pymbs.common.functions import sym
from pymbs.common.expression import Expression

from pymbs.symbolics import sin, cos, Wild, trigsimp, sympify, Symbol

#x,y = symbols('xy')
x = Symbol('x')
y = Symbol('y')

exp1 = Expression('Test1', sym('t1'), 0)
exp2 = exp1
exp3 = exp1

exp3.symbol = sym('t4')

l = [exp1,
     Expression('Test2', sym('t2'), 0),
     Expression('Test3', sym('t3'), 0),
     Expression('Test4', sym('t4'), 0)]


print("Arbitrary")
w=Wild('w')                   # will match arbitrary expressions
print(w.matches(sympify('x+3')))
#{w_: x + 3}
print("Atom Only")
w1=Wild('w1',properties=[lambda expr: expr.is_Atom]) # will match only Atom instances
print(w1.matches(sympify('x+3')))
print(w1.matches(sympify('x')))
#{w1_: x}
print(w1.matches(sympify('4')))
#{w1_: 4}

print("And Now Something Completely Different")
a,b,c,d = list(map(Wild, 'abcd'))
pattern = a*sin(b)**2+a*cos(b)**2+d
replace = a+d

t = cos(x)**2*sin(x) + sin(x)**3

t1 = sin(x)**3
p1 = a*sin(b)**2
res = t1.match(p1)  # does not work
print(res)


print("Term:")
print(trigsimp(t))

res = t.match(pattern)  # works like a charm?!

old = pattern.subs(res)
new = replace.subs(res)

print("Alt:")
print(old)
print("Neu:")
print(new)

print("Ergebnis:")
print(t.subs( old, new )) # = replace.subs(res)

tSimple = cos(x)**2+sin(x)**2 
tSimpler = trigsimp(tSimple)
print(tSimpler)
