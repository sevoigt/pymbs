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
Created on 30.06.2009

@author: Christian Schubert
'''

from PyMbs.Common.MbsElement import MbsElement
import PyMbs.Processing
import PyMbs.Symbolics as Symbolics


class Load(MbsElement):
    '''
    Load applied to a Coordinate System
    '''


    def __init__(self, name, parent, child, force=None, torque=None, ref = None):
        '''
        Constructor
        name: Name of the Load
        force: 3x1 Force Vector (w.r.t. ref)
        torque: 3x1 Torque Vector (w.r.t. ref)
        ref: Reference Frame
        '''

        # Type Conversions
        if (ref is None): ref = parent

        # Assertions
        assert( isinstance(name, str) )
        assert( isinstance(parent, PyMbs.Processing.Frame) )
        assert( isinstance(child, PyMbs.Processing.Frame) or (child is None) )
        assert( isinstance(force, Symbolics.Symbol) or (force is None) )
        assert( isinstance(torque, Symbolics.Symbol) or (torque is None) )
        assert( isinstance(ref, PyMbs.Processing.Frame) )


        # Call Constructor of MbsElement
        MbsElement.__init__(self, name)

        # Assign Parameters
        self.parent = parent
        self.child = child
        self.force = force;
        self.torque = torque;
        self.ref = ref;

        # Connect to Parent and Child Coordinate System
        parent.loads.append(self)
        if (child is not None):
            child.loads.append(self)


    def calc(self, graph = None, states = None):
        '''
        Calculcate Wf and Wt
        '''

        raise NotImplementedError
