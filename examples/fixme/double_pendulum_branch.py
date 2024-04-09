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

@author: knoll

model-description for a simple pendulum
adapted from matlab-Example
'''

from PyMbs.Input import *

world=MbsSystem([0,-1,0])

m1=world.addParam('mass 1', 'm1', 1)
l1=world.addParam('lengthOfRod 1', 'l1', 1)
I1=world.addParam('inertia 1', 'I1', m1*l1**2/12)
a =world.addParam('distance a', 'a', 0.5)
m2=world.addParam('mass 2', 'm2', 1)
l2=world.addParam('lengthOfRod 2', 'l2', 1)
I2=world.addParam('inertia 2', 'I2', m2*l2**2/12)
m3=world.addParam('mass 3', 'm3', 1)
l3=world.addParam('lengthOfRod 3', 'l3', 1)
I3=world.addParam('inertia 3', 'I3', m3*l3**2/12)

world.addFrame('CS_p1', zeros((3,)), eye(3))

pend1=world.addBody('Pendulum1', m1, [0,0,0], diag([0,0,I1]))
pend1.addFrame('CS_joint', [0, l1, 0], eye(3))
pend1.addFrame('CS_tip', [0, 0, 0], eye(3))
pend1.addFrame('CS_p2', [a, 0, 0], eye(3))
pend1.addFrame('CS_p3', [-a, 0, 0], eye(3))

pend2=world.addBody('Pendulum2', m2, [0,0,0], diag([0,0,I2]))
pend2.addFrame('CS_joint', [0, l2, 0], eye(3))
pend2.addFrame('CS_tip', [0, 0, 0], eye(3))

pend3=world.addBody('Pendulum3', m3, [0,0,0], diag([0,0,I3]))
pend3.addFrame('CS_joint', [0, l3, 0], eye(3))
pend3.addFrame('CS_tip', [0, 0, 0], eye(3))


world.addJoint('RevPendulum1', world.CS_p1, pend1.CS_joint, 'Rz')
world.addJoint('RevPendulum2', pend1.CS_p2, pend2.CS_joint, 'Rz')
world.addJoint('RevPendulum3', pend1.CS_p3, pend3.CS_joint, 'Rz')

print("System has been assembled")


world.genEquations(explicit=False)
world.genCode('m', 'DoublePendulumBranch_rec', '.\Output', debugMode=True)

world.genEquations(explicit=True)
world.genCode('m', 'DoublePendulumBranch_exp', '.\Output', debugMode=True)

