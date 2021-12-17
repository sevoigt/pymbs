from .input_element import MbsInputElement
from .frame import Frame
from .joint import Joint

import pymbs.symbolics as symbolics


class Constraint(MbsInputElement):

    def __init__(self, name, CS1, CS2, transLock, rotLock):

        assert isinstance(transLock, symbolics.Matrix)
        assert isinstance(rotLock, symbolics.Matrix)

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
        assert isinstance(loadsymb, symbolics.Symbol)
        self.joint = joint
        self.loadsymb = loadsymb

        MbsInputElement.__init__(self, name, None)
