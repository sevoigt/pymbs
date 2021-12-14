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

from PyMbs.Common.AbstractFrame import AbstractFrame

from . import Body
from . import Joint

import PyMbs.Symbolics as Symbolics

class Frame(AbstractFrame):
    '''
    Class Of A Coordinate System. Used To Mark Important Points On Bodies.
    '''


    def __init__(self, name, parentBody, p=Symbolics.zeros((3,)), R=Symbolics.eye((3,3)), graph = None):
        '''
        Constructor
        parentBody: Reference to ParentBody (Body)
        p: Position (3x1 Vector)
        R: Orientation (3x3 Vector)
        '''

        # Assertions
        assert( isinstance(parentBody, (Body.Body,Body.FlexibleBody) ))

        # Constructor
        assert graph is not None
        AbstractFrame.__init__(self, name, p, R, graph)

        # Assign Parameters
        self.parentBody = parentBody    # reference to parentBody

        # Additional Attributes
        self.joints = []                # list of connected joints
        self.loads = []                 # list of connected loads

        # attributes used for calculation
#       self.p = p              # Position of Origin w.r.t. Parent Frame
        self.I_r = None         # Position of Origin w.r.t. Inertial Frame
        self.I_v = None         # Velocity of Origin w.r.t. Inertial Frame
        self.I_a = None         # Acceleration of Origin w.r.t. Inertial Frame

        self.I_R = None         # Transformation Matrix, Inertial Frame <- This Frame
#        self.R = R              # Transformation Matrix, Body Frame <- This Frame
        self.K_om = None        # Angular Velocity w.r.t. (Body Frame if Explicit, Inertial Frame if Recursive!!!)
        self.K_al = None        # Angular Acceleration w.r.t. (Body Frame if Explicit, Inertial Frame if Recursive!!!)


    def connectTo(self, body, type, axis, name = None, q0 = 0, qd0 = 0):
        '''
        Connect This Coordinate System to a Body
        body: Body which Shall Be Connected
        type: 'Rot' or 'Trans' for Rotational or Translational Joint
        axis: Axis of Translation/Rotation (normalised 3x1 vector)
        '''

        # Name of Bodies Involved
        pName = self.parentBody.name;
        cName = body.name;

        # Create a Joint Object
        if ((type == 'Rot') or (type == 'Rotation')):
            if (name is None): name = 'Rot_%s_%s' % (pName, cName)
            j = Joint.Joint(name, self, body, Symbolics.zeros((3,)), axis, q0, qd0, self.graph)
        elif ((type == 'Trans') or (type == 'Translation')):
            if (name is None): name = 'Trans_%s_%s' % (pName, cName)
            j = Joint.Joint(name, self, body, axis, Symbolics.zeros((3,)), q0, qd0, self.graph)
        else:
            raise NameError('Unknown Joint Type: %s' % type)

        # Connect Joint to Frame and Body
        self.joints += [j]
        body.joint = j

        return j