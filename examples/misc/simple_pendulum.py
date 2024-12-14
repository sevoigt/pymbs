'''
A simple pendulum:


                 Ry
        ----------O----------
                  \ \
                   \ \
                    \m\
                     \ \
                      \ \
                       \_\

'''


from pymbs.input import MbsSystem

world = MbsSystem()


# body and joint
pend = world.addBody(mass=0.0785,
                     cg=[0,0,0],
                     inertia=[66.07e-6, 0,66.07e-6,0,0,1.31e-6],
                     name='pendulum')
pend.addFrame('link',[0,0,0.05] )


world.addJoint(world, pend.link, 'Ry', 1)

world.addVisualisation.Box(pend, 0.01, 0.01, 0.1)


# generate equations and output
#world.genEquations.Recursive(kinematicsOnly=False,graphOptimizations=True)
world.genEquations.Explicit(graphOptimizations=True)

#world.genCode.Modelica('pendulum_simple', './output')
#world.genCode.Python('pendulum_simple', './output')

world.show('simple_pendulum')
