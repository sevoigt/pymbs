# -*- coding: utf-8 -*-
"""
Created on 05.07.2009

@author: knoll


Module containing auxillary mathematical classes.
"""


from pymbs.symbolics import Symbol, sympify, Basic

from pymbs.common.mbselement import MbsElement


# !! unify with Expr.
class Parameter(MbsElement):

    def __init__(self, longName, symbol_str, defaultValue):
        # call superclass constructor
        assert isinstance( sympify(defaultValue), Basic)
        MbsElement.__init__(self, longName)
        self.symbol=Symbol(symbol_str, real=True)
        self.defaultValue=defaultValue



