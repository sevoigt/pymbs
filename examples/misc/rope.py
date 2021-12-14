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

L = 3      # Total Length
d = 0.2    # Rotational Damping

from PyMbs.Input import *
import os
from time import *

ropeSize = list()
times = list()

EXPLICIT = 1
RECURSIVE = 2
ORDERN = 3
METHOD = ORDERN

if (METHOD==3):
    #ropeSize.append((4,377))
    #ropeSize.append((25,25))
    ropeSize.append((20,1))
    #ropeSize.append((100,100))
    #ropeSize.append((200,200))
    #ropeSize.append((400,400))
    #ropeSize.append((800,800))
    #ropeSize.append((1000,1000))
    #ropeSize.append((1600,1600))
    #ropeSize.append((3200,3200))
elif (METHOD==2):
    #ropeSize.append((2,377))
    ropeSize.append((4,377))
    ropeSize.append((7,707))
    ropeSize.append((12,1397))
    ropeSize.append((20,2865))
    ropeSize.append((31,5615))
    ropeSize.append((47,11127))
    ropeSize.append((70,22190))
    #ropeSize.append((103,44531))
    #ropeSize.append((150,89510))
    #ropeSize.append((216,178775))
elif (METHOD==1):
    ropeSize.append((2,2))

for j in range(0,len(ropeSize)): #len(ropeSize)):

    n = ropeSize[j][0]


    world=MbsSystem([0,0,-1])

    # Initialize Lists
    bodies = [None]*n
    joints = [None]*n

    # Create Bodies and connect them
    for i in range(0,n):
        # Parameters
        rho = 500;  # 500 kg/m^3 = 0.5 g/cm^3 = 1/2*Wasser
        r = 0.1;    # Radius
        l = L/n;
        m = rho*pi*r**2*l

        # Create Body and Frame
        bodies[i] = world.addBody(name='part%i_'%i, mass=m, cg=[l/2,0,0], inertia=1/12*m*l**2*diag([1,1,1]))
        bodies[i].addFrame(name='end', p=[l,0,0])

        # Create Joint
        if (i==0):
            joints[i] = world.addJoint(name='rot_%i_'%i, CS1=world, CS2=bodies[i], dofList='Ry')
        else:
            joints[i] = world.addJoint(name='rot_%i_'%i, CS1=bodies[i-1].end, CS2=bodies[i], dofList='Ry')

        # Nice Graphics
        world.addVisualisation.Line(bodies[i], l, name='Graphics%i'%i)

        # Add Damping
        s = world.addSensor.Joint(symbol='state%i_'%i, joint=joints[i], name='Sensor%i_'%i)
        D = world.addExpression(name='Friction%i_'%i, symbol_str='T%i_'%i, exp=-d*s[1])
        world.addLoad.Joint(joint=joints[i], symbol=D, name='Load%i_'%i)


    print("System has been assembled (n:%s)"%n)

    t1 = clock()
    #import cProfile
    #cProfile.run('b, t = world.genEquations.Recursive(graphOptimizations=True)', 'stats')
#    b, t = world.genEquations.Recursive(graphOptimizations=True)
    if (METHOD==3):
        b, t = world.genEquations.OrderN(graphOptimizations=True)
    elif (METHOD==2):
        b, t = world.genEquations.Recursive(graphOptimizations=True)
    elif (METHOD==1):
        b, t = world.genEquations.Explicit(graphOptimizations=True)

    t2 = clock()
    dt = t2-t1
    print("Write Code %s,%s"%(dt,t))
    if (METHOD==3):
        #dt1 = world.genCode.Modelica('Rope_%s_OrderN'%ropeSize[j][0], './Output')
		dt1 = 0
    elif (METHOD==2):
        #dt1 = world.genCode.Modelica('Rope_%s_Recursive'%ropeSize[j][0], './Output')
		#world.genCode.Python('Rope_%s_Recursive'%ropeSize[j][0], './Output')
		dt1 = 0
    elif (METHOD==1):
        dt1 = world.genCode.Modelica('Rope_%s_Explicit'%ropeSize[j][0], './Output')
    #dt1 = 0
    #world.graph.printStats()
    times.append((t,dt,dt1))
    print("Done in %s,%s,%s"%(t,dt,dt1))

print("Results")
for j in range(0,len(ropeSize)):
    print("%s;%s;%s;%s;%s"%(ropeSize[j][0],ropeSize[j][1],times[j][0],times[j][1],times[j][2]))

if (METHOD == ORDERN):
    world.show('Rope')
