# -*- coding: iso-8859-1 -*-
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

Created on 27.03.2012

@author: Sebastian Voigt
'''




from PyMbs.Input import *


from math import pi, sin, cos



# hexapod longitudinal axis points in x-direction

world = MbsSystem([-1,0,0])




#-------------------------------------------------------------------------------
#                              base platform
#-------------------------------------------------------------------------------

# frames for cylinders
r1 = 1
world.addFrame('FR_cyl1', p=[0, r1*cos(  1/15*pi), r1*sin(  1/15*pi)])
world.addFrame('FR_cyl2', p=[0, r1*cos(  9/15*pi), r1*sin(  9/15*pi)])
world.addFrame('FR_cyl3', p=[0, r1*cos( 11/15*pi), r1*sin( 11/15*pi)])
world.addFrame('FR_cyl4', p=[0, r1*cos(-11/15*pi), r1*sin(-11/15*pi)])
world.addFrame('FR_cyl5', p=[0, r1*cos( -9/15*pi), r1*sin( -9/15*pi)])
world.addFrame('FR_cyl6', p=[0, r1*cos( -1/15*pi), r1*sin( -1/15*pi)])


# visualisation
world.addFrame('FR_visual',R=rotMat(pi/2,'z'))
world.addVisualisation.Cylinder(world.FR_visual, radius=r1, height=0.01)

world.addVisualisation.Sphere(world.FR_cyl1, radius=0.12)
world.addVisualisation.Sphere(world.FR_cyl2, radius=0.12)
world.addVisualisation.Sphere(world.FR_cyl3, radius=0.12)
world.addVisualisation.Sphere(world.FR_cyl4, radius=0.12)
world.addVisualisation.Sphere(world.FR_cyl5, radius=0.12)
world.addVisualisation.Sphere(world.FR_cyl6, radius=0.12)





#-------------------------------------------------------------------------------
#                           tool platform
#-------------------------------------------------------------------------------

# body
tp = world.addBody(mass=10, cg=[0,0,0], inertia=diag([5,2.5,2.5]), name='tool')


# frames for cylinders
r2 = 0.5
rot = pi/3
tp.addFrame('FR_cyl1', p=[0, r2*cos(-1/12*pi+rot), r2*sin(-1/12*pi+rot)])
tp.addFrame('FR_cyl2', p=[0, r2*cos( 1/12*pi+rot), r2*sin( 1/12*pi+rot)])
tp.addFrame('FR_cyl3', p=[0, r2*cos( 7/12*pi+rot), r2*sin( 7/12*pi+rot)])
tp.addFrame('FR_cyl4', p=[0, r2*cos( 9/12*pi+rot), r2*sin( 9/12*pi+rot)])
tp.addFrame('FR_cyl5', p=[0, r2*cos(-9/12*pi+rot), r2*sin(-9/12*pi+rot)])
tp.addFrame('FR_cyl6', p=[0, r2*cos(-7/12*pi+rot), r2*sin(-7/12*pi+rot)])


# free movement
world.addJoint(world, tp, ['Tx','Ty','Tz','Rx','Ry','Rz'])


# visualisation
tp.addFrame('FR_visual',R=rotMat(pi/2,'z'))
world.addVisualisation.Frame(tp, size=2)
world.addVisualisation.Cylinder(tp.FR_visual, radius=r2, height=0.01)

world.addVisualisation.Sphere(tp.FR_cyl1, radius=0.1)
world.addVisualisation.Sphere(tp.FR_cyl2, radius=0.1)
world.addVisualisation.Sphere(tp.FR_cyl3, radius=0.1)
world.addVisualisation.Sphere(tp.FR_cyl4, radius=0.1)
world.addVisualisation.Sphere(tp.FR_cyl5, radius=0.1)
world.addVisualisation.Sphere(tp.FR_cyl6, radius=0.1)





#-------------------------------------------------------------------------------
#                            hexapod cylinders
#-------------------------------------------------------------------------------

# parameters
m = 1
cgHous = [0.5,0,0]
cgPist = [-0.5,0,0]
inertia = diag([0.00125,0.12,0.12])

rHous = 0.05
rPist = 0.03
h = 1


# bodies for housings
cylHous1 = world.addBody(m,cgHous,inertia)
cylHous2 = world.addBody(m,cgHous,inertia)
cylHous3 = world.addBody(m,cgHous,inertia)
cylHous4 = world.addBody(m,cgHous,inertia)
cylHous5 = world.addBody(m,cgHous,inertia)
cylHous6 = world.addBody(m,cgHous,inertia)


# bodies for pistons
cylPist1 = world.addBody(m,cgPist,inertia)
cylPist2 = world.addBody(m,cgPist,inertia)
cylPist3 = world.addBody(m,cgPist,inertia)
cylPist4 = world.addBody(m,cgPist,inertia)
cylPist5 = world.addBody(m,cgPist,inertia)
cylPist6 = world.addBody(m,cgPist,inertia)


# rotational joints to base platform
world.addJoint(world.FR_cyl1, cylHous1, ['Ry','Rz'])
world.addJoint(world.FR_cyl2, cylHous2, ['Ry','Rz'])
world.addJoint(world.FR_cyl3, cylHous3, ['Ry','Rz'])
world.addJoint(world.FR_cyl4, cylHous4, ['Ry','Rz'])
world.addJoint(world.FR_cyl5, cylHous5, ['Ry','Rz'])
world.addJoint(world.FR_cyl6, cylHous6, ['Ry','Rz'])


# translational joint of cylinder
world.addJoint(cylHous1, cylPist1, 'Tx')
world.addJoint(cylHous2, cylPist2, 'Tx')
world.addJoint(cylHous3, cylPist3, 'Tx')
world.addJoint(cylHous4, cylPist4, 'Tx')
world.addJoint(cylHous5, cylPist5, 'Tx')
world.addJoint(cylHous6, cylPist6, 'Tx')


# close hexapod loops
world.addLoop.Hexapod(tp.FR_cyl1, cylPist1)
world.addLoop.Hexapod(tp.FR_cyl2, cylPist2)
world.addLoop.Hexapod(tp.FR_cyl3, cylPist3)
world.addLoop.Hexapod(tp.FR_cyl4, cylPist4)
world.addLoop.Hexapod(tp.FR_cyl5, cylPist5)
world.addLoop.Hexapod(tp.FR_cyl6, cylPist6)


# visualisation for housings
cylHous1.addFrame('FR_visual', p=[0.5,0,0], R=rotMat(pi/2,'z'))
cylHous2.addFrame('FR_visual', p=[0.5,0,0], R=rotMat(pi/2,'z'))
cylHous3.addFrame('FR_visual', p=[0.5,0,0], R=rotMat(pi/2,'z'))
cylHous4.addFrame('FR_visual', p=[0.5,0,0], R=rotMat(pi/2,'z'))
cylHous5.addFrame('FR_visual', p=[0.5,0,0], R=rotMat(pi/2,'z'))
cylHous6.addFrame('FR_visual', p=[0.5,0,0], R=rotMat(pi/2,'z'))

world.addVisualisation.Cylinder(cylHous1.FR_visual, radius=rHous, height=h)
world.addVisualisation.Cylinder(cylHous2.FR_visual, radius=rHous, height=h)
world.addVisualisation.Cylinder(cylHous3.FR_visual, radius=rHous, height=h)
world.addVisualisation.Cylinder(cylHous4.FR_visual, radius=rHous, height=h)
world.addVisualisation.Cylinder(cylHous5.FR_visual, radius=rHous, height=h)
world.addVisualisation.Cylinder(cylHous6.FR_visual, radius=rHous, height=h)


# visualisation of pistons
cylPist1.addFrame('FR_visual', p=[-0.5,0,0], R=rotMat(pi/2,'z'))
cylPist2.addFrame('FR_visual', p=[-0.5,0,0], R=rotMat(pi/2,'z'))
cylPist3.addFrame('FR_visual', p=[-0.5,0,0], R=rotMat(pi/2,'z'))
cylPist4.addFrame('FR_visual', p=[-0.5,0,0], R=rotMat(pi/2,'z'))
cylPist5.addFrame('FR_visual', p=[-0.5,0,0], R=rotMat(pi/2,'z'))
cylPist6.addFrame('FR_visual', p=[-0.5,0,0], R=rotMat(pi/2,'z'))

world.addVisualisation.Cylinder(cylPist1.FR_visual, radius=rPist, height=h)
world.addVisualisation.Cylinder(cylPist2.FR_visual, radius=rPist, height=h)
world.addVisualisation.Cylinder(cylPist3.FR_visual, radius=rPist, height=h)
world.addVisualisation.Cylinder(cylPist4.FR_visual, radius=rPist, height=h)
world.addVisualisation.Cylinder(cylPist5.FR_visual, radius=rPist, height=h)
world.addVisualisation.Cylinder(cylPist6.FR_visual, radius=rPist, height=h)





#-------------------------------------------------------------------------------
#                       generate equations and show
#-------------------------------------------------------------------------------

world.genEquations.Recursive()
world.show('Hexapod_x')
