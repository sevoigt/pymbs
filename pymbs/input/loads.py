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
Created on 05.07.2009

@author: knoll


Module containing classes modelling mechanical loads
'''

from .MbsInputElement import MbsInputElement
from .Frame import Frame
from .Joint import Joint

import PyMbs.Symbolics as Symbolics

class Constraint(MbsInputElement):

    def __init__(self, name, CS1, CS2, transLock, rotLock):

        assert isinstance(transLock, Symbolics.Matrix)
        assert isinstance(rotLock, Symbolics.Matrix)

        assert transLock.shape() == (3,)
        assert rotLock.shape() == (3,)

        MbsInputElement.__init__(self, name, CS1)
        self.child=CS2

        self.transLock=transLock
        self.rotLock=rotLock




class AbstractLoadElement(MbsInputElement):

    prefix=''

    def __init__(self, name, CS1, CS2, symb, CSref=None):

        assert CS1.__class__ == Frame
        assert CS2.__class__ == Frame
        assert CSref.__class__ in  (Frame, type(None))

        MbsInputElement.__init__(self, name, CS1)
        self.child=CS2
        self.symb=symb


        if CSref is None:
            self.CSref=CS1
        else:
            self.CSref=CSref


class PtPForce(AbstractLoadElement):
    prefix='F_'
    def __init__(self, name, CS1, CS2, expr, CSref=None):
        # assert expr.dim==1
        assert CSref is None # Point to Point
        AbstractLoadElement.__init__(self, name, CS1, CS2, expr, CSref)


class CmpForce(AbstractLoadElement):
    prefix='F_'
    def __init__(self, name, CS1, CS2, expr, CSref=None):
        # assert expr.dim==3
        AbstractLoadElement.__init__(self, name, CS1, CS2, expr, CSref)


class CmpTorque(AbstractLoadElement):
    prefix='T_'

    def __init__(self, name, CS1, CS2, expr, CSref=None):
        # assert expr.dim==3
        AbstractLoadElement.__init__(self, name, CS1, CS2, expr, CSref)


class JointLoad(MbsInputElement):
    # its implementation has nothing in common with the other Loads
    # therefore it is not a childclass of AbstractLoadElement

    def __init__(self, name, joint, loadsymb):

        assert isinstance(joint, Joint)
        assert isinstance(loadsymb, Symbolics.Symbol)
        self.joint = joint
        self.loadsymb = loadsymb

        MbsInputElement.__init__(self, name, None)



