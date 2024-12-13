# -*- coding: utf-8 -*-
"""
Created on 02.10.2009
@author: Christian Schubert

Double pendulum modelled with a constraint between the two bodies
This results in a system of differential-algebraic-equations (dae)
"""

from pymbs.input import MbsSystem, diag

world = MbsSystem([0,0,-1])

m1 = world.addParam('m1', 1, name='mass1')
l1 = world.addParam('l1', 1, name='length1')
m2 = world.addParam('m2', 1, name='mass2')
l2 = world.addParam('l2', 1, name='length2')

pend1 = world.addBody(m1, [l1/2,0,0], diag([0,m1*l1**2/12,0]), name='Pendulum1')
pend1.addFrame('tip', p=[l1,0,0])
pend2 = world.addBody(m2, [l2/2,0,0], diag([0,m2*l2**2/12,0]), name='Pendulum2')

world.addJoint(world, pend1, 'Ry', 1, name='J_Rot1')

world.addJoint(world, pend2, ['Tx', 'Tz', 'Ry'], [0.5,-0.8,1], name='J_Planar')

world.addConstraint(pend1.tip, pend2, transLock=[1,0,1], name='Constr_Tip')

world.addVisualisation.Frame(world, 0.2)
world.addVisualisation.Line(pend1, 1)
world.addVisualisation.Line(pend2, 1)

print('System has been assembled')

world.genEquations.Recursive()

# TODO: crashes when writing the code (to file or for gui)
world.genCode.Modelica('double_pendulum_constraint', './output')
#world.show('double_pendulum_constraint')
