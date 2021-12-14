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
Created on 29.06.2009

@author: knoll
'''


from PyMbs.Symbolics import Symbol, Matrix
from .MbsInputElement import MbsInputElement

from .Frame import Frame

class Joint(MbsInputElement):
    """
    implement a joint with 1 DOF
    """
    def __init__(self, name, CS1, CS2, dof, \
                                        startVal=None, startVal_d=None):
        """
        name
        Frame1
        Frame2
        degree-of-freedom-List
        list of symbol for those
        start values
        start velocities
        """

        assert CS1.__class__ == Frame
        assert CS2.__class__ == Frame

        if (dof is not None) and (not dof in ('Tx', 'Ty', 'Tz',  'Rx', 'Ry', 'Rz')):
            raise ValueError("Invalid DOF-Specifier for Joint %s" % name)

        # append dof-suffix to name
        if '_%s' % dof not in name:
           name += '_%s' % dof

        assert isinstance(startVal, (int, float, Symbol))
        assert isinstance(startVal_d, (int, float, Symbol))

        MbsInputElement.__init__(self,name, CS1)
        self.child=CS2
        self.dof=dof

        if startVal == None:
            startVal=0

        if startVal_d == None:
            startVal_d=0

        self.q0=startVal
        self.qd0=startVal_d

        self.jointSensor = None
        self.jointLoad = None


    @property
    def coord(self):
        '''
        Return the position coordinate (=variable) for this joint
        '''
        if (self.referenceToNew is None):
            raise ValueError('genEquations must be run before variables can be retrieved!')
        return self.referenceToNew.q


    @property
    def vel(self):
        '''
        Return the velocity coordinate (=variable) for this joint
        '''
        if (self.referenceToNew is None):
            raise ValueError('genEquations must be run before variables can be retrieved!')
        return self.referenceToNew.qd


    @property
    def acc(self):
        '''
        Return the acceleration coordinate (=variable) for this joint
        '''
        if (self.referenceToNew is None):
            raise ValueError('genEquations must be run before variables can be retrieved!')
        return self.referenceToNew.qdd


    @property
    def IsIndependent(self):
        '''
        Return whether this joint is truly independent
        '''
        if (self.referenceToNew is None):
            raise ValueError('genEquations must be run before variables can be retrieved!')
        return self.referenceToNew.IsIndependent
