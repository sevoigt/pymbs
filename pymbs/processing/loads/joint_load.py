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
from PyMbs.Common.Functions import transpose, norm

from PyMbs.Processing.Joint import Joint

from PyMbs.Symbolics import eye


class JointLoad(Load):
    '''
    Load used for actuated Joints
    '''


    def __init__(self, name, joint, load):
        '''
        Constructor
        '''

        # Assertions
        assert( isinstance(joint, Joint) )

        # Get Type of Joint
        if ((norm(joint.Phi) == 1) and (norm(joint.Psi) == 0)):
            torque = load;
            force = None;
        elif ((norm(joint.Phi) == 0) and (norm(joint.Psi) == 1)):
            torque = None;
            force = load;
        else:
            raise AttributeError('Joint %s is neither pure rotational nor translational!'%joint.name)


        # Constructor of Super Class
        # BugFix for 014:
        if (joint.body.CS_0.R != eye(3)):
            if (not hasattr(joint.body, '_CS_0')):
                joint.body._CS_0 = joint.body.addFrame('_int_CS0_2')
            Load.__init__(self, name, joint.body._CS_0, joint.coordSys, force, torque)

        else:
            Load.__init__(self, name, joint.body.CS_0, joint.coordSys, force, torque)

        self.joint = joint


    def calc(self, graph = None, states = None):
        '''
        Calculcate Wf and Wt
        Overwrites Function in Super Class
        '''

        self.Wf = self.joint.Psi
        self.Wt = self.joint.Phi
