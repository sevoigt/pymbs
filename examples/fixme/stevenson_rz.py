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
Created on 19.07.2009

@author: Christian Schubert
'''

from PyMbs.Input import *
from scipy import arctan, sqrt

# Bar1
world=MbsSystem([0,0,-1], name='Bar1')
A1 = world
B1 = world.addFrame(name='B', p=[-2.5,0,0])
F1 = world.addFrame(name='F', p=[3.0,0,0])
world.addVisualisation.Line(B1, 5.5, name='Body1')

# ZylSta
B2 = world.addBody(name='ZylSta', mass=1, cg=[0.5,0,0], inertia=diag([1,1,1]))
world.addVisualisation.Line(B2, 1, name='Body2')

# ZylGeh
C3 = world.addBody(name='ZylGeh', mass=1, cg=[-0.5,0,0], inertia=diag([1,1,1]))
world.addVisualisation.Line(C3, -1, name='Body3')

# Teil 4
bar4 = world.addBody(name='Bar4', mass=1, inertia=diag([1,1,1]))
D4 = bar4
#world.addVisualisation.Frame(D4, 2)
C4 = bar4.addFrame(name='C', p=[-1.5,-0.5,0])
#world.addVisualisation.Frame(C4, 1)
E4 = bar4.addFrame(name='E', p=[ 1.5,-0.5,0])
#world.addVisualisation.Frame(E4, 3)
C4_Line = C4.addFrame('C_Line', R=rotMat(-arctan(0.5/1.5), 'z'))
world.addVisualisation.Line(C4_Line, sqrt(1.5**2+0.5**2), name='Body4_1')
E4_Line = E4.addFrame('E_Line', R=rotMat(arctan(0.5/1.5), 'z'))
world.addVisualisation.Line(E4_Line, -sqrt(1.5**2+0.5**2), name='Body4_2')
world.addVisualisation.Line(C4, 3, name='Body4_3')

# Teil 5
bar5 = world.addBody(name='Bar5', mass=1, inertia=diag([1,1,1]) )
F5 = bar5
E5 = bar5.addFrame(name='E', p=[2.0,0,0])
world.addVisualisation.Line(bar5, 2.0, name='Body5')

# Teil 6
bar6 = world.addBody(name='Bar6', mass=1, inertia=diag([1,1,1]))
A6 = bar6
D6 = bar6.addFrame(name='D', p=[1.0,0,0])
world.addVisualisation.Line(bar6, 1, name='Body6')

# Insert Joints
jA = world.addJoint(name='Rot_jA', CS1=A1, CS2=A6, dofList='Rz', startVals=-2)
jB = world.addJoint(name='Rot_jB', CS1=B1, CS2=B2, dofList='Rz', startVals=-1)
jC = world.addJoint(name='Rot_jC', CS1=B2, CS2=C3, dofList='Tx', startVals=1.5)
jD = world.addJoint(name='Rot_jD', CS1=D6, CS2=D4, dofList='Rz', startVals=2)
jF = world.addJoint(name='Rot_jF', CS1=F1, CS2=F5, dofList='Rz', startVals=-2.5)

# Close Loops - Fails
dae = False
if (dae == True):
    world.addConstraint('Constr_F1', E4, E5, [1,1,0], [0,0,0])
    world.addConstraint('Constr_F2', C3, C4, [1,1,0], [0,0,0])
else:
    # FourBar
    world.addLoop.FourBar(E4, E5, -1, name='FourBar')
    # FourBarTrans
    world.addLoop.FourBarTrans(C3, C4, 1, name='FourBarTrans')

print("System has been assembled")

world.genEquations.Recursive()
#world.genCode.Modelica('Stevenson', '.\Output')
world.show('Stevenson')