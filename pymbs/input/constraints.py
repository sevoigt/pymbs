"""
Module with constraints and loops
"""

from .input_element import MbsInputElement
from .joint import Joint
from .frame import Frame
from .body import Body

#!! TODO: rename to AbstractLoop
# .. and the whole module to Loops
class Constraint(MbsInputElement):
    """
    prevent motion between two coordinate systems
    """

    def __init__(self, name, CS1, CS2, conList):
        """
        name
        Frame1
        Frame2
        list of constraints
        """
        self.super(name, CS1)
        self.child=CS2
        self.conList=conList
        self._checkConList()

    def _checkConList(self):
        """
        check whether the list of constraints meets the conventions
        """

        assert 0 < len(self.conList) <=6

        for elt in self.conList:
            assert elt in ('x', 'y', 'z', 'Rx', 'Ry', 'Rz')

class Loop(MbsInputElement):
    """
    entity which is used to prevent algebraic constraints for some common
    types of kinematic loops
    """
    def __init__(self, name, joints, CSList):
        """
        expects List of Joints, List of Frames
        """

        assert( isinstance(joints, list) )
        for j in joints: assert( isinstance(j, Joint) )
        assert( isinstance(CSList, list) )

        # Convert Bodies into Frames
        for i in range(len(CSList)):
            cs = CSList[i]
            if (isinstance(cs, Body)):
                cs = cs._CS_0
                CSList[i] = cs
            assert( isinstance(cs, Frame) )

        if (len(CSList) > 1):
            MbsInputElement.__init__(self, name, CSList[0])
            self.child=CSList[1]
        else:
            MbsInputElement.__init__(self, name, None)
            self.child=None

        self.joints = joints
        self.CSList = CSList


class FourBar(Loop):
    def __init__(self, name, CS1, CS2, posture = 1):
            Loop.__init__(self, name, [], [CS1, CS2])
            self.posture = posture

class FourBarTrans(Loop):
    def __init__(self, name, CS1, CS2, posture = 1):
            Loop.__init__(self, name, [], [CS1, CS2])
            self.posture = posture

class ThreeBarTrans(Loop):
    def __init__(self, name, CS1, CS2):
            Loop.__init__(self, name, [], [CS1, CS2])

class Steering(Loop):
    def __init__(self, name, CS1, CS2, setUpH = 1, setUpS = 1):
            Loop.__init__(self, name, [], [CS1, CS2])
            self.setUpH = setUpH
            self.setUpS = setUpS

class CrankSlider(Loop):
    def __init__(self, name, CS1, CS2):
            Loop.__init__(self, name, [], [CS1, CS2])

class Transmission(Loop):
    def __init__(self, name, J1, J2, ratio = 1):
            Loop.__init__(self, name, [J1, J2], [])
            self.ratio = ratio

class ExpJoint(Loop):
    def __init__(self, name, J1, exp):
            Loop.__init__(self, name, [J1], [])
            self.exp = exp

class Hexapod(Loop):
    def __init__(self, name, CS1, CS2):
            Loop.__init__(self, name, [], [CS1, CS2])

class Hexapod_m_AV(Loop):
    def __init__(self, name, CS1, CS2):
            Loop.__init__(self, name, [], [CS1, CS2])
