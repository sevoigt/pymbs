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


from PyMbs.Input import *

world=MbsSystem([0,0,-1])
world.addFrame('CS_0')

m=world.addParam('mass', 'm', 1)

mass=world.addBody('Mass', m, [0,0,0], zeros((3,3)))
mass.addFrame('CS_0')

world.addJoint('J_Planar', world.CS_0, mass.CS_0, 'Tz')

world.addSensor('Pos', 'Position', world.CS_0, mass.CS_0, 'p')
world.addSensor('Vel', 'Velocity', world.CS_0, mass.CS_0, 'v')
world.addSensor('Acc', 'Acceleration', world.CS_0, mass.CS_0, 'a')

print("System has been assembled")

genEquations(world, explicit=False)
genCode('m', 'TransAcc_rec', '.\Output', debugMode=True)
