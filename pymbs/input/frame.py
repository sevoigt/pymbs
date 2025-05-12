"""
This module contains the Frame class which is basically a cartesian
coordinate system
"""

from pymbs.common.abstractframe import AbstractFrame
from pymbs.symbolics import zeros, eye
from .input_element import MbsInputElement


class Frame(AbstractFrame, MbsInputElement):
    """
    A cartesian coordinate system with a positon and an orientation (rotation)
    in three-dimensional space.
    """
    def __init__(self, name, parent, p, R):
        MbsInputElement.__init__(self, name, parent)
        AbstractFrame.__init__(self, name, p, R)

        self.coordList = []
        self._frames = {}


    def addFrame(self, name, p=zeros((3,)), R=eye((3,3))):
        """
        addFrame(name, p=zeros((3,)), R=eye((3,3)))
        Add a new Coordinate System (CS) as a Child.

        :param name: Name of CS
        :type name: String
        :param p: Position vector of coordinate origin
        :param R: Rotation matrix
        """
        cs = Frame(name, self, p, R)
        self.children.append(cs)
        self.coordList.append(cs)

        # add to object namespace
        assert not hasattr(self, name), f'name {name} already occupied'
        self._frames[name] = cs

        return cs


    def __getattr__(self, name):
        """
        Make child-frames accessible as attributes via their name
        """
        if name in self._frames:
            return self._frames[name]
        else:
            raise AttributeError(f'frame {self.name} does not have a child frame {name}')


    # TODO: the parent-child relationship is not obvious / there seems to
    # be an implicit distinction between frame and body here respectively
    def getParentBody(self):

        # will be overridden in Body-Class, such that
        # a body returns self
        return self.parent.getParentBody()
