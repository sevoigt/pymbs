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
Created on 30.08.2009

@author: Carsten Knoll

'''

from PyMbs.Input import *

world=MbsSystem([0,0,-1])

box = world.addBody('box', mass = 5)
j = world.addJoint('Joint1', world, box, 'Tx', startVals=0.1)
s = world.addSensor.Joint(j, 'dist_x',  'Federweg')
# damped Spring
c=4
d=2
Fm = world.addExpression('MotorForce', 'Fm', -c*s[0]-d*s[1])
world.addJointLoad('jointLoad1', j, Fm)
world.genEquations(explicit=False)

world.genCode('mo', 'jointLoad', '.\Output')
world.genCode('py', 'dampedSpring', '.\Output')