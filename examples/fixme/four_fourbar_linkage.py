# -*- coding: utf-8 -*-
'''
This file is part of PyMbs.

PyMbs is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

PyMbs is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with PyMbs.
If not, see <http://www.gnu.org/licenses/>.

Copyright 2011, 2012 Carsten Knoll, Christian Schubert,
                     Jens Frenkel, Sebastian Voigt
'''
from PyMbs.Symbolics import VarKind

'''
Created on 01.04.2011

@author: Franziska Jung
'''

from PyMbs.Input import *

# set up world
world = MbsSystem([0,-1,0])

# set up parameters (mass and inertia)
m1 = world.addParam(name='Mass1', symbol_str='m1', defaultValue=0.04325) # groß
I1 = world.addParam(name='Inertia1', symbol_str='I1', defaultValue=2.194e-6)
m2 = world.addParam(name='Mass2', symbol_str='m2', defaultValue=0.00365) # klein
I2 = world.addParam(name='Inertia2', symbol_str='I2', defaultValue=4.410e-7)

# set up parameters (positions)
xa = world.addParam('xa', 0)
y = world.addParam('y', 0)
xb = world.addParam('xb', 0.02)
xc = world.addParam('xc', 0.04)
xd = world.addParam('xd', 0.06)
xe = world.addParam('xe', 0.08)

# set up parameters (lengths)
h = world.addParam('h', 0.01)
l = world.addParam('l', 0.02)
b = world.addParam('b', 0.02)

# add Bodies
world.addFrame('A', [xa,y,0])
world.addFrame('B', [xb,y,0])
world.addFrame('C', [xc,y,0])
world.addFrame('D', [xd,y,0])
world.addFrame('E', [xe,y,0])

K1 = world.addBody(name='K1', mass=m2, cg=[h/2,0,0], inertia=diag([0,I2,0]))
K1.addFrame('K6', [h,0,0])

K2 = world.addBody(name='K2', mass=m2, cg=[h/2,0,0], inertia=diag([0,I2,0]))
K2.addFrame('K7', [h,0,0])

K3 = world.addBody(name='K3', mass=m2, cg=[h/2,0,0], inertia=diag([0,I2,0]))
K3.addFrame('K8', [h,0,0])

K4 = world.addBody(name='K4', mass=m2, cg=[h/2,0,0], inertia=diag([0,I2,0]))
K4.addFrame('K9', [h,0,0])

K5 = world.addBody(name='K5', mass=m2, cg=[h/2,0,0], inertia=diag([0,I2,0]))
K5.addFrame('KX', [h,0,0])

K6 = world.addBody(name='K6', mass=m1, cg=[l/2,0,0], inertia=diag([0,I1,0]))
K6.addFrame('K1', [l,0,0])
K7 = world.addBody(name='K7', mass=m1, cg=[b/2,0,0], inertia=diag([0,I1,0]))
K7.addFrame('K2', [b,0,0])
K8 = world.addBody(name='K8', mass=m1, cg=[l/2,0,0], inertia=diag([0,I1,0]))
K8.addFrame('K3', [l,0,0])
K9 = world.addBody(name='K9', mass=m1, cg=[b/2,0,0], inertia=diag([0,I1,0]))
K9.addFrame('K4', [b,0,0])

# Joints
world.addJoint(name='Rot1', CS1=world.A, CS2=K1, dofList='Rz', startVals=-0.7)
world.addJoint(name='Rot2', CS1=world.B, CS2=K2, dofList='Rz', startVals=-0.7)
world.addJoint(name='Rot3', CS1=world.C, CS2=K3, dofList='Rz', startVals=-0.7)
world.addJoint(name='Rot4', CS1=world.D, CS2=K4, dofList='Rz', startVals=-0.7)
world.addJoint(name='Rot5', CS1=world.E, CS2=K5, dofList='Rz', startVals=-0.7)

world.addJoint(name='Rot6', CS1=K2.K7, CS2=K6, dofList='Rz', startVals=0.7-pi)
world.addJoint(name='Rot7', CS1=K3.K8, CS2=K7, dofList='Rz', startVals=0.7-pi)
world.addJoint(name='Rot8', CS1=K4.K9, CS2=K8, dofList='Rz', startVals=0.7-pi)
world.addJoint(name='Rot9', CS1=K5.KX, CS2=K9, dofList='Rz', startVals=0.7-pi)

# Loops
dae = False
if (dae == True):
    world.addConstraint('Constr_1', K1.K6, K6.K1, [1,1,0], [0,0,0])
    world.addConstraint('Constr_2', K2.K7, K7.K2, [1,1,0], [0,0,0])
    world.addConstraint('Constr_3', K3.K8, K8.K3, [1,1,0], [0,0,0])
    world.addConstraint('Constr_4', K4.K9, K9.K4, [1,1,0], [0,0,0])

else:
    world.addLoop.FourBar(K1.K6, K6.K1, posture=1)
    world.addLoop.FourBar(K2.K7, K7.K2, posture=1)
    world.addLoop.FourBar(K3.K8, K8.K3, posture=1)
    world.addLoop.FourBar(K4.K9, K9.K4, posture=1)


# Visualisierung
world.addVisualisation.Line(K1, 0.01)
world.addVisualisation.Line(K2, 0.01)
world.addVisualisation.Line(K3, 0.01)
world.addVisualisation.Line(K4, 0.01)
world.addVisualisation.Line(K5, 0.01)
world.addVisualisation.Line(K6, 0.02)
world.addVisualisation.Line(K7, 0.02)
world.addVisualisation.Line(K8, 0.02)
world.addVisualisation.Line(K9, 0.02)


# Add Sensor
world.addSensor.Orientation(CS1=world, CS2=K6.K1, symbol='RotMat1')
world.addSensor.Orientation(CS1=world, CS2=K7.K2, symbol='RotMat2')
world.addSensor.Orientation(CS1=world, CS2=K8.K3, symbol='RotMat3')
world.addSensor.Orientation(CS1=world, CS2=K9.K4, symbol='RotMat4')

E_K1 = world.addSensor.Energy(body=K1, symbol_str='E_K1', name='E_K1')
E_K2 = world.addSensor.Energy(body=K2, symbol_str='E_K2', name='E_K2')
E_K3 = world.addSensor.Energy(body=K3, symbol_str='E_K3', name='E_K3')
E_K4 = world.addSensor.Energy(body=K4, symbol_str='E_K4', name='E_K4')
E_K5 = world.addSensor.Energy(body=K5, symbol_str='E_K5', name='E_K5')
E_K6 = world.addSensor.Energy(body=K6, symbol_str='E_K6', name='E_K6')
E_K7 = world.addSensor.Energy(body=K7, symbol_str='E_K7', name='E_K7')
E_K8 = world.addSensor.Energy(body=K8, symbol_str='E_K8', name='E_K8')
E_K9 = world.addSensor.Energy(body=K9, symbol_str='E_K9', name='E_K9')

E = world.addExpression(name='Energy', symbol_str='E', exp=E_K1+E_K2+E_K3+E_K4+E_K5+E_K6+E_K7+E_K8+E_K9, category=VarKind.Sensor)

# Generate Equations, Code and Show Assembly
world.genEquations.Recursive()
world.genCode.Modelica('fourFourBarLinkage', '.\Output')
world.genMatlabAnimation('fourFourBarLinkage', '.\Output', axislimits=(-0.02, 0.1, -0.02, 0.02, -0.01, 0.01))
world.show('fourFourBarLinkage_Mechanism')
