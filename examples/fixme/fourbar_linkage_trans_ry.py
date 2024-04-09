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

world=MbsSystem([0,0,-1], name="Bar1")

# Parameter
l1=world.addParam('Length 1', 'l1', 0.05)

m2=world.addParam('Mass 2', 'm2', 0.6)
l2=world.addParam('Length 2', 'l2', 0.06)
I2=world.addParam('Inertia 2', 'I2', m2/12*l2**2)

m3=world.addParam('Mass 3', 'm3', 0.65)
l3=world.addParam('Length 3', 'l3', 0.065)
I3=world.addParam('Inertia 3', 'I3', m3/12*l3**2)

m4a=world.addParam('Mass 4a', 'm4a', 0.6)
l4a=world.addParam('Length 4a', 'l4a', 0.02)
I4a=world.addParam('Inertia 4a', 'I4a', m4a/12*l4a**2)

m4b=world.addParam('Mass 4b', 'm4b', 0.6)
l4b=world.addParam('Length 4b', 'l4b', 0.02)
I4b=world.addParam('Inertia 4b', 'I4b', m4b/12*l4b**2)

# Create Bodies with Coordinate Systems
# Bar1 = Inertial System
bar1_A = world.addFrame(name='CS_A')
bar1_D = world.addFrame(name='CS_D', p=[l1,0,0])

bar2 = world.addBody(name='Bar2', mass=m2, cg=[l2/2,0,0], inertia=diag([0,0, I2]))
bar2.addFrame(name='CS_A')
bar2.addFrame(name='CS_B', p=[ l2,0,0])

bar3 = world.addBody(name='Bar3', mass=m3, cg=[l3/2,0,0], inertia=diag([0,0,I3]))
bar3.addFrame(name='CS_B')
bar3.addFrame(name='CS_C', p=[ l3,0,0])

bar4a = world.addBody(name='Bar4a', mass=m4a, cg=[l4a/2,0,0], inertia=diag([0,0,I4a]))
bar4a.addFrame(name='CS_C')

bar4b = world.addBody(name='Bar4b', mass=m4b, cg=[l4b/2,0,0], inertia=diag([0,0,I4b]))
bar4b.addFrame(name='CS_D')

# Insert Joints
jA = world.addJoint('jA', world.CS_A, bar2.CS_A, 'Ry', 0.01)
jB = world.addJoint('jB', bar2.CS_B, bar3.CS_B, 'Ry')
jD = world.addJoint('jD', world.CS_D, bar4b.CS_D, 'Ry')
jDC = world.addJoint('jDC', bar4b.CS_D, bar4a.CS_C, 'Tx')

# Constraint
dae = False
if (dae == True):
    world.addConstraint('Constr_FB', bar3.CS_C, bar4a.CS_C, [1,1,0], [0,0,0])
else:
    world.addLoop.FourBarTrans(bar4a.CS_C, bar3.CS_C, name='FourBarTrans')


# Sensors
world.addSensor.Position(world.CS_A, world.CS_A, 'r1_A', world.CS_A, name='r1_A')
world.addSensor.Position(world.CS_A, world.CS_D, 'r1_D', world.CS_A, name='r1_D')

world.addSensor.Position(world.CS_A, bar2.CS_A, 'r2_A', world.CS_A, name='r2_A')
world.addSensor.Position(world.CS_A, bar2.CS_B, 'r2_B', world.CS_A, name='r2_B')

world.addSensor.Position(world.CS_A, bar3.CS_B, 'r3_B', world.CS_A, name='r3_B')
world.addSensor.Position(world.CS_A, bar3.CS_C, 'r3_C', world.CS_A, name='r3_C')

world.addSensor.Position(world.CS_A, bar4b.CS_D, 'r4b_D', world.CS_A, name='r4b_D')
world.addSensor.Position(world.CS_A, bar4a.CS_C, 'r4a_C', world.CS_A, name='r4a_C')

world.addVisualisation.Line(bar2, 0.06) 
world.addVisualisation.Line(bar3, 0.065) 
world.addVisualisation.Line(bar4b, 0.1) 
world.addVisualisation.Box(bar4a, 0.02, 0.005, 0.005)


print("System has been assembled")

world.genEquations(explicit=False)
world.genCode('m', 'FourBarLinkageTrans', '.\Output')
world.show('fourFourBarLinkage_Mechanism')

