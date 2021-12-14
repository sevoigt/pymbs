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
Created on 10.10.2009

@author: Christian Schubert
'''

from PyMbs.Processing.Body import Body
from PyMbs.Symbolics import VarKind, Matrix
from PyMbs.Processing.Sensors.Sensor import Sensor
from PyMbs.Common.Functions import transpose, diag
from PyMbs.Processing.Joint import Joint



class EnergySensor(Sensor):
    '''
    Obtain the potential and kinetic energy of a body
    '''

    def __init__(self, name, body, symbol, gravity, category=VarKind.Sensor):
        '''
        Constructor
        '''

        assert( isinstance(body, Body) )

        # Call Constructor of Super Class
        Sensor.__init__(self, name, body.CS_0, body.CS_0, symbol, None, category)
        self.gravity = gravity;
        self.body = body;

    def calc(self, graph):
        '''
        Calculate Distance, either explicitly or symbolically
        '''
        assert( graph is not None )

        # get mass and Inertia of body
        m = self.body.mass
        I = self.body.inertia

        # get potential Energy
        assert self.body.I_l is not None, 'Position of Body "%s" has not been calculated!'%self.body
        E_pot = -m*transpose(self.gravity)*self.body.I_l

        # get kinetic energy
        assert self.body.I_ldot is not None, 'Velocity of Body "%s" has not been calculated!'%self.body
        E_kin = 0.5*m*transpose(self.body.I_ldot)*self.body.I_ldot + \
                0.5*transpose(self.body.K_om)*I*self.body.K_om

        # Total Energy
        E = E_pot + E_kin
        if isinstance(E, Matrix):
            assert E.shape == (1,1)
            E = E[0]

        self.writeResult(graph, E)

    def getShape(self):
        '''
        Returns Shape of Sensor Result
        '''

        return (1,1)