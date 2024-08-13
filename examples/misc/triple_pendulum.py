"""
Model of a triple pendulum where the parts are connected by one
rotational degree of freedom
"""

from pymbs.input import *

world = MbsSystem([-1,0,0])


# Hinzufuegen eines Koordinatensystems, welches um 90 Grad um die z-Achse gedreht
# ist, damit Aufhaengung richtig gedreht ist
world.addFrame('base_gedreht',R=rot_mat(pi/2,'z'))

# einezelne Pendel und Haken als Koerper erzeugen
haken = world.addBody(mass = 1, cg = [0,0,0], inertia = diag([0,1,0]), name = 'Haken')
pendel = world.addBody(mass = 1, cg = [-1.5,0,0], inertia = diag([0,1/12+2.25,0]), name = 'Pendel')
pendel2 = world.addBody(mass = 1, cg = [-1.5,0,0], inertia = diag([0,1/12+2.25,0]), name = 'Pendel2')
pendel3 = world.addBody(mass = 1, cg = [-1.5,0,0], inertia = diag([0,1/12+2.25,0]), name = 'Pendel3')

# Erzeugung der Koordinatensysteme am Ende der jeweiligen Pendel um neuen
# Koerper ueber Drehgelenk anfuegen zu koennen
verb_pendel_pendel2 = pendel.addFrame('Verbindung_Pendel_Pendel2',[-3,0,0])
verb_pendel2_pendel3 = pendel2.addFrame('Verbindung_Pendel2_Pendel3',[-3,0,0])
verb_pendel3_Masse = pendel3.addFrame('Verbindung_Pendel3_Masse',[-3,0,0])

# Erzeugung der Koordinatensysteme in der Mitte der Pendel um Koerper
# als Pendel zu visualisieren, Visualisierungen muessen immer in der Mitte,
# befestigt werden, damit sie an der richtigen Position sind.
visualisierung_pendel = pendel.addFrame('vis',p=[-1.5, 0, 0],R=rot_mat(pi/2,'z'))
visualisierung_pendel2 = pendel2.addFrame('vis2',p=[-1.5, 0, 0],R=rot_mat(pi/2,'z'))
visualisierung_pendel3 = pendel3.addFrame('vis3',p=[-1.5, 0, 0],R=rot_mat(pi/2,'z'))

# Verbinden der Koerper durch Gelenke
world.addJoint(name = 'Rotation', CS1 = world, CS2 = pendel, dofList = 'Ry')
world.addJoint(name = 'Rotation2', CS1 = verb_pendel_pendel2, CS2 = pendel2, dofList = 'Ry')
world.addJoint(name = 'Rotation3', CS1 = verb_pendel2_pendel3, CS2 = pendel3, dofList = 'Ry')
world.addJoint(name = 'starres_Gelenk',CS1 = verb_pendel3_Masse,CS2 = haken,dofList = [])


# Visualisierung
world.addVisualisation.Cylinder(visualisierung_pendel, radius = 0.01, height = 3)
world.addVisualisation.Cylinder(visualisierung_pendel2, radius = 0.01, height = 3)
world.addVisualisation.Cylinder(visualisierung_pendel3, radius = 0.01, height = 3)
world.addVisualisation.Sphere(verb_pendel3_Masse, radius=0.2)
world.addVisualisation.Cylinder(world.base_gedreht, radius=2, height=0.01)

world.genEquations.OrderN()
world.genCode.Matlab('triple_pendulum', './output')
world.show('triple_pendulum')
