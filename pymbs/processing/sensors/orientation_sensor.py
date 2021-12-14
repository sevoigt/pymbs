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
Created on 08.07.2009

@author: Christian Schubert
'''

from PyMbs.Processing.Sensors.Sensor import Sensor
from PyMbs.Common.Functions import transpose

import PyMbs.Symbolics as Symbolics

class OrientationSensor(Sensor):
    '''
    Calculates the Relative Orientation between Parent and Child yielding a Transformation Matrix (Parent <- Child)
    '''

    def calc(self, graph):
        '''
        Calculate Transformation Matrix
        '''
        assert( graph is not None )        
        self.check()
        
        # Calculate Vector Pointing From Child to Parent (w.r.t. Inertial Frame)
        if (isinstance(self.parent.I_R, Symbolics.Matrix)):
            R = transpose(self.parent.I_R)*self.child.I_R
        else:
            R = transpose(self.parent.I_R)*self.child.I_R
        
        # Add Result to the List of Equations
        self.writeResult(graph, R)
        
                
    def getShape(self):
        '''
        Returns Shape of Sensor Result
        '''
        
        return (3,3)