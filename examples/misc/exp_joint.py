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
n = 3      # 3 Joints

from PyMbs.Input import *

world = MbsSystem([0, 0, -1])
    
# Initialise Lists
bodies = [None]*n
joints = [None]*n
sensors = [None]*n

# Create Bodies and connect them
for i in range(0, n):
    # Parameters
    rho = 500  # 500 kg/m^3 = 0.5 g/cm^3 = 1/2*Wasser
    r = 0.1    # Radius
    l = L/n
    m = rho*pi*r**2*l
    
    # Create Body and Frame
    bodies[i] = world.addBody(name='part%i_'%i, mass=m, cg=[l/2, 0, 0], 
                              inertia=1/12*m*l**2*diag([1, 1, 1]))
    bodies[i].addFrame(name='end', p=[l, 0, 0])
    
    # Create Joint
    if (i==0):
        joints[i] = world.addJoint(name='rot_%i_'%i, CS1=world, 
                                   CS2=bodies[i], dofList='Ry')
    else:
        joints[i] = world.addJoint(name='rot_%i_'%i, CS1=bodies[i-1].end,
                                   CS2=bodies[i], dofList='Ry')
    
    # Nice Graphics
    world.addVisualisation.Line(bodies[i], l, name='Graphics%i'%i)
    
    # Add Damping
    sensors[i] = world.addSensor.Joint('state%i_'%i, joints[i], 
                                       name='Sensor%i_'%i)

# Add up the coordinates of all joints
tmp = 0
for i in range(0, n-1):
    s = sensors[i]
    tmp += s[0]
    
# Parameterise last joint
q = world.addExpression(name='qEnd', symbol_str='qEnd', exp=pi/2-tmp)
world.addLoop.ExpJoint(joints[n-1], q, name='LastJoint')
    
print("System has been assembled")
    
world.genEquations.Recursive(kinematicsOnly=False)
world.show('ParameterisedJoint')
