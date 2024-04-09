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
Created on 11.02.20013

@author: jochmann

model-description for a flexibkle body zero dof

'''

from PyMbs.Input import *

world=MbsSystem([1,0,0])


world.addFrame('base_gedreht',R=rotMat(pi/2,'z'))

flex_bod=world.addFlexibleBody('funfKnotenmod.SID_FEM',name='pendulum_flex')
flex_bod2=world.addFlexibleBody('funfKnotenmod.SID_FEM',name='pendulum_flex2')

world.addVisualisation.flexible_Body(cs = flex_bod.node(1), flex_Body = flex_bod, name = 'flexi')
world.addVisualisation.flexible_Body(cs = flex_bod2.node(1), flex_Body = flex_bod2, name = 'flexi2')
world.addVisualisation.Cylinder(world.base_gedreht, radius=5, height=0.1)

world.addJoint(name='revolute', CS1=world, CS2=flex_bod.node(1), dofList= [])
world.addJoint(name='revolute2', CS1=flex_bod.node(5), CS2=flex_bod2.node(1), dofList= 'Rz')
#world.addLoad.CmpForce([0,50,0],CS1=world,CS2=pend2.node(2),name='kraft2')
#world.addLoad.Joint(M,a,name = 'drehmom')

print("System has been assembled")


##world.addVisualisation.Cylinder(visualisierung_pendel, radius = 0.01, height = 3)
##world.addVisualisation.Cylinder(visualisierung_pendel, radius = 0.01, height = 3)

explicit = False
if (explicit==True):
    world.genEquations.Recursive()
    world.genCode.Matlab('flex_doppelrotation', '.\Output', debugMode=True)
else:
    world.genEquations.OrderN(kinematicsOnly = False)
    world.genCode.Matlab('flex_doppelrotation', '.\Output', debugMode=False)
    world.genCode.Python('flex_doppelrotation', '.\Output', debugMode=False)


# new: configuration for gui
options = {'qFlexScaling':20}

world.show('flex_rotation', options=options)