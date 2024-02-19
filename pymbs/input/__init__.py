"""
Everything that's needed to create the input version of the model
"""

from numpy import pi
# somehow math.pi does not work?!
#from math import pi # a simple float instance (here: better than pymbs.Symbolics.pi)

from pymbs.input.mbs_system import MbsSystem
from pymbs.input.body import Body
from pymbs.input.frame import Frame

from pymbs.symbolics import zeros, eye, sin, cos, Matrix
from pymbs.common.functions import diag, rotMat


# unitvectors for convenience
# TODO: breaks autodoc on Read the Docs because there the compiled symbolics 
# module is not available
#Ex = eye(3)[:,0]
#Ey = eye(3)[:,1]
#Ez = eye(3)[:,2]
