from sympy import *
import time


print('Start:', time.strftime("%d.%m.%Y um %H:%M:%S Uhr"))


# Symbole anlegen
v0,v1,v2 = symbols('v0,v1,v2')
v0d,v1d,v2d = symbols('v0d,v1d,v2d')

u0,u1,u2,u3,u4,u5 = symbols('u0,u1,u2,u3,u4,u5')
u0d,u1d,u2d,u3d,u4d,u5d = symbols('u0d,u1d,u2d,u3d,u4d,u5d')

v = Matrix([v0,v1,v2])
u = Matrix([u0,u1,u2,u3,u4,u5])
q = Matrix([u0,u1,u2,u3,u4,u5,v0,v1,v2])

qd = Matrix([u0d,u1d,u2d,u3d,u4d,u5d,v0d,v1d,v2d])


# Position der Zylinderanlenkpunkte
bpx,bpy,bpz,tpx,tpy,tpz = symbols('bpx,bpy,bpz,tpx,tpy,tpz')

bp = Matrix([bpx,bpy,bpz])
tp = Matrix([tpx,tpy,tpz])


# Rotationsmatrizen
Rx = Matrix([[1,       0,      0],
             [0, cos(u3),sin(u3)],
             [0,-sin(u3),cos(u3)]])

Ry = Matrix([[cos(u4),0,-sin(u4)],
             [0,      1,       0],
             [sin(u4),0, cos(u4)]])

Rz = Matrix([[ cos(u5),sin(u5),0],
             [-sin(u5),cos(u5),0],
             [       0,      0,1]])

r0 = Matrix([u0,u1,u2])


# Zwangsbedingung
l  = r0 + Rx.T*Ry.T*Rz.T*tp - bp

l_2 = Matrix([v0*cos(v1)*cos(v2),
              v0*sin(v2),
              v0*sin(v1)*cos(v2)])

print('simplifying constraint equations...')

l.simplify()
l_2.simplify()


Phi = l - l_2

Phi.simplify()


# Jacobi-Matrizen
J = Phi.jacobian(q)
Ju = Phi.jacobian(u)
Jv = Phi.jacobian(v)

print('simplifying jacobians...')

J.simplify()
Ju.simplify()
Jv.simplify()


Jv_inv = Jv.inv()
J_dot = (J*qd).jacobian(q)


print('simplifying inverse and derivative of jacobians...')

Jv_inv.simplify()
J_dot.simplify()


Bvu = -Jv_inv * Ju
b_prime = -Jv_inv * J_dot * qd

print('simplifying Bvu and b_prime...')

#Bvu.simplify()
#b_prime.simplify()


# Ergebnisse speichern
fid = open('hexapod_bvu_bprime.txt', 'w')

fid.write('Bvu\n\n')
fid.write(str(Bvu))
fid.write('\n\n\n\n')

fid.write('b_prime\n\n')
fid.write(str(b_prime))
fid.close()


print('Ende:', time.strftime("%d.%m.%Y um %H:%M:%S Uhr"))