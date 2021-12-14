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
from PyMbs.Common.Functions import transpose, sqrt
from PyMbs.Common.Simple import simple

from PyMbs.Symbolics import Matrix 


class DistanceSensor(Sensor):
    '''
    Calculates the Distance and its Derivative between two Coordinate Systems
    '''

    def calc(self, graph):
        '''
        Calculate Distance, either explicitly or symbolically
        '''
        assert( graph is not None )
        self.check()
        
        # Calculate Vector Pointing From Child to Parent (w.r.t. Inertial Frame)
        r = self.parent.I_r - self.child.I_r
        v = self.parent.I_v - self.child.I_v
        # Explicit if r is a Matrix
        if (isinstance(r, Matrix)):
            d = r.norm()
            d = simple(d)
            
            d_dot = transpose((r/d))*v
            d_dot = simple(d_dot[0,0])
        else:
            r_sym = graph.addEquation('r_%s'%self.name, r)
            l_sym = graph.addEquation('l_%s'%self.name, sqrt( transpose(r_sym) * r_sym ) )                                       
            v_sym = graph.addEquation('v_%s'%self.name, v)
            d = l_sym 
            d_dot = transpose(r_sym)*v_sym / l_sym 
        
        # Add Result to the List of Equations
        self.writeResult(graph, Matrix([d, d_dot]))
        
        
    def getShape(self):
        '''
        Returns Shape of Sensor Result
        '''
        
        return (2,)