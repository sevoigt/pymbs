#-------------------------------------------------------------------------------
# Name:        module1
# Purpose:
#
# Author:      Schubert
#
# Created:     09.09.2011
# Copyright:   (c) Schubert 2011
# Licence:     <your licence>
#-------------------------------------------------------------------------------
#!/usr/bin/env python

from PyMbs.Input import *

world = MbsSystem()

EXPLICIT = 1
RECURSIVE = 2
ORDERN = 3

MODE = ORDERN

l = 1.0
rod = world.addBody(mass=1, cg=[l/2,0,0], inertia=diag([0,0,1]), name="rod")
rod.addFrame(name='end', p=[l,0,0])
world.addJoint(CS1=world, CS2=rod, dofList='Rz')
F = world.addParam('F', 1)
world.addLoad.CmpForce([0,F,0], CS1=rod.end, CS2=world )
world.addVisualisation.Line(rod, length=l)
world.addVisualisation.Frame(world)

if (MODE == EXPLICIT):
    world.genEquations.Explicit()
    world.genCode.Python('rod_Explicit', 'Output')
elif (MODE == RECURSIVE):
    world.genEquations.Recursive()
    world.genCode.Python('rod_Recursive', 'Output')
else:
    world.genEquations.OrderN(graphOptimizations=True)
    world.genCode.Python('rod_OrderN', 'Output')

world.show('Rod')

