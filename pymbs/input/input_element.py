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
Created on 31.03.2009

@author: sebavoig
'''

from PyMbs.Common.MbsElement import MbsElement

class MbsInputElement(MbsElement):
    '''
    This class provides basic properties for all mbs-elements:
        name
        predecessor (parent)
        successor (child)
    '''

    def __init__(self, name, parent):
        MbsElement.__init__(self, name, None)
        self.parent = parent
        self.children = []

        # Reference to the coresponding Processing Object
        self.referenceToNew=None

    def _getChild(self):
        return self.children[0]

    def _setChild(self, val):
        if len(self.children)==0:
            self.children.append(val)
        else:
            self.children[0]=val

    # make "child" a shortcut to "self.children[0]"
    child=property(_getChild, _setChild)
