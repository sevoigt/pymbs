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
Created on 12.08.2009

@author: Christian
'''

from PyMbs.Input import *

world = MbsSystem([0, 0, -1], 'Aufnahme')

acgx = world.addParam('acgx', 1, name='Ausleger_cg_x')
z1x = world.addParam('z1x', -1.550, name='Zylinder1_x')
z1z = world.addParam('z1z', -0.180, name='Zylinder1_z')

ax = world.addParam('ax', 0.244, name='Ausleger_x')
az = world.addParam('az', 0.338, name='Ausleger_z')

world.addFrame('KS_Ausleger', [ax, 0, az])
world.addFrame('KS_Zylinder1', [0, 0, 0])

ausleger = world.addBody(mass=500, cg=[acgx, 0, 0], name='Ausleger')
ausleger.addFrame('KS_Aufnahme')
ausleger.addFrame('KS_Zylinder1', [z1x, 0, z1z])

world.addJoint(world.KS_Ausleger, ausleger.KS_Aufnahme, 'Ry', -0.6, 0,
               name='Rot_Ausleger')

world.addHydCylSensorsLoads('Zylinder1', world.KS_Zylinder1,
                            ausleger.KS_Zylinder1)

world.addSensor.Position('r', world.KS_Zylinder1, ausleger.KS_Zylinder1, world,
                         name='Pos')
world.addSensor.Velocity('v', world.KS_Zylinder1, ausleger.KS_Zylinder1, world,
                         name='Vel')

world.genEquations.Explicit()
world.genCode.Matlab('BaggerArm_exp', './Output', debugMode=True)

# world.genEquations.Recursive()
# world.genCode.Matlab('BaggerArm_rec', './Output', debugMode=True)
# 
# world.genEquations.Recursive()
# world.genCode.Modelica('BaggerArm', './Output')

world.show()
