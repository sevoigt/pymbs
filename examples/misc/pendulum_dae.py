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

world = MbsSystem([0,0,-1])

m = world.addParam('m', 1, name='mass')
l = world.addParam('l', 1, name='length')

pend = world.addBody(m, [l/2,0,0], diag([0,0,m*l**2/12]),name='Pendulum')

world.addJoint(world, pend, ['Tx', 'Tz', 'Ry'], [0, 0, 1],name='J_Planar')

world.addConstraint(world, pend, [1,0,1], [0,0,0],name='Constr_Tip')

world.addVisualisation.Frame(world, 0.2)
world.addVisualisation.Line(pend, 1)

print("System has been assembled")

world.genEquations.Recursive(kinematicsOnly=False,graphOptimizations=False, diff=0)
world.genCode.Modelica('PendulumDAE_3', './Output')

#world.genEquations.Recursive(kinematicsOnly=False,graphOptimizations=True, diff=2)
#world.genCode.Modelica('PendulumDAE_1', './Output')

print("Done")
