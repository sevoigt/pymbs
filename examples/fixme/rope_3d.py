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

EXPLICIT = 1
RECURSIVE = 2
ORDERN = 3

from PyMbs.Input import *

mode = ORDERN

# Probiere es mit 5, 10 und 20 Elementen
#for n in [5,10,20]:
for n in [2]:
    
    world=MbsSystem([0,0,-1])
    
    # Initialise Lists
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
            joints[i] = world.addJoint(name='rot_%i_'%i, CS1=world, CS2=bodies[i], dofList=['Ry','Rz'], startVals=[1.6,0.15], startVals_d=[0,0.5])
        else:
            joints[i] = world.addJoint(name='rot_%i_'%i, CS1=bodies[i-1].end, CS2=bodies[i], dofList=['Ry','Rz'], startVals=[0.1,0.15], startVals_d=[0,0.5])
        
        # Nice Graphics
        world.addVisualisation.Line(bodies[i], l, name='Graphics%i'%i)
        
        # Add Damping
        s0 = world.addSensor.Joint('state0%i_'%i, joints[i][0], name='Sensor0%i_'%i)
        D0 = world.addExpression(name='Friction0%i_'%i, symbol_str='T0%i_'%i, exp=-d/n**2.0*s0[1])
        world.addLoad.Joint(joint=joints[i][0], symbol=D0, name='Load0%i_'%i)
        s1 = world.addSensor.Joint('state1%i_'%i, joints[i][1], name='Sensor1%i_'%i)
        D1 = world.addExpression(name='Friction1%i_'%i, symbol_str='T1%i_'%i, exp=-d/n**2.0*s1[1])
        world.addLoad.Joint(joint=joints[i][1], symbol=D1, name='Load1%i_'%i)
    
    print("System has been assembled")
    
    world.addVisualisation.Frame(world)
    if (mode == EXPLICIT):
        world.genEquations.Explicit()
        world.genCode.Modelica('Pendel3D_%i_Explicit'%n, 'Output')
    elif (mode == RECURSIVE):
        world.genEquations.Recursive()
        world.genCode.Modelica('Pendel3D_%i_Recursive'%n, 'Output')
    else:
        world.genEquations.OrderN()
        world.genCode.Modelica('Pendel3D_%i_OrderN'%n, 'Output')
        
    print("Finished")
    
#world.show('Rope')

