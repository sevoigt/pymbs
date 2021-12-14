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
Created on 10.08.2009

@author: Christian Schubert
'''

from PyMbs.Symbolics import VarKind
from PyMbs.Processing.Sensors.Sensor import Sensor
from PyMbs.Common.Functions import transpose
from PyMbs.Processing.Joint import Joint

import PyMbs.Symbolics as Symbolics

class JointSensor(Sensor):
    '''
    Obtains a Joint's State of Motion (Position, Velocity)
    '''

    def __init__(self, name, joint, symbol, category=VarKind.Sensor):
        '''
        Constructor
        '''

        assert( isinstance(joint, Joint) )

        # !! this causes trouble because there is no parent and child
        #self.check()

        # Call Constructor of Super Class
        Sensor.__init__(self, name, joint.body.CS_0, joint.coordSys, symbol, None, category)
        self.joint = joint


    def calc(self, graph):
        '''
        Calculate Distance, either explicitly or symbolically
        '''
        assert( graph is not None )

        # Calculate Vector Pointing From Parent to Child (w.r.t. Inertial Frame)
        # we cannot include qdd here
        # it would cause a loop in the equations if this sensor was used to calculate a force or torque, since qdd = f(qdd, ...)
        self.writeResult(graph, Symbolics.Matrix([self.joint.q, self.joint.qd]))


    def getShape(self):
        '''
        Returns Shape of Sensor Result
        '''

        return (2,1)