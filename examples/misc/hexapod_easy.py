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
Created on 06.02.2012

@author: Sebastian Voigt

'''

from PyMbs.Input import *
from scipy import sin, cos

world = MbsSystem([0, 0, -1])
world.addFrame('CS_visual', p=[0, 0, 0], R=rotMat(pi/2, 'x'))

#-------------------------------------------------------------------------------
# parameters
#-------------------------------------------------------------------------------

h = 0.01
R1 = 0.35
R2 = 0.2
R_ZySt = 0.02
R_ZyGeh = 0.04
l_Zy = 0.6
I_Zy = [4.443e-2, 2.068e-8, 4.443e-2, 0, 0, 7.398e-5]

#-------------------------------------------------------------------------------
# tool plane
#-------------------------------------------------------------------------------

AP = world.addBody(mass=1.,
                   inertia=[6.186e-3, 6.016e-10, 6.186e-3, 0, 0, 1.233e-2],
                   name='Arbeitsplattform')
AP.addFrame('CS_visual', p=[0, 0, 0], R=rotMat(pi/2,'x'))

world.addJoint(world, AP, ['Tx', 'Ty', 'Tz', 'Rx', 'Ry', 'Rz'],
               [0, 0, 1, 0, 0, 0], name='AP_6dof')

# body dict to access bodies from within for loop
bodies = {}

#-------------------------------------------------------------------------------
# assemble hexapod
#-------------------------------------------------------------------------------

for i in range(1, 7):

    # angle for iteration
    phi = i*pi/3

    # frame world
    world.addFrame('CS_Anlenkpkt_%i' % i, p=[R1*cos(phi), R1*sin(phi), 0])

    # frame tool plane
    AP.addFrame('CS_Anlenkpkt_%i' % i, p=[R2*cos(phi), R2*sin(phi), 0])

    # cylinder housings
    bodies.update({'ZyGeh_%i' % i : world.addBody(mass=1., cg=[0, 0, 0],
                                                  inertia=I_Zy,
                                                  name='ZyGeh_%i' % i)})
    bodies['ZyGeh_%i' % i].addFrame('CS_visual', p=[0, 0, l_Zy],
                                   R=rotMat(pi/2, 'x'))
    bodies['ZyGeh_%i' % i].addFrame('CS_link', p=[0, 0, l_Zy/2])

    # cylinder rods
    bodies.update({'ZySt_%i' % i : world.addBody(mass=1., cg=[0, 0, 0],
                                                 inertia=I_Zy,
                                                 name='ZySt_%i' % i)})
    bodies['ZySt_%i' % i].addFrame('CS_visual', p=[0, 0, -l_Zy],
                                  R=rotMat(pi/2, 'x'))
    bodies['ZySt_%i' % i].addFrame('CS_link', p=[0, 0, -l_Zy/2])

    # cardan joints
    world.addJoint(eval('world.CS_Anlenkpkt_%i' % i),
                   bodies['ZyGeh_%i' %i].CS_link, ['Rz', 'Ry'], [0, 0],
                   name='BP_ZyGeh_%i' % i)

    # prismatic joints
    world.addJoint(bodies['ZyGeh_%i' %i].CS_link, bodies['ZySt_%i' %i].CS_link,
                   'Tz', [1.0], name='ZyGeh_ZySt_%i' % i)

    # loops
    world.addLoop.Hexapod(eval('AP.CS_Anlenkpkt_%i' % i),
                          bodies['ZySt_%i' %i].CS_link, 'Verbindung_%i' %i)

    # cylinder visualisation
    world.addVisualisation.Cylinder(bodies['ZyGeh_%i' % i].CS_visual, R_ZyGeh,
                                    l_Zy)
    world.addVisualisation.Cylinder(bodies['ZySt_%i' % i].CS_visual, R_ZySt,
                                    l_Zy)

#-------------------------------------------------------------------------------
# visualisation
#-------------------------------------------------------------------------------

world.addVisualisation.Cylinder(world.CS_visual, R1, h)
world.addVisualisation.Cylinder(AP.CS_visual, R2, h)
world.addVisualisation.Frame(AP, 1)
world.addVisualisation.Frame(world, 1)

#-------------------------------------------------------------------------------
# create equations and output
#-------------------------------------------------------------------------------

world.genEquations.Recursive()
#world.genCode('py', 'hexapod_z_kpl','.\HP_Output', debugMode=False)
#world.genCode('m', 'hexapod_z_kpl_mF','.\HP_Output', debugMode=False)

world.show('Hexapod')
