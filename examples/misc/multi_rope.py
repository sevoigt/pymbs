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

@author: Christian Schubert

model description of a rope consisting of n pendulums as a benchmark test
'''

'''

Masse = 4.91 Gramm

Massenmittelpunkt: ( Millimeter )
	X = -0.00
	Y = 0.00
	Z = -46.18

Trägheitsmomente: ( Gramm * QuadratMillimeter )
Bezogen auf den Massenmittelpunkt, ausgerichtet auf das Ausgabekoordinatensystem.
	Lxx = 876.78	Lxy = 0.00	Lxz = 0.62
	Lyx = 0.00	Lyy = 3633.74	Lyz = 0.00
	Lzx = 0.62	Lzy = 0.00	Lzz = 2777.40


Masse = 0.74 Gramm

Massenmittelpunkt: ( Millimeter )
	X = 0.00
	Y = 0.00
	Z = -22.32

Trägheitsmomente: ( Gramm * QuadratMillimeter )
Bezogen auf den Massenmittelpunkt, ausgerichtet auf das Ausgabekoordinatensystem.
	Lxx = 27.92	Lxy = 0.00	Lxz = 0.00
	Lyx = 0.00	Lyy = 110.24	Lyz = 0.00
	Lzx = 0.00	Lzy = 0.00	Lzz = 83.08

2:  1018
3:  2250
4:  4714
5:  9642
6: 19498
7: 39210
8: 78634

'''

d = 0.02    # Rotational Damping
N = 8.0      # Recursoin Deepth

from PyMbs.Input import *
import os
from time import *

global globali
globali = 0

def getName():
    global globali
    globali = globali+1
    return globali-1


def addBody(i,l,prebody,world,scale,m,c_z,I_xx,I_yy,I_zz):

    if (i > N):
        return

    name = getName()

    # Create Body and Frame
    body = world.addBody(name='part%s'%name, mass=m, cg=[0,0,c_z], inertia=diag([I_xx,I_yy,I_zz]))
    body.addFrame(name='left', p=[l*0.5,0,-l*0.7])
    body.addFrame(name='right', p=[-l*0.5,0,-l*0.7])

    # Create Joint
    joint = world.addJoint(name='rot%s_'%name, CS1=prebody, CS2=body, dofList='Ry')

    # Nice Graphics
    world.addVisualisation.File(body, 'RopeBody.stl', scale=scale)

    # Add Damping
    s = world.addSensor.Joint(symbol='state%s_'%name, joint=joint, name='Sensor%s_'%name)
    D = world.addExpression(name='Friction%s_'%name, symbol_str='T%s_'%name, exp=-d*s[1])
    world.addLoad.Joint(joint=joint, symbol=D, name='Load%s_'%name)

    addBody(i+1,l/2.0,body.left,world,scale/2.0,m*0.15071283095723015,c_z*0.4833261152013859,I_xx*0.03184379205730058,I_yy*0.030337888786759647,I_zz*0.029912868150068408)
    addBody(i+1,l/2.0,body.right,world,scale/2.0,m*0.15071283095723015,c_z*0.4833261152013859,I_xx*0.03184379205730058,I_yy*0.030337888786759647,I_zz*0.029912868150068408)


world=MbsSystem([0,0,-1])

# Parameters
l = 100;
m = 4.91
c_z = -46.18
I_xx = 876.78
I_yy = 3633.74
I_zz = 2777.40
d = 0.2    # Rotational Damping

addBody(0,l,world,world,1000,m,c_z,I_xx,I_yy,I_zz)


print('System has been assembled (n:%s)'%int(N))

world.genEquations.OrderN(graphOptimizations=True)
world.genCode.Modelica('MultiRope%s_OrderN'%int(N), './Output')

#world.show('MultiRope%s'%int(N))