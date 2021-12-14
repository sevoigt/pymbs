"""
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
"""

from pymbs.symbolics import symbolics    # C++ module
from pymbs.symbolics.symbolics import CSymbol as Variable
from pymbs.symbolics.symbolics import der, scalar

import numpy

PEDANTIC = False

class VarAlreadyExistsError(BaseException):
    def __init__(self, name):
        self.name = name
    def __str__(self):
        return self.name


class VarUnknownError(BaseException):
    def __init__(self, name):
        self.name = name
    def __str__(self):
        return self.name


class EquationAlreadyExistsError(BaseException):
    def __init__(self, lhs, rhs, eqn):
        self.lhs = lhs
        self.rhs = rhs
        self.eqn = eqn
    def __str__(self):
        return "'" + str(self.lhs) + " == " + str(self.rhs) \
                   + "' conflicts with \r\n " \
                   + "'" + str(self.eqn.lhs) + " == " \
                   + str(self.eqn.rhs) + "'!"


class ShapeError(BaseException):
    def __init__(self, lhs, lshape, rhs, rshape):
        self.lhs = lhs
        self.lshape = lshape
        self.rhs = rhs
        self.rshape = rshape
    def __str__(self):
        return "Shape mismatch '%s!=%s' at " % (str(self.lshape), 
                                                str(self.rshape)) \
                                                + str(self.lhs) + " == " \
                                                + str(self.rhs)

class VarKind:
    Variable = 0x01     # ordinary variables
    Constant = 0x02     # a variable which never change its value
    Parameter = 0x04    # parameter
    Input = 0x08        # input
    State = 0x10        # map state to generalised coordinates / 
                        # constraint forces / Lagrangian multipliers
    Der_State = 0x20    # state derivative (for internal Use only)
    User_Exp = 0x40     # expressions defined by the user
    Sensor = 0x100      # sensor
    Graphics = 0x200    # graphical sensor
    Controller = 0x400  # controller


class Assignment(object):
    """
    Create an assignment. The lhs may contain more than one variable.
    """

    def __init__(self, lhs, rhs, varKind, shape, graph):

        # support for numpy type
        if isinstance(rhs, numpy.matrix):
            rhs = rhs.tolist()

        # check right hand side
        assert isinstance(rhs, (symbolics.CBasic, list)), \
            "rhs must be a 'CBasic' not a '%s' with %s" % (str(rhs.__class__), 
                                                           str(rhs))

        if (PEDANTIC):
            # extract atoms

            if isinstance(rhs, symbolics.CBasic):
                rhs = [rhs]

            for r in rhs:
                atoms = r.atoms()

                # check if all atoms exist in graph
                for a in atoms:
                    if isinstance(a, symbolics.CSymbol):
                        assert str(a) in graph.variables, \
                            "Unknown variable '%s'" % (str(a))

        if isinstance(rhs, symbolics.CBasic):
            if (shape is None):
                rshape = graph.getShape(rhs)
            else:
                rshape = shape
        else:
            if (shape is None):
                # calculate shape
                if (len(rhs) == 1):
                    rshape = tuple()
                    if (PEDANTIC):
                        rhs = rhs[0]
                else:
                    rshape = (len(rhs),)
            else:
                rshape = shape

        if lhs is not None:
            assert isinstance(lhs,(list,str,Variable,der)), \
                   "lhs (%s) must be either list, str or variable but is %s" \
                    % (str(lhs), str(lhs.__class__))

        # is it a list?
        if (isinstance(lhs, list)):
            # create variables if neccessary
            for i in range(len(lhs)):
                if (isinstance(lhs[i], str)):
                    lhs[i] = graph.addVariable(name=lhs[i], 
                                               shape=tuple(), 
                                               varKind=varKind)
                assert isinstance(lhs[i], (Variable, der))
            # calculate shape
            if (len(lhs) == 1):
                lshape = tuple()
                lhs = lhs[0]
            else:
                lshape = (len(lhs),)
                self.lhs = lhs

        else:
            # create variable, if lhs is a string
            if (isinstance(lhs, str)):
                lhs = graph.addVariable(name=lhs, 
                                        shape=rshape, 
                                        varKind=varKind)
            if lhs is not None:
                assert isinstance(lhs, (Variable, der)), \
                       "Got lhs = %s (%s)" % (lhs, lhs.__class__)
            
            lshape = rshape

        if (lshape != rshape):
            if ((lshape == ()) and \
               ((rshape == (1,)) or (rshape == (1, 1)))) :
                rhs = scalar(rhs)
            else:
                raise ShapeError(lhs, lshape, rhs, rshape)

        self.lhs = lhs
        self.shape = rshape
        self.rhs = rhs



class Graph(object):
    """
    Interface to the C++ graph object
    """

    def __init__(self):
        """
        Constructor
        """
        if (PEDANTIC):
            self.variables = dict()
            print("Graph is in pedantic mode. May be painfully slow!!!")
        self.cgraph = symbolics.CGraph()

        #self.nGetShape = 0
        #self.nAddVariable = 0
        #self.nAddEquation = 0


    def addVariable(self, name, shape=None, varKind=VarKind.Variable, 
                          initialValue=None, comment=None, **kwargs):
        """
        Creates a new variable
        """

        #self.nAddVariable = self.nAddVariable + 1

        # shape is None => shape is scalar
        if (shape is None):
            shape = tuple()

        assert isinstance(name, str), "name must be a string"
        assert isinstance(shape, tuple), "shape must be tuple"
        assert isinstance(varKind, int), "varKind must be an integer"
        if (comment is None): comment = name
        assert isinstance(comment, str), "comment must be a string"

        if (PEDANTIC):
            # look for duplicates
            if (name in self.variables):
                raise VarAlreadyExistsError(name)

        # hand it over to the graph
        var = self.cgraph.addVariable(name, shape, varKind, 
                                      initialValue, comment)

        # add it to the list
        if (PEDANTIC):
            self.variables[str(var)] = var

        # return it
        return var


    def getVariable(self, name):
        """
        Return variable
        """
        return self.cgraph.getVariable(name)


    def getAssignments(self, kind):
        """
        Return all assignments for a specific kind (only DER_STATE or SENSOR)
        """
        assert kind in (VarKind.Der_State, VarKind.Sensor), \
               'kind must be DER_STATE or SENSOR'
        
        return self.cgraph.getAssignments(kind)


    def getVariables(self, kind):
        """
        Return variables with kind
        """
        return self.cgraph.getVariables(kind)


    def getinitVal(self, symbol):
        """
        Return variable's initial value
        """
        return self.cgraph.getinitVal(symbol)


    def addEquation(self, lhs, rhs, varKind=VarKind.Variable, 
                          shape=None, implicit=False):
        """
        Add a new Equation
        """

        #self.nAddEquation = self.nAddEquation + 1

        assert isinstance(varKind, int), \
              "varKind must be an integer, not a %s" % (str(varKind.__class__))

        # cast
        if (isinstance(rhs, (int,float))):
            rhs = symbolics.CNumber(rhs)

        # new  assignment
        ass = Assignment(lhs, rhs, varKind, shape, self)

        # push assignment to graph
        try:
            self.cgraph.addEquation(ass.lhs, ass.rhs, implicit)
        except symbolics.Error as ex:
            raise symbolics.Error("%s\nError while adding equation \
                 %s = %s, (%s = %s)" % (str(ex), 
                                        ass.lhs, 
                                        ass.rhs, 
                                        ass.lhs.__class__, 
                                        ass.rhs.__class__))
        return ass.lhs


    def getShape(self, exp):
        """
        Return the shape of an expression
        """

        # shortcut for simple types
        if (isinstance(exp, (int, float))):
            return tuple()
        if (isinstance(exp, symbolics.CBasic)):
            return exp.shape()
        else:
            raise TypeError("Class %s has no shape!" % str(exp.__class__))

        #self.nGetShape = self.nGetShape + 1
        #return self.cgraph.getShape(exp)


    def buildGraph(self, optimize=True):
        """
        Build graph with optional optimization
        """
        assert isinstance(optimize, bool), "optimize must be a bool"
        return self.cgraph.buildGraph(optimize)


    def writeCode(self, typeStr, name, path, **kwargs):
        """
        Write graph to file
        """

        assert isinstance(typeStr,str), "typeStr must be a string"
        assert isinstance(name,str), "name must be a string"
        assert isinstance(path,str), "path must be a string"

        return self.cgraph.writeOutput(typeStr, name, path, **kwargs)


    """
    def printStats(self):

        print('addVariable: %s\n \
               addEquation: %s\n \
               getShape: %s ' % (self.nAddVariable,
                                 self.nAddEquation,
                                 self.nGetShape))
    """