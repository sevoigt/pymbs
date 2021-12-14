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
Created on 14.07.2009

@author: Christian Schubert
'''

from PyMbs.Processing.Sensors.Sensor import Sensor
from PyMbs.Common.Functions import transpose



class AccelerationSensor(Sensor):
    '''
    Calculates the Relative Acceleration between Parent and Child w.r.t. ref
    '''

    def calc(self, graph):
        '''
        Calculate Distance, either explicitly or symbolically
        '''
        assert( graph is not None )
        self.check()
        
        # Calculate Vector Pointing From Child to Parent (w.r.t. Inertial Frame)
        a = transpose(self.ref.I_R)*(self.child.I_a - self.parent.I_a)
        if (a == 0):
            a = PyMbs.Symbolics.zeros(self.getShape())
        
        # Add Result to the List of Equations
        self.writeResult(graph, a)
        
    def getShape(self):
        '''
        Returns Shape of Sensor Result
        '''
        
        return (3,1)