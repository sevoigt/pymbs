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


Module containing auxillary mathematical classes.

'''

from PyMbs.Symbolics import Symbol, sympify, Basic

from PyMbs.Common.MbsElement import MbsElement
from PyMbs.Common.Expression import Expression


# !! unify with Expr.
class Parameter(MbsElement):

    def __init__(self, longName, symbol_str, defaultValue):
        # call superclass constructor
        assert isinstance( sympify(defaultValue), Basic)
        MbsElement.__init__(self, longName)
        self.symbol=Symbol(symbol_str, real=True)
        self.defaultValue=defaultValue



