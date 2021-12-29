"""
Model of an Andrews squeezing mechanism.
"""

from pymbs.symbolics import VarKind
from pymbs.input import *


# Set up world
world = MbsSystem([0, 0, 0])

# Set up parameters (mass and inertia)
m1 = world.addParam('m1', 0.04325, name='Mass1', positive=True)
I1 = world.addParam('I1', 2.194e-6, name='Inertia1', positive=True)
m2 = world.addParam('m2', 0.00365, name='Mass2', positive=True)
I2 = world.addParam('I2', 4.410e-7, name='Inertia2', positive=True)
m3 = world.addParam('m3', 0.02373, name='Mass3', positive=True)
I3 = world.addParam('I3', 5.255e-6, name='Inertia3', positive=True)
m4 = world.addParam('m4', 0.00706, name='Mass4', positive=True)
I4 = world.addParam('I4', 5.667e-7, name='Inertia4', positive=True)
m5 = world.addParam('m5', 0.07050, name='Mass5', positive=True)
I5 = world.addParam('I5', 1.169e-5, name='Inertia5', positive=True)
m6 = world.addParam('m6', 0.00706, name='Mass6', positive=True)
I6 = world.addParam('I6', 5.667e-7, name='Inertia6', positive=True)
m7 = world.addParam('m7', 0.05498, name='Mass7', positive=True)
I7 = world.addParam('I7', 1.912e-5, name='Inertia7', positive=True)

# Set up parameters (positions)
xa = world.addParam('xa', -0.06934, negative=True)
ya = world.addParam('ya', -0.00227, negative=True)
xb = world.addParam('xb', -0.03635, negative=True)
yb = world.addParam('yb',  0.03273, positive=True)
xc = world.addParam('xc',  0.014, positive=True)
yc = world.addParam('yc',  0.072, positive=True)

# Set up parameters (lengths)
d = world.addParam('d', 0.028, positive=True)
da = world.addParam('da', 0.0115, positive=True)
e = world.addParam('e', 0.02, positive=True)
ea = world.addParam('ea', 0.01421, positive=True)
rr = world.addParam('rr', 0.007, positive=True)
ra = world.addParam('ra', 0.00092, positive=True)
ss = world.addParam('ss_', 0.035, positive=True)
sa = world.addParam('sa', 0.01874, positive=True)
sb = world.addParam('sb', 0.01043, positive=True)
sc = world.addParam('sc', 0.018, positive=True)
sd = world.addParam('sd', 0.02, positive=True)
ta = world.addParam('ta', 0.02308, positive=True)
tb = world.addParam('tb', 0.00916, positive=True)
u = world.addParam('u', 0.04, positive=True)
ua = world.addParam('ua', 0.01228, positive=True)
ub = world.addParam('ub', 0.00449, positive=True)
zf = world.addParam('zf', 0.02, positive=True)
zt = world.addParam('zt', 0.04, positive=True)

# Spring
c0 = world.addParam('c0', 4530.0, positive=True)
l0 = world.addParam('l0', 0.07785, positive=True)
fa = world.addParam('fa', 0.01421, positive=True)
mom = world.addParam('mom', 0.033, positive=True)

# Add Bodies
world.addFrame('A', [xa, ya, 0])
world.addFrame('B', [xb, yb, 0])
world.addFrame('C', [xc, yc, 0])

K1 = world.addBody(m1, cg=[ra, 0, 0], inertia=diag([0, 0, I1]), name='K1')
K1.addFrame('K2', [rr, 0, 0])

K2 = world.addBody(m2, cg=[-da, 0, 0], inertia=diag([0, 0, I2]), name='K2')
K2.addFrame('C', [-d, 0, 0])

K3 = world.addBody(m3, cg=[sb, -sa, 0], inertia=diag([0, 0, I3]), name='K3')
K3.addFrame('C', [0, -ss, 0])
K3.addFrame('Spring', [sd, -sc, 0])

K4 = world.addBody(m4, cg=[0, ea, 0], inertia=diag([0, 0, I4]), name='K4')
K4.addFrame('K5', [0, e, 0])

K5 = world.addBody(m5, cg=[ta, tb, 0], inertia=diag([0, 0, I5]), name='K5')
K5.addFrame('K4', [zt, 0, 0])

K6 = world.addBody(m6, cg=[-fa, 0, 0], inertia=diag([0, 0, I6]), name='K6')
K6.addFrame('K7', [-zf, 0, 0])

K7 = world.addBody(m7, cg=[-ua, -ub, 0], inertia=diag([0, 0, I7]), name='K7')
K7.addFrame('K6', [0, -u, 0])

# Joints
world.addJoint(name='rot_beta', CS1=world, CS2=K1, dofList='Rz',
               startVals=-0.0617138900142764496358948458001)
world.addJoint(name='rot_Theta', CS1=K1.K2, CS2=K2, dofList='Rz',
               startVals=0.0)
world.addJoint(name='rot_gamma', CS1=world.B, CS2=K3, dofList='Rz',
               startVals=0.455279819163070380255912382449)
world.addJoint(name='rot_delta', CS1=world.A, CS2=K5, dofList='Rz',
               startVals=0.487364979543842550225598953530)
world.addJoint(name='rot_epsilon', CS1=world.A, CS2=K7, dofList='Rz',
               startVals=1.23054744454982119249735015568)

# Loops
useLoops = True
if (useLoops):
    world.addJoint(name='Rot4', CS1=K3.C, CS2=K4, dofList='Rz')
    world.addJoint(name='Rot6', CS1=K3.C, CS2=K6, dofList='Rz')

    world.addLoop.FourBar(K5.K4, K4.K5, posture=-1)
    world.addLoop.FourBar(K7.K6, K6.K7, posture=1)
    world.addLoop.FourBar(K2.C, K3.C, posture=1)

else:
    world.addJoint(name='rot_Phi', CS1=K5.K4, CS2=K4.K5, dofList='Rz',
                   startVals=0.222668390165885884674473185609)
    world.addJoint(name='rot_Omega', CS1=K7.K6, CS2=K6.K7, dofList='Rz',
                   startVals=-0.222668390165885884674473185609)

    world.addConstraint('Loop1', K2.C, K3.C, [1, 1, 0])
    world.addConstraint('Loop2', K2.C, K4, [1, 1, 0])
    world.addConstraint('Loop3', K2.C, K6, [1, 1, 0])

# Force elements
L = world.addSensor.Distance('L', K3.Spring, world.C, 'SpringLength')
F = world.addExpression('F', -c0*(L[0]-l0), 'Force')
world.addLoad.PtPForce(F, K3.Spring, world.C, 'Spring')
world.addLoad.CmpTorque([0, 0, -mom], world, K1, name='DrivingTorque')

# Visualiation
world.addVisualisation.Frame(world, 0.01)
world.addVisualisation.Frame(world.A, 0.01)
world.addVisualisation.Frame(world.B, 0.01)
world.addVisualisation.Frame(world.C, 0.01)
world.addVisualisation.Frame(K3.Spring, 0.01)

world.addVisualisation.Line(K1, 0.007) #rr
world.addVisualisation.Line(K2, -0.028) #-d
K3.addFrame('Line', R=rotMat(3.141/2, 'z'))
world.addVisualisation.Line(K3.Line, 0.035) #ss
K4.addFrame('Line', R=rotMat(-3.141/2, 'z'))
world.addVisualisation.Line(K4.Line, 0.02) #e
world.addVisualisation.Line(K5, 0.04) #zt
world.addVisualisation.Line(K6.K7, 0.02) #zf
K7.addFrame('Line', R=rotMat(3.141/2, 'z'))
world.addVisualisation.Line(K7.Line, 0.04) #u

# Generate equations, code and show assembly
E_K1 = world.addSensor.Energy('E_K1', K1)
E_K2 = world.addSensor.Energy('E_K2', K2)
E_K3 = world.addSensor.Energy('E_K3', K3)
E_K4 = world.addSensor.Energy('E_K4', K4)
E_K5 = world.addSensor.Energy('E_K5', K5)
E_K6 = world.addSensor.Energy('E_K6', K6)
E_K7 = world.addSensor.Energy('E_K7', K7)

E = world.addExpression('E', E_K1+E_K2+E_K3+E_K4+E_K5+E_K6+E_K7, name='Energy',
                        category=VarKind.Sensor)

world.genEquations.Explicit(diff=[0, 1, 2], simplify=False)
#world.genCode.Matlab('Andrew', './Output',  symbolic=False) # set false!
#world.genCode.Modelica('Andrew', './Output')     # Modelica
#world.genCode.Python('Andrew', './Output')     # Python
#world.genCode.Fortran('Andrew', './Output')      # Fortran
#world.genCode.Cpp('Andrew', './Output')    # C++
#world.genMatlabAnimation('Andrew', './Output',
#                         axislimits=(-0.07, 0.02, -0.02, 0.07, -0.01, 0.01))
world.show('Andrews_Squeezing_Mechanism')
