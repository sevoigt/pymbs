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

@author: cknoll
'''

from PyMbs.Common.AbstractFrame import AbstractFrame
from .MbsInputElement import MbsInputElement
from PyMbs.Symbolics import zeros, eye

class Frame(AbstractFrame, MbsInputElement):
    def __init__(self, name, parent, p, R):
        MbsInputElement.__init__(self, name, parent)
        AbstractFrame.__init__(self, name, p, R)

        self.coordList=[]


    def addFrame(self, name, p=zeros((3,)), R=eye((3,3))):
        """
        addFrame(name, p=zeros((3,)), R=eye((3,3)))
        Add a new Coordinate System (CS) as a Child.

        :param name: Name of CS
        :type name: String
        :param p: Position vector of coordinate origin
        :param R: Rotation matrix
        """
        CS=Frame(name, self, p, R)
        self.children.append(CS)
        self.coordList.append(CS)

        # add to object namespace
        assert not hasattr(self, name), "name %s already occupied" % name
        self.__dict__[name]=CS

        return CS

    def getParentBody(self):

        # will be overridden in Body-Class, such that
        # a body returns self
        return self.parent.getParentBody()

