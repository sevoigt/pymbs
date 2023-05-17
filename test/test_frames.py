'''
Tests for adding frames
'''


from pymbs.input import *

world = MbsSystem([0, 0, -1])

world.addFrame(name='A', p=[1,0,0])

world.addVisualisation.Frame(world, 0.5)
world.addVisualisation.Frame(world.A, 0.1)

world.genEquations.Recursive()
world.show()
