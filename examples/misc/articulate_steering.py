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
Created on 23.06.2010

@author: Jens Frenkel

model description of a vehicle with articulated steering
'''

from pymbs.input import *
import pymbs.common.graphreps as gr

world=MbsSystem([0,0,-1])

l=world.addParam('l', 1.0)

m_front=world.addParam('m_front', 100.0)
I_front=world.addParam('Inertia_front', [0.85,0.46,0.85])

m_back=world.addParam('m_back', 100.0)
I_back=world.addParam('Inertia_back', [0.85,0.46,0.85])

m_rad=world.addParam('m_rad', 12.0)
I_rad=world.addParam('Inertia_rad', [0.1,0.2,0.1])

front_body=world.addBody(mass=m_front, cg=[l,0,0.0], inertia=diag(I_front))
front_body.addFrame('CS_Steering', [0,0,0])
front_body.addFrame('CS_Rad_l', [1.5*l,l,0])
front_body.addFrame('CS_Rad_r', [1.5*l,-l,0])
front_body.addFrame('NullSystem', [l,0,0])
world.addVisualisation.Frame(front_body.NullSystem)
back_body=world.addBody(mass=m_back, cg=[-l,0,0.0], inertia=diag(I_back))
back_body.addFrame('CS_Steering', [0,0,0])
back_body.addFrame('CS_Rad_l', [-1.5*l,l,0])
back_body.addFrame('CS_Rad_r', [-1.5*l,-l,0])
back_body.addFrame('NullSystem', [-l,0,0])
world.addVisualisation.Frame(back_body.NullSystem)


world.addJoint(world, front_body.CS_Steering, ['Tx', 'Ty', 'Tz', 'Rx', 'Ry', 'Rz'],name='DOF6')

j1 = world.addJoint(front_body.CS_Steering, back_body.CS_Steering, 'Rz')
lenkung = world.addInput('Lenkung')
world.addLoop.ExpJoint(j1,lenkung)


flanges = [front_body.CS_Rad_l,front_body.CS_Rad_r,back_body.CS_Rad_l,back_body.CS_Rad_r]
for i in range(0,4):
    rad=world.addBody(mass=m_rad, cg=[0,0,0.0], inertia=diag(I_rad))
    rad.addFrame('CS_Flansch', [0,0,0])
    world.addVisualisation.Frame(rad)
    world.addJoint(flanges[i], rad.CS_Flansch, 'Ry')

    # Radantrieb
    M = world.addInput('M_Rad_%s'%i)
    M_vec = world.addExpression('M_vec_%s'%i,  [0,M,0])
    world.addLoad.CmpTorque(M_vec, rad.CS_Flansch, flanges[i])

    # Reifenflansch
    world.addTyreSensorsLoads('TS_Rad_%s'%i, flanges[i], rad.CS_Flansch)

world.addSensor.CenterofMass('totalMass')

print("System has been assembled")

world.genEquations.Recursive()
world.genCode.Modelica('KnickLenker', './Output',inputsAsInputs=False)

world.show('Knicklenker')

print("Done")
