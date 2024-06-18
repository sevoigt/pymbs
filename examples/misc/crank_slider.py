"""
Model with a crank driving a sliding mass, like a piston of a
combustion engine
"""

from pymbs.symbolics import VarKind

from pymbs.input import MbsSystem, diag, pi


world = MbsSystem([0,0,-1])


# Parameters
m1 = world.addParam('m1', 5, name='mass_1')
l1 = world.addParam('l1', 0.5, name='len_rod_1')
I1 = world.addParam('I1', m1*l1**2/12, name='inertia_1')

m2 = world.addParam('m2', 5, name='mass_2')
l2 = world.addParam('l2', 2, name='len_rod_2')
I2 = world.addParam('I2', m2*l2**2/12, name='inertia_2')

m3 = world.addParam('m3', 10, name='mass_3')

# Bodies
cs = world.addBody(name='CrankShaft', mass=m1, cg=[l1/2,0,0], inertia=diag([0,I1,0]))
cs.addFrame('cs_rod', p=[l1,0,0])

rod = world.addBody(name='Rod', mass=m2, cg=[l2/2,0,0], inertia=diag([0,I2,0]))
rod.addFrame('rod_cs', p=[l2,0,0])

piston = world.addBody(name='Piston', mass=m3)
piston.addFrame('piston_rod', p=[0,0,0])

# Joints
world.addJoint(world, cs, 'Ry', 0, name='Ry_Crank')
world.addJoint(world, piston, 'Tx', 2.5, name='Tx_Piston')
world.addJoint(piston.piston_rod, rod, 'Ry', pi, name='Ry_Rod')

# Constraints
#world.addConstraint(rod.rod_cs, piston, [1,0,1], name='Rod_Piston')
world.addLoop.CrankSlider(cs.cs_rod, rod.rod_cs, 'Loop')

# Visuals
world.addVisualisation.Line(cs,  0.5)
world.addVisualisation.Line(rod, 2)
world.addVisualisation.Box(piston, 0.3, 0.1, 0.1)

# Energy Sensors - work with explicit algorithm only
#E_cs = world.addSensor.Energy('E_cs', cs, 'E_cs')
#E_rod = world.addSensor.Energy('E_rod', rod, 'E_rod')
#E_piston = world.addSensor.Energy('E_piston', piston, 'E_piston')
#E = world.addExpression('E', E_cs+E_rod+E_piston, category=VarKind.Sensor, name='Energy')

world.genEquations.Recursive()
#world.genCode.Modelica('crank_slider', './output')
#world.genCode.Matlab('crank_slider', './output')
#world.genMatlabAnimation('crank_slider', './output')
world.show('crank_slider')
