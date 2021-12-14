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
Created on 18.10.2011

@author: Jens Frenkel

'''
from PyMbs.Input import *

world = MbsSystem([0, 0, -1])

m = world.addParam('m', 12.0,'mass')
I = world.addParam('Inertia', [0.85, 0.46, 0.85],'inertia')

body = world.addBody(mass=m/100.0, cg=[0, 0, 0], inertia=diag(I)*0.001)
body.addFrame('CS_Flange', [0, 0, 0])

wheel = world.addBody(mass=m, cg=[0, 0, -0.07], inertia=diag(I),
                      name='WheelBody')
wheel.addFrame('CS_Flange', [0, 0, 0])

world.addJoint(world, body.CS_Flange, ['Tx', 'Ty', 'Tz'], name='DOF3')

j1 = world.addJoint(body.CS_Flange, wheel.CS_Flange, ['Rx', 'Ry', 'Rz'], 
                    name='RDOF3')

camber = world.addInput('Camber')
world.addLoop.ExpJoint(j1[0], camber)

steering = world.addInput('Steering')
world.addLoop.ExpJoint(j1[2], steering)

# Driving torque
M = world.addInput('M_Wheel')
M_vec = world.addExpression('M_vec',  [0, M, 0])
world.addLoad.CmpTorque(M_vec, wheel.CS_Flange, body.CS_Flange)

# Wheel Load Sensor
world.addTyreSensorsLoads('TS_Wheel', body.CS_Flange, wheel.CS_Flange)

print("System has been assembled")

world.genEquations.Recursive()
world.genCode.Modelica('WheelTestBench', './Output')

print("Done")
