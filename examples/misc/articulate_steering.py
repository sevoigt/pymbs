"""
Model description of a vehicle with articulated steering
"""

from pymbs.input import MbsSystem, diag

world=MbsSystem([0,0,-1])

l=world.addParam('l', 1.0)

m_front=world.addParam('m_front', 100.0)
I_front=world.addParam('inertia_front', [0.85,0.46,0.85])

m_back=world.addParam('m_back', 100.0)
I_back=world.addParam('inertia_back', [0.85,0.46,0.85])

m_wheel=world.addParam('m_rad', 12.0)
I_wheel=world.addParam('inertia_rad', [0.1,0.2,0.1])

front_body=world.addBody(mass=m_front, cg=[l,0,0], inertia=diag(I_front))
front_body.addFrame('CS_steering', [0,0,0])
front_body.addFrame('CS_wheel_l', [1.5*l,l,0])
front_body.addFrame('CS_wheel_r', [1.5*l,-l,0])
front_body.addFrame('origin', [l,0,0])
world.addVisualisation.Frame(front_body.origin)

back_body=world.addBody(mass=m_back, cg=[-l,0,0], inertia=diag(I_back))
back_body.addFrame('CS_steering', [0,0,0])
back_body.addFrame('CS_wheel_l', [-1.5*l,l,0])
back_body.addFrame('CS_wheel_r', [-1.5*l,-l,0])
back_body.addFrame('origin', [-l,0,0])
world.addVisualisation.Frame(back_body.origin)


world.addJoint(world, front_body.CS_steering, ['Tx', 'Ty', 'Tz', 'Rx', 'Ry', 'Rz'], name='DOF6')

j1 = world.addJoint(front_body.CS_steering, back_body.CS_steering, 'Rz', name='steering')
steering = world.addInput('in_steering')
world.addLoop.ExpJoint(j1,steering)


flanges = [front_body.CS_wheel_l, front_body.CS_wheel_r, back_body.CS_wheel_l, back_body.CS_wheel_r]
for i in range(0,4):
    wheel = world.addBody(mass=m_wheel, cg=[0,0,0], inertia=diag(I_wheel))
    wheel.addFrame('CS_flange', [0,0,0])
    world.addVisualisation.Frame(wheel)
    world.addJoint(flanges[i], wheel.CS_flange, 'Ry')

    # Wheel driving torque
    T = world.addInput(f'T_wheel_{i}')
    T_vec = world.addExpression(f'T_vec_{i}',  [0,T,0])
    world.addLoad.CmpTorque(T_vec, wheel.CS_flange, flanges[i])

    # Tyre sensors and loads interface
    world.addTyreSensorsLoads(f'TS_wheel_{i}', flanges[i], wheel.CS_flange)

world.addSensor.CenterofMass('total_mass')

print("System has been assembled")

world.genEquations.Recursive()
world.genCode.Modelica('articulate_steering', './output',inputsAsInputs=False)

world.show('articulate_steering')
