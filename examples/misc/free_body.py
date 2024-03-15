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
Created on 05.07.2009

@author: Christian Schubert

'''
from pymbs.input import *

world=MbsSystem([0,0,-1])

m=world.addParam('m', 1,'mass')
I=world.addParam('Inertia', [1,1,1],'inertia')
a=world.addParam('a', 1,'length')

k=world.addParam('c', 100,'stiffness')
d=world.addParam('d', 10,'damping_coeff')

world.addFrame('CS_world', zeros((3,)), eye(3))

body=world.addBody( m, [0,0,a/2], diag(I),'Body')
body.addFrame('CS_cg', [0,0,a/2])
body.addFrame('CS_Top_Front_Left', [a/2,a/2,a])
body.addFrame('CS_Top_Front_Right', [a/2,-a/2,a])
body.addFrame('CS_Top_Rear_Left', [-a/2,a/2,a])
body.addFrame('CS_Top_Rear_Right', [-a/2,-a/2,a])
body.addFrame('CS_Bottum_Front_Left', [a/2,a/2,0])
body.addFrame('CS_Bottum_Front_Right', [a/2,-a/2,0])
body.addFrame('CS_Bottum_Rear_Left', [-a/2,a/2,0])
body.addFrame('CS_Bottum_Rear_Right', [-a/2,-a/2,0])


world.addJoint(world.CS_world, body.CS_cg, ['Tx', 'Ty', 'Tz', 'Rx', 'Ry', 'Rz'],name='DOF6')

p_B = world.addSensor.Position('p_B',world.CS_world, body.CS_cg)
v_B = world.addSensor.Velocity('v_B',world.CS_world, body.CS_cg)
a_B = world.addSensor.Acceleration('a_B',world.CS_world , body.CS_cg)
p_OFL = world.addSensor.Position('p_OFL',world.CS_world, body.CS_Top_Front_Left)
p_OFR = world.addSensor.Position('p_OFR',world.CS_world, body.CS_Top_Front_Right)
p_ORL = world.addSensor.Position('p_ORL',world.CS_world, body.CS_Top_Rear_Left)
p_ORR = world.addSensor.Position('p_ORR',world.CS_world, body.CS_Top_Rear_Right)
v_OFL = world.addSensor.Velocity('v_OFL',world.CS_world, body.CS_Top_Front_Left)
v_OFR = world.addSensor.Velocity('v_OFR',world.CS_world, body.CS_Top_Front_Right)
v_ORL = world.addSensor.Velocity('v_ORL',world.CS_world, body.CS_Top_Rear_Left)
v_ORR = world.addSensor.Velocity('v_ORR',world.CS_world, body.CS_Top_Rear_Right)
a_OFL = world.addSensor.Acceleration('a_OFL',world.CS_world, body.CS_Top_Front_Left)
dw_OFL = world.addSensor.AngularAcceleration('dw_OFL',world.CS_world, body.CS_Top_Front_Left)
p_UFL = world.addSensor.Position('p_UFL',world.CS_world, body.CS_Bottum_Front_Left)
p_UFR = world.addSensor.Position('p_UFR',world.CS_world, body.CS_Bottum_Front_Right)
p_URL = world.addSensor.Position('p_URL',world.CS_world, body.CS_Bottum_Rear_Left)
p_URR = world.addSensor.Position('p_URR',world.CS_world, body.CS_Bottum_Rear_Right)
v_UFL = world.addSensor.Velocity('v_UFL',world.CS_world, body.CS_Bottum_Front_Left)
v_UFR = world.addSensor.Velocity('v_UFR',world.CS_world, body.CS_Bottum_Front_Right)
v_URL = world.addSensor.Velocity('v_URL',world.CS_world, body.CS_Bottum_Rear_Left)
v_URR = world.addSensor.Velocity('v_URR',world.CS_world, body.CS_Bottum_Rear_Right)

# two possibilities: [0, 0, k*z] or Ez*k*z (Ex, Ey, Ez are imported from Input/__init__.py)
#F_FL = world.addExpression('SpringForce_Front_Left', 'F_FL', [0, 0, k*p_FL[2]+d*v_FL[2]])
#F_FR = world.addExpression('SpringForce_Front_Right', 'F_FR', Ez*(k*p_FR[2]+d*v_FR[2]))
#F_RL = world.addExpression('SpringForce_Rear_Left', 'F_RL', Ez*(k*p_RL[2]+d*v_RL[2]))
#F_RR = world.addExpression('SpringForce_Rear_Right', 'F_RR', Ez*(k*p_RR[2]+d*v_RR[2]))

F_OFL = world.addInput('F_OFL', (3,));
F_OFR = world.addInput('F_OFR', (3,));
F_ORL = world.addInput('F_ORL', (3,));
F_ORR = world.addInput('F_ORR', (3,));
F_UFL = world.addInput('F_UFL', (3,));
F_UFR = world.addInput('F_UFR', (3,));
F_URL = world.addInput('F_URL', (3,));
F_URR = world.addInput('F_URR', (3,));

world.addLoad.CmpForce(F_OFL,world.CS_world, body.CS_Top_Front_Left  )
world.addLoad.CmpForce(F_OFR,world.CS_world, body.CS_Top_Front_Right )
world.addLoad.CmpForce(F_ORL,world.CS_world, body.CS_Top_Rear_Left  )
world.addLoad.CmpForce(F_ORR,world.CS_world, body.CS_Top_Rear_Right  )
world.addLoad.CmpForce(F_UFL,world.CS_world, body.CS_Bottum_Front_Left  )
world.addLoad.CmpForce(F_UFR,world.CS_world, body.CS_Bottum_Front_Right )
world.addLoad.CmpForce(F_URL,world.CS_world, body.CS_Bottum_Rear_Left  )
world.addLoad.CmpForce(F_URR,world.CS_world, body.CS_Bottum_Rear_Right  )

print("System has been assembled")

world.genEquations.Recursive()
world.genCode.Modelica('FreeBody', '.\Output')

print("Done")