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

from .MbsElement import MbsElement
from PyMbs.Symbolics import Basic

class State(MbsElement):
    '''
    State Object holds all Elements which are part of the state, including algebraic variables
    '''

    def __init__(self, q=None, qd=None, qdd=None, mu=None):
        '''
        Constructor
        '''

        # It is not advisable to use more than one empty list as a default argument
        if (q is None):
            q = []
        if (qd is None):
            qd = []
        if (qdd is None):
            qdd = []
        if (mu is None):
            mu = []
        # Achtung Assertions: prüfen auf int muss noch geändert werden
        # Assertions
        assert( isinstance(q, list) )
        for e in q:
            assert( isinstance(e, (Basic,int)) )
        assert( isinstance(qd, list) )
        for e in qd:
            assert( isinstance(e, (Basic,int)) )
        assert( isinstance(qdd, list) )
        for e in qdd:
            assert( isinstance(e, (Basic,int)) )
        assert( isinstance(mu, list) )
        for e in mu:
            assert( isinstance(e, (Basic,int)) )

        # Constructor of Super Class
        MbsElement.__init__(self, 'State')

        # Save Parameters
        self.q = q              # Positions
        self.qd = qd            # Velocities
        self.qdd = qdd          # Accelerations
        self.mu = mu            # Constraint Forces/Torques
