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

'''
Created on 19.07.2009

@author: Christian Schubert
'''

from PyMbs.Input import *
from PyMbs.Symbolics import VarKind

world=MbsSystem([0,-1,0], name="Bar1")

# Parameter
l1=world.addParam('l1', 0.05, positive=True)

m2=world.addParam('m2', 0.6, positive=True)
l2=world.addParam('l2', 0.06, positive=True)
I2=world.addParam('I2', m2/12*l2**2, positive=True)

m3=world.addParam('m3', 0.65, positive=True)
l3=world.addParam('l3', 0.065, positive=True)
I3=world.addParam('I3', m3/12*l3**2, positive=True)

m4=world.addParam('m4', 0.6, positive=True)
l4=world.addParam('l4', 0.06, positive=True)
I4=world.addParam('I4', m4/12*l4**2, positive=True)

# Create Bodies with Coordinate Systems
# Bar1 = Inertial System
bar1_A = world.addFrame(name='CS_A')
bar1_D = world.addFrame(name='CS_D', p=[l1,0,0])

bar2 = world.addBody(name='Bar2', mass=m2, cg=[l2/2,0,0], inertia=diag([0,I2,I2]))
bar2.addFrame(name='CS_A')
bar2.addFrame(name='CS_B', p=[l2,0,0])

bar3 = world.addBody(name='Bar3', mass=m3, cg=[l3/2,0,0], inertia=diag([0,I3,I3]))
bar3.addFrame(name='CS_B')
bar3.addFrame(name='CS_C', p=[l3,0,0])


bar4 = world.addBody(name='Bar4', mass=m4, cg=[l4/2,0,0], inertia=diag([0,I4,I4]))
bar4.addFrame(name='CS_D')
bar4.addFrame(name='CS_C', p=[l4,0,0])

# Insert Joints
jA = world.addJoint(world.CS_A, bar2.CS_A, 'Rz', -0.5)
jB = world.addJoint(bar2.CS_B, bar3.CS_B, 'Rz',  -2.95)
jD = world.addJoint(world.CS_D, bar4.CS_D, 'Rz', -2.99)

# Constraint
dae = False
if (dae == True):
    world.addConstraint(bar3.CS_C, bar4.CS_C, [1,1,0], [0,0,0])
else:
    world.addLoop.FourBar(bar3.CS_C, bar4.CS_C, 1, name='FourBarLinkage')


# Sensors
world.addVisualisation.Line(world, 0.05)
world.addVisualisation.Line(bar2, 0.06)
world.addVisualisation.Line(bar3, 0.065)
world.addVisualisation.Line(bar4, 0.06)

E_bar2 = world.addSensor.Energy('E_bar2', bar2, name='E_bar2')
E_bar3 = world.addSensor.Energy('E_bar3', bar3, name='E_bar3')
E_bar4 = world.addSensor.Energy('E_bar4', bar4, name='E_bar4')
world.addExpression('E', E_bar2 + E_bar3 + E_bar4, name='Energy', category=VarKind.Sensor + VarKind.User_Exp)

#world.addSensor.Position(world.CS_A, world.CS_A, 'r1_A', world.CS_A, name='r1_A')
#world.addSensor.Position(world.CS_A, world.CS_D, 'r1_D', world.CS_A, name='r1_D')

#world.addSensor.Position(world.CS_A, bar2.CS_A, 'r2_A', world.CS_A, name='r2_A')
#world.addSensor.Position(world.CS_A, bar2.CS_B, 'r2_B', world.CS_A, name='r2_B')

#world.addSensor.Position(world.CS_A, bar3.CS_B, 'r3_B', world.CS_A, name='r3_B')
#world.addSensor.Position(world.CS_A, bar3.CS_C, 'r3_C', world.CS_A, name='r3_C')

#world.addSensor.Position(world.CS_A, bar4.CS_D, 'r4_D', world.CS_A, name='r4_D')
#world.addSensor.Position(world.CS_A, bar4.CS_C, 'r4_C', world.CS_A, name='r4_C')

print("System has been assembled")

world.genEquations.OrderN()
world.genCode.Matlab('FourBarLinkage_ODE', '.\Output')
#world.genMatlabAnimation('FourBarLinkage_ODE', '.\Output', axisLimits = (-0.1,0.1, -0.1, 0.1, -0.1,0.1) )
world.show('FourBar')