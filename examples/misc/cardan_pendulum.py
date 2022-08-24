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
Created on 31.07.2009

@author: Christian Schubert

Cardan Pendulum
'''

from pymbs.input import *

world=MbsSystem([0,0,-1])

m=world.addParam('m', 1, name='mass')
l=world.addParam('l', 1, name='lengthOfRod')
Jx=world.addParam('Jx', 1, name='Jx')
Jy=world.addParam('Jy', 1, name='Jy')

world.addFrame('CS_0')

pend=world.addBody(m, zeros((3,)), diag([Jx,Jy,0]), name='Pendulum')
pend.addFrame('CS_joint', [0,0,l])

world.addJoint(world.CS_0, pend.CS_joint, ['Rx', 'Ry'], name='RevPendulum')

print("System has been assembled")

world.genEquations.Recursive()
world.genCode.Matlab('CardanPendulum_rec', '.\Output', debugMode=True)

# world.genEquations.Explicit()
# world.genCode.Matlab('CardanPendulum_exp', '.\Output', debugMode=True)
