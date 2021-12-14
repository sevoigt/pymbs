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
Created on 14.08.2012

@author: Jens Frenkel
'''
from PyMbs.Input import *



world=MbsSystem([0,0,-1], name="Bar0")

# Parameter
l0=world.addParam( 'l0', 2.5,'Length 0')
h0=world.addParam( 'h0', -0.5,'Height 0')

m1=world.addParam('m1', 1.0,'Mass 1')
l1=world.addParam( 'l1', 1.0,'Length 1')
I1=world.addParam( 'I1', m1/11*l1**2,'Inertia 1')

m2=world.addParam( 'm2', 0.025,'Mass 2')
l2=world.addParam( 'l2', 1.0,'Length 2')
I2=world.addParam( 'I2', m2/13*l2**2,'Inertia 2')

m3=world.addParam( 'm3', 0.025,'Mass 3')
l3=world.addParam( 'l3', 1.0,'Length 3')
I3=world.addParam( 'I3', m3/13*l3**2,'Inertia 3')

# Create Bodies with Coordinate Systems
# Bar1 = Inertial System
bar0_A = world.addFrame(name='CS_A')
bar0_D = world.addFrame(name='CS_D', p=[l0,h0,0])

bar1 = world.addBody(name='Bar1', mass=m1, cg=[l1/2,0,0], inertia=diag([0,I1,I1]))
bar1.addFrame(name='CS_A')
bar1.addFrame(name='CS_B', p=[l1,0,0])
bar1.addFrame(name='CS_Graphic', p=[l1/2,0,0])

bar2 = world.addBody(name='Bar2', mass=m2, cg=[l2/2,0,0], inertia=diag([0,I2,I2]))
bar2.addFrame(name='CS_B')
bar2.addFrame(name='CS_C', p=[l2,0,0])
bar2.addFrame(name='CS_Graphic', p=[l2/2,0,0])

bar3 = world.addBody(name='Bar3', mass=m3, cg=[l3/2,0,0], inertia=diag([0,I3,I3]))
bar3.addFrame(name='CS_C')
bar3.addFrame(name='CS_D', p=[l3,0,0])
bar3.addFrame(name='CS_Graphic', p=[l3/2,0,0])

# Insert Joints
jA = world.addJoint( world.CS_A, bar1.CS_A, 'Tx', name='jA_Tx')
jB = world.addJoint( bar1.CS_B, bar2.CS_B, 'Rz',name='jB_Rz')
jD = world.addJoint( world.CS_D, bar3.CS_D, 'Rz',name='jD_Rz')

# Loop
world.addLoop.Steering(bar2.CS_C, bar3.CS_C, name='SteeringLoop')


# Sensors
'''
world.addSensor.Position(world.CS_A, world.CS_A, 'r1_A', world.CS_A, name='r1_A')
world.addSensor.Position(world.CS_A, world.CS_C, 'r1_C', world.CS_A, name='r1_C')

world.addSensor.Position(world.CS_A, bar2.CS_A, 'r2_A', world.CS_A, name='r2_A')
world.addSensor.Position(world.CS_A, bar2.CS_B, 'r2_B', world.CS_A, name='r2_B')

world.addSensor.Position(world.CS_A, bar3a.CS_B, 'r3a_B', world.CS_A, name='r3a_B')
world.addSensor.Position(world.CS_A, bar3b.CS_C, 'r3b_C', world.CS_A, name='r3b_C')
'''

print("System has been assembled")


# Visualisation
world.addVisualisation.Box(bar1.CS_Graphic, 1.0, 0.005, 0.005, name='bar1')
world.addVisualisation.Box(bar2.CS_Graphic, 1.0, 0.005, 0.005, name='bar2')
world.addVisualisation.Box(bar3.CS_Graphic, 1.0, 0.005, 0.005, name='bar3')

world.addVisualisation.Frame(bar0_A,1,name='Ursprung')
world.addVisualisation.Frame(bar0_D,0.5,name='D')

world.genEquations.Recursive()
world.show('SteeringLoop')