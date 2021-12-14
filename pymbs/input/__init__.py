# -*- coding: utf-8 -*-
'''
This file is part of PyMbs.

PyMbs is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

PyMbs is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with PyMbs.
If not, see <http://www.gnu.org/licenses/>.

Copyright 2011, 2012 Carsten Knoll, Christian Schubert,
                     Jens Frenkel, Sebastian Voigt
'''

# import all, what will be needed to formulate the Input-Version
# of the modell

from PyMbs.Symbolics import zeros, eye, sin, cos, Matrix
from numpy import pi

# somehow math.pi does not work?!
#from math import pi # a simple float instance (here: better than PyMbs.Symbolics.pi)


from PyMbs.Input.MbsSystem import MbsSystem
from PyMbs.Input.Body import Body
from PyMbs.Input.Frame import Frame

from PyMbs.Common.Functions import diag, rotMat

# unitvectors as convenience

Ex = eye(3)[:,0]
Ey = eye(3)[:,1]
Ez = eye(3)[:,2]



# The code below this line can be deleted when all examples have been changed
# from genEquations(world)
# to world.genEquations()
# ------------------------------------------
from PyMbs.Input.transformation import PublicMethods

#src:
#http://wiki.python.org/moin/PythonDecoratorLibrary#GeneratingDeprecationWarnings
import warnings

def deprecated(func):
    """This is a decorator which can be used to mark functions
    as deprecated. It will result in a warning being emitted
    when the function is used."""
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





