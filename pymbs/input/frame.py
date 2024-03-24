from pymbs.common.abstractframe import AbstractFrame
from pymbs.symbolics import zeros, eye
from .input_element import MbsInputElement


class Frame(AbstractFrame, MbsInputElement):
    def __init__(self, name, parent, p, R):
        MbsInputElement.__init__(self, name, parent)
        AbstractFrame.__init__(self, name, p, R)

        self.coordList = []


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
        self.__dict__[name]=cs

        return cs

    def getParentBody(self):

        # will be overridden in Body-Class, such that
        # a body returns self
        return self.parent.getParentBody()

