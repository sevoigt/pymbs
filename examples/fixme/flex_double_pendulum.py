# -*- coding: utf-8 -*-
"""
@author: jochmann

Model description for a flexible body zero dof
"""

from pymbs.input import MbsSystem, rot_mat, pi

world=MbsSystem([1,0,0])
world.addFrame('base_rotated',R=rot_mat(pi/2,'z'))

flex_bod=world.addFlexibleBody('../sid/five_node_model.sid_fem',name='pendulum_flex')
flex_bod2=world.addFlexibleBody('../sid/five_node_model.sid_fem',name='pendulum_flex2')

world.addVisualisation.flexible_Body(cs=flex_bod.node(1), flex_Body=flex_bod, name='flexi')
world.addVisualisation.flexible_Body(cs=flex_bod2.node(1), flex_Body=flex_bod2, name='flexi2')
world.addVisualisation.Cylinder(world.base_rotated, radius=5, height=0.1)

world.addJoint(name='revolute', CS1=world, CS2=flex_bod.node(1), dofList=[])
world.addJoint(name='revolute2', CS1=flex_bod.node(5), CS2=flex_bod2.node(1), dofList='Rz')
#world.addLoad.CmpForce([0,50,0],CS1=world,CS2=pend2.node(2),name='kraft2')
#world.addLoad.Joint(M,a,name = 'drehmom')

##world.addVisualisation.Cylinder(visualisierung_pendel, radius = 0.01, height = 3)
##world.addVisualisation.Cylinder(visualisierung_pendel, radius = 0.01, height = 3)

explicit = True
if (explicit):
    world.genEquations.Recursive()
    world.genCode.Matlab('flex_double_pendulum', './output')
else:
    world.genEquations.OrderN(kinematicsOnly = False)
    world.genCode.Matlab('flex_double_pendulum', './output')
    world.genCode.Python('flex_double_pendulum', './output')


# new: configuration for gui
options = {'qFlexScaling': 20}

world.show('flex_rotation', options=options)
