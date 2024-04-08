"""
Procedurally generate a planar belt. The belt segments have a longitudinal
and bending stiffness. They are each connected by a 1D rotational joint and
a 1D translational joint.

This could also serve as chain or rope model if the stiffnesses are adjusted
accordingly.

This model is also used as benchmark. The original model with 16 segments
was created in SimulationX 4.0 and all parameters have been taken from there.
"""

import time

from pymbs.input import MbsSystem, diag

world = MbsSystem([0, 0, -1])


pitch = 0.131     # [m] pitch (length of a segment)
link_width = 0.05 # [m] width of the stiff part of a segment
mass = 15.943     # [kg] mass per length
nseg = 16         # number of segments

c = 120e6   # [N/m] translational stiffness
d =   1e6   # [Ns/m] translational damping

c_r = 1000  # [Nm/rad] rotational stiffness
d_r = 1500  # [Nms/rad] rotational damping


world.addFrame(name='start', p=[link_width/2, 0, 0])
world.addVisualisation.Frame(world, 0.05)

# Initialize lists
bodies = [None]*nseg
joints_Tx = [None]*nseg
joints_Ry = [None]*nseg

# Parameters
I = diag([2.2896, 0.0194 + mass*pitch**2, 2.2896])

# Create bodies and connect them
for i in range(0, nseg):
    # Create Body and Frame
    bodies[i] = world.addBody(mass, cg=[0, 0, -0.037], inertia=I)
    bodies[i].addFrame(name='start', p=[-link_width/2, 0, 0])
    bodies[i].addFrame(name='end', p=[link_width/2, 0, 0])

    # Create joints
    if i==0:
        joints_Ry[i], joints_Tx[i] = world.addJoint(world, bodies[i], ['Ry', 'Tx'], [0, pitch])
    else:
        joints_Ry[i], joints_Tx[i] = world.addJoint(bodies[i-1], bodies[i], ['Ry', 'Tx'], [0, pitch])

    world.addVisualisation.Line(bodies[i].start, -(pitch-link_width))
    world.addVisualisation.Sphere(bodies[i], link_width/2, res=10, color=[0.8, 0.5, 0.5])

    # Add force
    cs1 = world.start if i == 0 else bodies[i-1].end
    cs2 = bodies[i].start
    world.addSpringDamper(cs1, cs2, c, d, pitch-link_width, f'sd_{i}')

    # Add torque
    dphi = world.addSensor.Joint(symbol = f'ry_{i}', joint=joints_Ry[i])
    t = world.addExpression(symbol_str=f'tau_{i}', exp= -c_r*dphi[0] - d_r*dphi[1])
    world.addLoad.Joint(joint=joints_Ry[i], symbol=t)


t = time.time()

world.genEquations.Recursive()
print(f'Time needed for generating equations: {time.time() - t:.2f} s')

world.show('belt')
