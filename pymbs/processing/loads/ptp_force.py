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

'''
Created on 02.07.2009

@author: Christian Schubert
'''

from PyMbs.Processing.LoadElements.Load import Load
from PyMbs.Common.Functions import transpose, sqrt

from PyMbs.Symbolics import Matrix, zeros


class PtPForce(Load):
    '''
    Point to Point Force
    '''


    def __init__(self, name, parent, child, force, ref = None):
        '''
        Constructor
        '''

        # Assertions
        assert(parent is not None)
        assert(child is not None)

        # Constructor of Super Class (ref=None, means ref = Inertial Frame)
        Load.__init__(self, name, parent, child, force, torque = None)
        # Setting Ref to None Means That the Inertial Frame is the Reference Frame
        self.ref = None


    def calc(self, graph = None, states = None):
        '''
        Calculcate Wf and Wt
        Overwrites Function in Super Class
        '''

        # Calculate Vector Pointing From Child to Parent (w.r.t. Inertial Frame)
        r = self.parent.I_r - self.child.I_r
        if (isinstance(r, Matrix)):
            l = r.norm()
            r = r/l
        else:
            assert( graph is not None )
            r_sym = graph.addEquation( 'r_%s'%self.name, r )
            l = sqrt( transpose(r_sym) * r_sym )
            r = r_sym/l

        # Update Wf, Wft
        self.Wf = r
        self.Wt = zeros((3,))
