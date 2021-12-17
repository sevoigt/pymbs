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
Ex = eye(3)[:,0]
Ey = eye(3)[:,1]
Ez = eye(3)[:,2]


# The code below this line can be deleted when all examples have been changed
# from genEquations(world)
# to world.genEquations()
# ------------------------------------------
from pymbs.input.transformation import PublicMethods

#src:
#http://wiki.python.org/moin/PythonDecoratorLibrary#GeneratingDeprecationWarnings
import warnings

def deprecated(func):
    """
    This is a decorator which can be used to mark functions
    as deprecated. It will result in a warning being emitted
    when the function is used.
    """
    
    def new_func(*args, **kwargs):
        warnings.warn("Call to deprecated function %s." % func.__name__,
                      category=DeprecationWarning, stacklevel=2)
        return func(*args, **kwargs)
    new_func.__name__ = func.__name__
    new_func.__doc__ = func.__doc__
    new_func.__dict__.update(func.__dict__)
    return new_func


genEquations = deprecated(PublicMethods.genEquations)
genCode = deprecated(PublicMethods.genCode)
show = deprecated(PublicMethods.show)
genSarturisXml = deprecated(PublicMethods.genSarturisXml)
genMatlabAnimation = deprecated(PublicMethods.genMatlabAnimation)





