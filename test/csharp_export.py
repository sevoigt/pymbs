"""
Test model for C#-export (and Unity)
"""

from pymbs.input import *


world = MbsSystem([0, 0, -1])

m = world.addParam('m', 1.0)
l = world.addParam('l', 1.0)
I = world.addParam('Inertia', (m*l**2)/12)

pend = world.addBody(mass=m, inertia=diag([0, I, 0]))
pend.addFrame('link', [0,0,0.5*l])

world.addJoint(world, pend.link, 'Ry')

world.addVisualisation.Box(pend, length=0.1, width=0.1, height=1.0)

world.genEquations.Recursive()
#world.show('SimplePendulum')

world.genCode.CSharp('pendulum', 'D:/Unity/PyMbsDemo/Assets/PyMbs')
