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

@author: knoll, modified by jochmann

model-description for a simple pendulum
adapted from matlab-Example
'''

from PyMbs.Input import *

world=MbsSystem([1,0,0])
##world.addFrame('FR_visual',R=rotMat(pi/2,'z'))
##world.addVisualisation.Cylinder(world.FR_visual, radius=1, height=0.01)

m1=world.addParam('m1', 1, name='mass1', )
l1=world.addParam('l1', 500, name='lengthOfRod1')
I1=world.addParam('I1', m1*l1**2/12, name='inertia1')
m2=world.addParam('m2', 1, name='mass2')
l2=world.addParam('l2', 1, name='lengthOfRod2')
I2=world.addParam('I2', m2*l2**2/12, name='inertia2')

p1 = world.addFrame(name='CS_p1')

pend1=world.addBody(m1, [l1/2,0,0], diag([0,I1,0]),name='Pendulum1')



##pend1=world.addBody(m1, [0,0,0], diag([0,0,I1]),name='Pendulum1')

##pend1.addFrame(p=[0, l1, 0], R=eye(3),name='CS_joint')
##pend1.addFrame(p=[0, 0, 0], R=eye(3),name='CS_tip')

pend2=world.addFlexibleBody('BeispielAlleEV.SID_FEM',name='Pendulum2')
pend1.addFrame(p=[l1, 0, 0], R=eye(3),name='CS_joint')
Mitte = pend1.addFrame(p=[l1/2, 0, 0], R=rotMat(pi/2,'z'),name='middle')
pend1.addFrame(p=[0, 0, 0], R=eye(3),name='CS_tip')

world.addVisualisation.Cylinder(Mitte, radius = 10, height = 500)
world.addVisualisation.flexible_Body(cs = pend2.node(1), flex_Body = pend2, name = 'flex')


world.addJoint(name='RevPendulum1', CS1=world.CS_p1, CS2=pend1.CS_tip, dofList='Ry')
setup = 1
if (setup == 1):
    world.addJoint(name='RevPendulum2', CS1=pend1.CS_joint, CS2=pend2.node(1), dofList='Ry')
elif (setup == 2):
    world.addJoint(name='RevPendulum2', CS1=pend1.node(3), CS2=pend2.CS_joint, dofList='Rx')
elif (setup == 3):
    world.addJoint(name='RevPendulum2', CS1=pend1.CS_tip, CS2=pend2.CS_joint, dofList=['Rx', 'Rz'])
else:
    raise ValueError('Invalid value for setup!')

print("System has been assembled")


##world.addVisualisation.Cylinder(visualisierung_pendel, radius = 0.01, height = 3)
##world.addVisualisation.Cylinder(visualisierung_pendel, radius = 0.01, height = 3)

explicit = False
if (explicit==True):
    world.genEquations.Recursive()
    world.genCode.Matlab('DoublePendulum_exp', '.\Output', debugMode=True)
else:
    world.genEquations.OrderN(kinematicsOnly = False)
    world.genCode.Matlab('DoublePendulum_rec', '.\Output', debugMode=True)

world.show('Doppelpendel')

