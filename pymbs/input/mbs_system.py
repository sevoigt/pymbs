'''
pymbs.input.MbsSystem provides the user interface for pymbs.

The task of setting up a multibody system is divided into six parts, to
which the corresponding functions are given below.

#. Initialisation

    * :class:`pymbs.input.MbsSystem`
    * :meth:`pymbs.input.MbsSystem.addParam`
    * :meth:`pymbs.input.MbsSystem.addInput`
    * :meth:`pymbs.input.MbsSystem.setGravity`

#. Create Bodies and Coordinate Systems, i.e. Frames or Connectors

    * :meth:`pymbs.input.MbsSystem.addBody`
    * :meth:`pymbs.input.MbsSystem.addFrame`

#. Connect Bodies

    * :meth:`pymbs.input.MbsSystem.addJoint`
    * :meth:`pymbs.input.MbsSystem.addConstraint`
    * :class:`pymbs.input.MbsSystem.addLoop`

#. Add Load Elements, Sensors, Expressions and Controllers

    * :class:`pymbs.input.MbsSystem.addLoad`
    * :class:`pymbs.input.MbsSystem.addSensor`
    * :meth:`pymbs.input.MbsSystem.addExpression`
    * :meth:`pymbs.input.MbsSystem.addController`

#. Add Visualisation

    * :class:`pymbs.input.MbsSystem.addVisualisation`

#. Calculate Equations of Motion and Generate Code

    * :class:`pymbs.input.MbsSystem.genEquations`
    * :class:`pymbs.input.MbsSystem.genCode`
    * :meth:`pymbs.input.MbsSystem.show`
    * :meth:`pymbs.input.MbsSystem.simulate`

In the following the members of the module pymbs.Input are explained in more
detail.
'''

import sys
import tempfile

from numpy import inf
from math import pi
import numpy as np

import pymbs.symbolics as symbolics
import pymbs.common.functions as functions

from .frame import Frame

from pymbs.symbolics import Graph, VarKind, zeros
from .body import Body, FlexibleBody
from .joint import Joint
from .loads import PtPForce, CmpForce, CmpTorque, JointLoad,\
                  Constraint#, AbstractLoadElement

from .sensors import AbstractSensor, DistanceSensor,\
                    PositionSensor, VelocitySensor,\
                    AccelerationSensor, OrientationSensor,\
                    AngVelocitySensor, AngAccelerationSensor,\
                    JointSensor, EnergySensor, \
                    ConstraintForceSensor, ConstraintTorqueSensor

import pymbs.common.graphreps as gr
from pymbs.common.functions import diag, transpose

from .constraints import FourBar, ThreeBarTrans, CrankSlider, FourBarTrans, \
                         Transmission, ExpJoint, Hexapod, Hexapod_m_AV, \
                         Steering

from pymbs.input.transformation import PublicMethods as trafo, \
                                       setMbsSystemType

from pymbs.ui.integrator import Integrator


class MbsSystem(Body):
    """
    The 'world', which is a kind of body, manages (mainly collects) all 
    these elements: bodies, joints, loads, loops, sensors, constraints and
    expressions. It gives access to the transformation module which is the 
    user interface to the functionality of pymbs.
    """
    
    def __init__(self, gravity_vect=[0,0,-1], name="world"):
        # create Frame (without position and orientation)

        cg=[0,0,0]
        # inf=float('inf') - Did not work?!, Imported from numpy
        inertia=diag([inf, inf, inf])
        Body.__init__(self, name, mass=inf, cg=cg, inertia=inertia)

        self.bodyDict={}
        self.jointDict={}
        self.loadDict={}
        self.loopDict={}
        self.sensorDict={}
        self.constraintDict={}
        self.expressionDict={}
        self.graphRepDict={}
        self.paramDict={}
        self.controllerDict={}

        # mapping from a symbol to an specific object
        self.symbolDict={}

        # save which names are already used (usefull in _checkNames)
        self.usedNameDict={}

        # save which names have been extended (and how often)
        self.extendedNamesDict={}

        # mapping from the jointVar to a 2tuple, which is the slider range
        # in the Kin-TestGUI:
        self.jointRangeDict = {}

        # used to determine the shape of Expressions
        self.graph=Graph()
        self.state=None

        self.gravity_vect=symbolics.Matrix(gravity_vect)
        # make sure that the gravity direction vector has a length of one or 
        # zero
        #  assert(self.gravity_vect.norm() in [0,1])

        self.gravity_const = self.addParam(name='gravity', symbol_str='g', 
                                           defaultValue=9.81, positive=True)
        self.time = self.graph.getVariable('time')

        self.validDofStrings = ('Tx', 'Ty', 'Tz',  'Rx', 'Ry', 'Rz')

        # submodules for sensors, load elements, loops and visualisations
        #: addSensor contains a :class:`pymbs.input.MbsSystem.AddSensor`
        #: instance, see :ref:`sensors` for the different sensors available
        #:
        #: >>> world.addSensors.Acceleration('a', CS1, CS2)
        self.addSensor = AddSensor(self)
        #: addLoad contains a :class:`pymbs.input.MbsSystem.AddLoad` instance,
        #: see :ref:`loads` for the different load types available
        #:
        #: >>> world.addLoad.PtPForce(F, body_one, body_two)
        self.addLoad = AddLoad(self)
        #: addLoop contains a :class:`pymbs.input.MbsSystem.AddLoop` instance,
        #: see :ref:`loops` for the different loops available
        #:
        #: >>> world.addLoop.FourBar(body_one, body_two)
        self.addLoop = AddLoop(self)
        #: addVisualisation contains a
        #: :class:`pymbs.input.MbsSystem.AddVisualisation` instance, see
        #: :ref:`visualisation` for the various visualisation options
        #:
        #: >>> world.addVisualisation.Box(body, length=1, width=1, height=1)
        self.addVisualisation = AddVisualisation(self)
        #: genEquations contains a :class:`pymbs.input.MbsSystem.GenEqns`
        #: instance, see :ref:`equations` for the different equation generators
        #: and their options
        #:
        #: >>> world.genEquations.Recursive()
        self.genEquations = GenEqns(self)
        #: genCode contains a :class:`pymbs.input.MbsSystem.GenCode` instance,
        #: see :ref:`code_generation` for all the writers and options available
        #:
        #: >>> world.genCode.Python()
        self.genCode = GenCode(self)
        self.counter = dict()


    def _getName(self, prefix):
        '''
        returns automatically generated name
        '''
        # increase counter
        try:
            self.counter[prefix] += 1
        except:
            self.counter[prefix] = 1
        # generate sensor name
        return '%s_%i'%(prefix,self.counter[prefix])


    def setGravity(self, value):
        '''
        *setGravity* may be used to change the value of the gravitational
        acceleration which is set by default to 9.81.

        :param value: New value of gravitational acceleration
        :type value: float or integer
        :return: Nothing

        .. note:: With this function only the value but not the direction of
                  gravity can be changed. Changing direction can be achieved
                  through the constructor of :class:`pymbs.input.MbsSystem`


        '''
        # cast to float should be possible
        assert isinstance(float(value), float)

        self.expressionDict['gravity'].exp = symbolics.sympify(value)

    def addBody(self, mass, cg=symbolics.zeros((3,)), \
                        inertia=symbolics.zeros((3,3)), \
                        name=None):
        '''
         With *addBody* you can insert a rigid body to your multibody system.
        TODO: Mentioning of body fixed frame.

        :param name: Name of the body.
        :type name: String.

        :param mass: Mass of the body in kg.
        :type mass: float/int or :class:`pymbs.Symbolics.Symbol` as returned by
                    :meth:`pymbs.input.MbsSystem.addParam`

        :param cg: Vector, describing the position of the Centre of Gravity
                   with respect to the body fixed frame.
        :type cg: 3x1 Vector (:class:`pymbs.Symbolics.Matrix` or list of float/int).

        :param inertia: Tensor of inertia at the centre of gravity for this body
                        written with respect to the body fixed frame.
        :type inertia: You can either specify a matrix (which must be symmetric)
                       or pass a 6x1 vector having the elements
                       [:math:`I_{xx}`, :math:`I_{xy}`, :math:`I_{yy}`,
                       :math:`I_{xz}`, :math:`I_{yz}`, :math:`I_{zz}`]

        :return: Reference to the generated Body object
        :rtype: :class:`pymbs.input.Body`

        .. note:: A newly generated body needs to be connected to other bodies
                  via a joint. (:meth:`pymbs.input.MbsSystem.addJoint`)

        '''
        if (name is None):
            name = self._getName('body')

        self._checkName(name)
        # TODO: asserts
        B=Body(name, mass, cg, inertia)

        if name in self.bodyDict:
            raise ValueError("Name of Body already exists")
        self.bodyDict[name]=B
        return B



    def addFlexibleBody(self,filepath, \
                        name=None):
        '''
        With *addFlexibleBody* you can insert a flexible body to your multibody system.

        :param name: Name of the body.
        :type name: String.

        :param mass: Mass of the body in kg.
        :type mass: float/int or :class:`pymbs.Symbolics.Symbol` as returned by
                    :meth:`pymbs.input.MbsSystem.addParam`

        :param cg: Vector, describing the position of the Centre of Gravity
                   with respect to the body fixed frame.
        :type cg: 3x1 Vector (:class:`pymbs.Symbolics.Matrix` or list of float/int).

        :param inertia: Tensor of inertia at the centre of gravity for this body
                        written with respect to the body fixed frame.
        :type inertia: You can either specify a matrix (which must be symmetric)
                       or pass a 6x1 vector having the elements
                       [:math:`I_{xx}`, :math:`I_{xy}`, :math:`I_{yy}`,
                       :math:`I_{xz}`, :math:`I_{yz}`, :math:`I_{zz}`]

        :return: Reference to the generated Body object
        :rtype: :class:`pymbs.input.Body`

        .. note:: A newly generated body needs to be connected to other bodies
                  via a joint. (:meth:`pymbs.input.MbsSystem.addJoint`)

        '''
        if (name is None):
            name = self._getName('flexibleBody')

        self._checkName(name)
        # TODO: asserts
        B=FlexibleBody(name,filepath)

        if name in self.bodyDict:
            raise ValueError("Name of Body already exists")
        self.bodyDict[name]=B
        return B

    def addJoint(self, CS1, CS2, dofList=None, \
                                startVals=None, startVals_d=None, \
                                name=None):
        """
        *addJoint* introduces a joint linking two bodies together. It is defined
        using two coordinate systems which define the position on each body. The
        parent coordinate system (body) should be the body which is closest to
        the inertial frame (world/MbsSystem). The movements allowed by this joint
        are defined by supplying a *dofList*. It is a list of strings where each
        element consists of the type:
        * Translational (T)
        * Rotational (R)
        and the axis of the parent coordinate system:
        * x, y, z.
        Thus a translational joint along the x-axis would be abbreviated by 'Tx'.
        A rotational joint around the z-axis would be 'Rz'. By supplying a list
        of such elementary rotations and translations more complex joints may be
        defined. A 6dof joint using cardan-angles, for example, is characterised
        by *dofList* = ['Tx', 'Ty', 'Tz', 'Rx', 'Ry', 'Rz']. Note that each
        elementary movement is carried out consecutively.
        In addition initial values, both for the position and the velocity, may
        be supplied for each elementary movement.

        :param name: Name of the joint.
        :type name: String.

        :param CS1: Parent coordinate system on the parent body.
        :type CS1: MbsSystem, Body or Coordinate System.
        :param CS2: Child coordinate system on the child body.
        :type CS2: Body or Coordinate System.

        :param dofList: List of strings describing the allowed elementary
                        movements. 'Ty' stands for translation along the y-axis.
                        'Rx' allows rotations around the x-axis.
                        dofList = [], creates a fixed joint.
        :type dofList: List of Strings.

        :param startVals: Initial position for each elementary movement.
        :type startVals: List of int/float. Dimension must be the same as *dofList*
        :param startVals_d: Initial velocity for each elementary movement.
        :type startVals_d: List of int/float. Dimension must be the same as *dofList*

        :return: Reference to the newly generated joint.
        :rtype: :class:`pymbs.input.Joint`

        .. note:: A joint may not be used to close a kinematic loop. This can only
                  be achieved by using Loops (:ref:`Kinematic-Loops`) or Constraints
                  (:meth:`pymbs.input.MbsSystem.addConstraint`)
        """
        if (name is None):
            name = self._getName('joint')

        self._checkName(name)

        # let the model autor use a body as a joint, while internally
        # use true coordsystems and not bodies

        if isinstance(CS1, Body):
            CS1 = CS1._CS_0
        if isinstance(CS2, Body):
            CS2 = CS2._CS_0

        assert type(CS1) == Frame, '%s is not a frame'%CS1
        assert type(CS2) == Frame, '%s is not a frame'%CS2

        # TODO: allow parameters (and check them)
        assert isinstance(startVals, (type(None), list, int, float))
        assert isinstance(startVals_d, (type(None), list, int, float))

        # convenience for lazy model-autors

        if isinstance(dofList , str):
            assert dofList in self.validDofStrings
            dofList=[dofList]

        if (dofList is None):
            dofList = []
        assert isinstance(dofList, list)
        assert 0 <= len(dofList) <=6

        if startVals is None:
            startVals=[0 for dof in dofList]
        if startVals_d is None:
            startVals_d=[0 for dof in dofList]

        if not isinstance(startVals , list):
            startVals=[startVals]

        if not isinstance(startVals_d , list):
            startVals_d=[startVals_d]

        for elt in dofList:
            if not elt in self.validDofStrings:
                raise ValueError("Invalid DOF-Specifier for Joint %s" % name)

        assert len(startVals) == len(dofList), 'Invalid startValue parameter for %s' % name
        assert len(startVals_d) == len(dofList)


        elemJointList = []


        if (len(dofList) == 0):
            lastDOF = None
            startVals = [0]     # just some dummy start vals
            startVals_d = [0]
        else:
            lastDOF=dofList.pop()
        startCS=CS1

        parBodyName1 = CS1.getParentBody().name
        parBodyName2 = CS2.getParentBody().name


        # template for the names of elementary joints
        # TODO: decide whether the parent body names should be
        # used here too
        # currently the name provided by the model author is used
        jointNameTpl = name + "_%s"

        # complex or elementary joint?
        if len(dofList) > 0:

            # template for dummy bodies
            dummyName="dummy_%s_%s_" % (parBodyName1, parBodyName2)


            for i, dof in enumerate(dofList):
                # create massless dummy-bodys to which the elem. joints
                # can be connected
                dummyBody=self.addBody(name=dummyName+dof,
                                         mass=0,
                                         cg=symbolics.Matrix([0,0,0]),
                                         inertia = functions.diag([0,0,0]))

                dummyBody.addFrame('CS_cg')

                targetCS = dummyBody.CS_cg
                j = self._addElementaryJoint( name=jointNameTpl % dof,
                                              CS1=startCS,
                                              CS2=targetCS,
                                              dof = dof,
                                              startVal=startVals[i],
                                              startVal_d=startVals_d[i])

                elemJointList.append(j)
                startCS=targetCS

            # End of loop: for i, dof in enumerate(dofList):

            # now create a consistent name for the last elementary joint
            nameOfLastJoint = jointNameTpl % lastDOF
        else:
            # just a elementary joint
            # -> use the name provided by model author
            nameOfLastJoint = name

        j = self._addElementaryJoint(name=nameOfLastJoint,
                                     CS1=startCS,
                                     CS2=CS2,
                                     dof = lastDOF,
                                     startVal=startVals[-1],
                                     startVal_d=startVals_d[-1])

        elemJointList.append(j)

        assert len(elemJointList) > 0

        if len(elemJointList) == 1:
            return elemJointList[0]

        else:
            # mark this name as used
            self.usedNameDict[name] = elemJointList

            return elemJointList


    def _addElementaryJoint(self, CS1, CS2, dof, \
                                startVal=None, startVal_d=None, \
                                name=None):
        """
        method that is used to create and add elementary joints (with one dof)
        """
        if (name is None):
            name = self._getName('joint')

        self._checkName(name)

        assert type(CS1) == Frame
        assert type(CS2) == Frame

        assert ((dof in self.validDofStrings) or (dof is None))

        assert isinstance(startVal, (type(None), int, float, symbolics.Symbol))
        assert isinstance(startVal_d, (type(None), int, float, symbolics.Symbol))


        # test whether there already ends a joint on the parent Body of CS2
        parentBody1 = CS1.getParentBody()
        parentBody2 = CS2.getParentBody()

        assert not parentBody1 == parentBody2

        endingJoint = parentBody2.endOfJoint
        if endingJoint is not None:
            raise ValueError("Body '%s` is already the target body of joint '%s`" \
                % ( parentBody2, endingJoint))


        J = Joint(name, CS1, CS2, dof, startVal, startVal_d)

        # TODO: add joint sensors (distance, angle)
        # TODO: add joint loads (driving force, driving torque)

        # save that Joint as endingJoint
        parentBody2.endOfJoint = J

        self.usedNameDict[name] = J
        self.jointDict[name]=J

        # range of the sliders in Kin-Test GUI (default values)
        if (dof is not None):
            if dof[0] == 'R':
                minVal = -pi
                maxVal = pi
            else:
                minVal = -1
                maxVal = 1

            self.jointRangeDict[J] =  (minVal, maxVal)

        return J

    def setJointRange(self, joint, minVal, maxVal):
        """
        Use this function to set the range in which the joint coordinate
        can be adjusted by the corresponding slider in the GUI.

        :param joint: Reference to joint
        :type joint: Joint-Object
        :param minVal: Minimum Value
        :type minVal: int/float
        :param maxVal: Maximum Value
        :type maxVal: int/float

        .. note: This option does NOT imply a hard stop of the joint regarding the simulation.
        """

        assert isinstance(joint, Joint)


        assert isinstance(minVal, (int, float, symbolics.Symbol))
        if isinstance(minVal, symbolics.Symbol):
            par = self.symbolDict.get(minVal, None)
            if par == None:
                raise ValueError("Unknown Symbol passed as minVal: %s" % minVal)
            elif not (VarKind.PARAMS and par.category):
                raise ValueError("Invalid Symbol passed as minVal: %s" % minVal\
                                + "\nValid are only Parameters")
            # use the default Value to compare:
            minVal_ = par.exp
        else:
            # use the value itself
            minVal_ = minVal

        assert isinstance(maxVal, (int, float, symbolics.Symbol))
        if isinstance(maxVal, symbolics.Symbol):
            par = self.symbolDict.get(maxVal, None)
            if par == None:
                raise ValueError("Unknown Symbol passed as maxVal: %s" % maxVal)
            elif not (VarKind.PARAMS and par.category):
                raise ValueError("Invalid Symbol passed as maxVal: %s" % maxVal\
                                + "\nValid are only Parameters")
            # use the default Value to compare:
            maxVal_ = par.exp
        else:
            # use the value itself
            maxVal_ = maxVal

        # now check if min < max

        if not minVal_ < maxVal_:
            raise ValueError("minVal < maxVal must hold!")

        self.jointRangeDict[joint] =  (minVal, maxVal)





    def addInput(self, symbol_str, shape=None, limits=None, name=None):
        """
        Add an input to the system. It may be used for values that are
        calculated externally, like tire forces. Its implementation depends
        on the format the generated code is exported. Look at the corresponding
        documentation for more information.

        :param name: Name of the input.
        :type name: String.
        :param symbol_str: Symbol that shall be used for the input.
        :type symbol_str: String.
        :param shape: Optional parameter, defining the shape of the input. Use
                     (m,n) if it is a m x n Matrix. By default (1,1), i.e.
                     a scalar input is assumed.
        :type shape: Tuple of two numbers (int/float).

        :param limits: Limits for the corresponding slider in the simulation tab.
                       Default is [-1,1]
        :type limits: List of (int/float), e.g. [-10, 10]

        :return: Symbol representing the input, which may consequently be used
                 in expressions (:meth:`pymbs.input.MbsSystem.addExpression`) and loads (ref:'Loads').
        :rtype: :class:`pymbs.Symbolics.Symbol`
        """
        if (name is None):
            name = symbol_str

        assert isinstance(name, str)
        self._checkName(name)

        assert isinstance(symbol_str, str)

        if (shape is None):
            shape = tuple()
        assert isinstance(shape, tuple) , "Input %s shape is not a tuple"%symbol_str

        if (limits is None):
            limits = [-1, 1]
        assert( isinstance(limits, list) )
        assert( len(limits) == 2 )

        # Symbol anlegen
        symbol = self.graph.addVariable(name=symbol_str, shape=shape, varKind=VarKind.Input)

        # right hand side is a zero of appropriate size
        if (len(shape) == 0):
            defaultValue = 0
        elif (len(shape) == 1):
            defaultValue = symbolics.zeros( (shape[0],1) )
        else:
            defaultValue = symbolics.zeros( shape )

        #exprSymb = self.addExpression(name=name, symbol_str=symbol, exp=defaultValue, limits=limits)
        #self.inputSymbolList.append(exprSymb)

        return symbol

    def addController(self, symbol_str, controlfunc, shape=None, name=None):
        """
        Add a controller to the system. This will call the supplied
        `controlfunc` function and store its return value, so that it can be
        used later, e.g. as a load. Because this will execute any valid
        python function, this is not available for exporting to other languages.

        :param name: Name of the controller.
        :type name: String.
        :param symbol_str: Symbol that shall be used for the controller.
        :type symbol_str: String.
        :param controlfunc: Python function taking the time and current state,
            e.g. `myfunction(t, y, sensors)`. Return value can be used e.g. as
            load.
        :type controlfunc: function
        :param shape: Optional parameter, defining the shape of the controller. Use
                     (m,n) if it is a m x n Matrix. By default (1,1), i.e.
                     a scalar input is assumed.
        :type shape: Tuple of two numbers (int/float).

        :return: Symbol representing the controller, which may consequently be
            used in expressions (:meth:`pymbs.input.MbsSystem.addExpression`)
            and loads (ref:'Loads').

        :rtype: :class:`pymbs.Symbolics.Symbol`
        """
        if (name is None):
            name = symbol_str

        assert isinstance(symbol_str, str)
        assert isinstance(name, str)
        self._checkName(name)

        if (shape is None):
            shape = tuple()
        assert isinstance(shape, tuple) , "Controller %s shape is not a tuple"%symbol_str

        # Get name of module containing control function
        fileName = controlfunc.__module__
        # Get name of controlfunc
        functionName = controlfunc.__name__
        # Join to comment, to be splitted in Symbolics module
        comment = fileName + ":" + functionName

        # Symbol anlegen
        symbol = self.graph.addVariable(name=symbol_str, shape=shape,
                                varKind=VarKind.Controller, comment=comment)

        # Sometimes vectors are input as e.g. (3,), but we want to output them
        # as (3, 1)
        if len(shape) == 1:
            self.controllerDict[symbol_str] = (shape[0], 1)
        else:
            self.controllerDict[symbol_str] = shape

        return symbol

    def addParam(self, symbol_str, defaultValue, positive=False, negative=False, name=None):
        """
        Creates a new parameter. A parameter is considered to be a value which
        is constant during simulation but may vary between different simulation
        runs. The dimension of a parameter is determined by its *defaultValue*.
        Furthermore, it is possible to supply a *positive* flag which, when set,
        allows simplifications like :math:`\\sqrt{x^2} = x` , otherwise it would
        be :math:`\\left\\vert x \\right\\vert`.

        :param name: Name of the parameter.
        :type name: String.
        :param symbol_str: Symbol that shall be used for the parameter.
        :type symbol_str: String.
        :param defaultValue: Value which is used by default. Note that this
                             option defines the dimension of this parameter.
                             The defaultValue may also be formula consisting of
                             parameters.
        :type defaultValue: int/float, list of int/float, symbolic term.
        :param positive: Will the parameter always be positive, which holds true
                         for a mass, for example.
        :type positive: Boolean.
        :returns: Symbol to the parameter.
        :rtype: :class:`pymbs.Symbolics.Symbol`
        """
        assert isinstance(symbol_str, str)
        assert defaultValue is not None
        if (name is None):
            name = symbol_str

        # !! just avoid changing all examples:
        name = name.replace(" ", "_")
        self._checkName(name)

        # symbol 'I' must not be used (Imaginary Unit)
        if (symbol_str == 'I'):
            raise AttributeError('\'I\' may not be used as a symbol, since it is used as the Imaginary Unit within pymbs.Symbolics!')

        if isinstance(defaultValue, (list, tuple)):
            L=len(defaultValue)
            assert L == 3 or L == 6, "vectors must have length 3 or 6"

            #
            suffixes=['x', 'y', 'z']

            if L == 3:
                p = symbolics.Matrix((3,))
                for v, suffix, idx in zip(defaultValue, suffixes, list(range(3))):

                    partName = "%s__%s" % (name, suffix)
                    partSymbolString = "%s_%s" % (symbol_str, suffix)

                    # create scalar Parameters for eacht component
                    p[idx]=self.addParam(name=partName, symbol_str=partSymbolString, defaultValue=v)

                # return the 3x1 matrix
                return p
            elif L == 6:

                suffixes=['xx', 'xy', 'yy', 'xz', 'yz', 'zz']
                p_temp = symbolics.zeros((L,1))
                for v, suffix, idx in zip(defaultValue, suffixes, list(range(L))):

                    partName = "%s__%s" % (name, suffix)
                    partSymbolString = "%s_%s" % (symbol_str, suffix)

                    # create scalar Parameters for eacht component
                    p_temp[idx]=self.addParam(name=partName, symbol_str=partSymbolString, defaultValue=v)

                return functions.symmetricMatrix(p_temp)


        # End of <if isinstance(defaultValue, (list, tuple)):>

        if isinstance(defaultValue, symbolics.Matrix):
            nr, nc = defaultValue.shape

            if defaultValue.shape == (3,3):
                suffixes=["xx", "xy", "xz", "yx", "yy", "yz", "zx", "zy", "zz"]
            elif defaultValue.shape == (3,1):
                suffixes=["x", "y", "z"]
            else:
                # create the crossproduct of the two index-sets:
                suffixes=["__%d_%d" % (a,b) for a in range(nr) for b in range(nc)]


            p = symbolics.zeros((nr,nc))
            defaultValueList = defaultValue[:]# matrix to list
            for v, suffix, idx in zip(defaultValueList,
                                         suffixes, list(range(nr*nc))):

                partName = "%s__%s" % (name, suffix)
                partSymbolString = "%s_%s" % (symbol_str, suffix)

                # create scalar Parameters for each component
                # (the matrix p can still be accessed by a single index)
                p[idx]=self.self.addParam(name=partName, symbol_str=partSymbolString, defaultValue=v)

            # return the 3x3 matrix
            return p
        # End of: if isinstance(defaultValue, Symbolics.Matrix):

        try: # it might happen that things like a*b<0 throw an exception
            if (positive and negative):
                raise AttributeError('Parameter "%s" cannot be positive and negative at the same time'%name)

            if ((positive==True) and (defaultValue < 0)):
                raise AttributeError('Parameter "%s" cannot be positive, if a default ' + \
                                     'value of %f has been specified!'%(name,defaultValue))
            if ((negative==True) and (defaultValue > 0)):
                raise AttributeError('Parameter "%s" cannot be negative, if a default ' + \
                                     'value of %f has been specified!'%(name,defaultValue))
        except TypeError:
            pass

        paramSymbol = self.graph.addVariable(name=symbol_str, shape=tuple(), varKind=VarKind.Parameter, positive=positive, negative=negative)
        self.graph.addEquation(lhs=paramSymbol, rhs=defaultValue)
        self.paramDict[symbol_str] = defaultValue

        return paramSymbol

    def _addLoad(self, typeString, loadSymb, CS1, CS2, CSref=None, name=None):
        """
        Internal function to add a 'PtPForce', 'CmpForce' or 'CmpTorque'
        It is called by addLoad.PtPForce, addLoad.CmpForce ...
        """

        if (name is None):
            name = self._getName('load')

        assert isinstance(typeString, str)
        assert isinstance(name, str)
        self._checkName(name)

        if isinstance(CS1, Body):
            CS1 = CS1._CS_0
        if isinstance(CS2, Body):
            CS2 = CS2._CS_0
        if isinstance(CSref, Body):
            CSref = CSref._CS_0
        if isinstance(loadSymb, list):
            loadSymb = self.addExpression(symbol_str=name+'_load', exp=symbolics.Matrix(loadSymb))


        assert CS1.__class__ == Frame
        assert CS2.__class__ == Frame, "CS2 should be a Frame but is a '%s' with '%s'"%(str(CS2.__class__), str(CS2))

        assert isinstance(loadSymb, symbolics.Symbol)
        assert CSref.__class__ in  (Frame, type(None))


        if typeString not in ['PtPForce', 'CmpForce', 'CmpTorque']:
            raise ValueError("Invalid force type specifier: %s"\
                                                    % typeString)

        if typeString == "PtPForce":
            # CSref does not make sense in PtP-case
            assert CSref is None
            #assert loadExpr.shape == (1,1)
            assert len(loadSymb.shape()) == 0
            loadObject=PtPForce(name, CS1, CS2, loadSymb)

        elif typeString == "CmpForce":
            assert loadSymb.shape() == (3,) , "Shape %s is wrong for %s"%(loadSymb.shape(),name)
            loadObject=CmpForce(name, CS1, CS2, loadSymb, CSref)

        elif typeString == "CmpTorque":
            assert loadSymb.shape() == (3,), "Shape must be (3,) but it is %s"%str(loadSymb.shape)
            loadObject=CmpTorque(name, CS1, CS2, loadSymb, CSref)


        if name in self.loadDict:
            raise ValueError('Force name %s already exists'\
                                                        % name)

        self.loadDict[name]=loadObject

        # return nothing

    def addJointLoad(self, loadSymb, thejoint, name=None):
        """
        Adds a load object to the joint.
        The load is a force or a torqe, depending on the type of the joint

        :returns: None

        .. note: This has not been tested yet!
        """
        if (name is None):
            name = self._getName('load')

        self._checkName(name)
        assert isinstance(thejoint, Joint),\
            "The passed joint object has the wrong type: %s" % type(thejoint)

        # Falls wir fuer die JointLoad einen Ausdruck aber kein Symbol erhalten,
        # dann neue temporaere Variable anlegen
        if (isinstance(loadSymb, symbolics.Basic) and not isinstance(loadSymb, symbolics.Symbol)):
            loadSymb = self.addExpression(symbol_str=name+'_load', exp=loadSymb)

        assert isinstance(loadSymb, symbolics.Symbol),\
            "loadSymb must be a Symbol, not a %s" % type(loadSymb)

        assert thejoint.jointLoad == None,\
            "The passed joint object already has a jointLoad"

        loadObject = JointLoad(name, thejoint, loadSymb)
        thejoint.jointLoad = loadObject
        self.loadDict[name] = loadObject
        self.usedNameDict[name] = loadObject


    def addExpression(self, symbol_str, exp, name=None, category=VarKind.User_Exp,
                      positive=False, negative=False, limits=None):
        """
        Add an expression to the system of equations. Expressions allow the user
        to include calculations of, say, forces from sensors and inputs, or to
        define own sensors. All symbols known to pymbs may be used in such an
        expression.

        :param name: Name of the expression.
        :type name: String.
        :param symbol: Symbol that shall be used for the parameter.
        :type symbol: String.
        :param exp: Term to calculate the expression, like c*x[0]+d*x[1]
        :type exp: :class:`pymbs.Symbolics.Basic`
        :param category: (Optional) Each expression may be categorised. Use
                        VarKind.Sensor, to let pymbs recognise it as a sensor,
                        for example.
        :type category: (List of) Strings.
        :param positive: (Optional) Let pymbs.Symbolics assume that it is positive, which
                         makes it easier to simplify.
        :type positive: Boolean.

        :returns: Symbol of the expression.
        :rtype: :class:`pymbs.Symbolics.Symbol`

        """
        # category is not an argument, because the user
        # should not be able to easily add an expr. with wrong CAT
        if (name is None):
            name = str(symbol_str)


        assert isinstance(name, str)
        self._checkName(name)

        assert isinstance(symbol_str, str)

        # TODO: getShape fuer eine Expression
        if (isinstance(exp,list)):
            exp = symbolics.Matrix(exp)
        if (isinstance(exp,(float,int))):
            shape  = ()
        else:
            shape = exp.shape()

        symbol = self.graph.addVariable(symbol_str, shape=shape, varKind=category, pos=positive, neg=negative)
        assert isinstance(exp, (int, float, symbolics.Basic, symbolics.Matrix, list))

        assert( (limits is None) or isinstance(limits, list) )
        if (isinstance(limits, list)):
            assert( len(limits) == 2 )

        # let the shape be set:
        if (isinstance(exp, list)):
            exp = symbolics.Matrix(exp)
        if (not isinstance(exp, (symbolics.Basic, symbolics.Matrix, float, int))):
            exp = symbolics.sympify(exp)
        self.graph.addEquation( lhs=symbol, rhs=exp  )

        return symbol


    def _addLoop(self, name, typeString, *args):

        self._checkName(name)

        if name in self.loopDict:
            raise ValueError("Name of Loop already exists: %s" % name)

        if (typeString == "FourBar"):
            assert(len(args) in (2,3))
            loop=FourBar(name, *args)
        elif (typeString == "ThreeBarTrans"):
            assert(len(args) == 2)
            loop=ThreeBarTrans(name, *args)
        elif (typeString == "Steering"):
            assert(len(args) in (2,3,4))
            loop=Steering(name, *args)
        elif (typeString == "CrankSlider"):
            assert(len(args) == 2)
            loop=CrankSlider(name, *args)
        elif (typeString == "FourBarTrans"):
            assert(len(args) in (2,3))
            loop=FourBarTrans(name, *args)
        elif (typeString == "Transmission"):
            assert(len(args) in (2,3))
            loop=Transmission(name, *args)
        elif (typeString == "ExpJoint"):
            assert(len(args) == 2)
            loop=ExpJoint(name, *args)
        elif (typeString == "Hexapod"):
            assert(len(args) == 2)
            loop=Hexapod(name, *args)
        elif (typeString == "Hexapod_m_AV"):
            assert(len(args) == 2)
            loop=Hexapod_m_AV(name, *args)

        else:
            raise ValueError('Unknown Loop: %s'%typeString)

        self.loopDict[name]=loop

    def addConstraint(self, CS1, CS2, transLock=[0,0,0], rotLock=[0,0,0], name=None):
        """
        One way of closing kinematic loops is through the use constraints. This
        will introduce constraint forces at both coordinate systems as well as
        a set of constraint equations inhibiting certain relative movements which
        are specified by *transLock* and *rotLock*.
        This will ultimately lead to formulation as a DAE (Differential Algebraic
        Equations).

        :param name: Name of the constraint.
        :type name: String.

        :param CS1: Parent coordinate system on the parent body.
        :type CS1: MbsSystem, Body or Coordinate System.
        :param CS2: Child coordinate system on the child body.
        :type CS2: MbsSystem, Body or Coordinate System.

        :param transLock: Vector [x,y,z] that defines which relative
                          translations are forbidden. Use 0 to allow relative
                          motion along the corresponding axis and 1 to inhibit it.
        :param rotLock: Vector [x,y,z] that defines which relative
                        rotations are forbidden. Use 0 to allow relative
                        motion around the corresponding axis and 1 to inhibit it.

        .. note: The axis x,y,z are taken from the parent coordinate system *CS1*

        """
        if (name is None):
            name = self._getName('constraint')

        self._checkName(name)

        if isinstance(CS1, Body):
            CS1 = CS1._CS_0
        if isinstance(CS2, Body):
            CS2 = CS2._CS_0

        assert CS1.__class__ == Frame
        assert CS2.__class__ == Frame

        assert isinstance(CS1, Frame)
        assert isinstance(CS2, Frame)

        assert isinstance(transLock, (list, symbolics.Matrix))
        assert isinstance(rotLock, (list, symbolics.Matrix))

        transLock=symbolics.Matrix(transLock)
        rotLock=symbolics.Matrix(rotLock)

        # TODO: check whether the the constraint is meaningful
        # CS1.parentBody != CS2.parentBody

        constr = Constraint(name, CS1, CS2, transLock, rotLock)

        self.usedNameDict[name] = constr
        self.constraintDict[name] = constr

        # return nothing





    def _addSensor(self, typeString, symb, CS1, CS2, CSref=None, name=None, category = VarKind.Sensor):

        if (name is None):
            name = str(symb)

        if isinstance(CS1, Body):
            CS1 = CS1._CS_0
        if isinstance(CS2, Body):
            CS2 = CS2._CS_0
        # TODO: is this desired behaviour? (conflicts with one test)
        if isinstance(CSref, Body):
            CSref = CSref._CS_0

        assert CS1.__class__ == Frame
        assert CS2.__class__ == Frame

        assert isinstance(name, str)
        self._checkName(name)
        assert isinstance(typeString, str)
        assert isinstance(CS1, Frame)
        assert isinstance(CS2, Frame)
        assert isinstance(symb, str)
        assert isinstance(category, int)

        assert CSref.__class__ in  (type(None), Frame)

        # shortcut to elementFunction:

        elem=functions.element

        if typeString == "Distance":
            symbol = self.graph.addVariable(name=symb, shape=(2,), varKind=category)
            newSensor = DistanceSensor(name, CS1, CS2, symbol, CSref, category)
            # Distance is 2x1 Vector (Distance, Velocity better when used for SpringDampers)

        elif typeString == "Position":
            symbol = self.graph.addVariable(name=symb, shape=(3,), varKind=category)
            newSensor = PositionSensor(name, CS1, CS2, symbol, CSref, category)

        elif typeString == "Velocity":
            symbol = self.graph.addVariable(name=symb, shape=(3,), varKind=category)
            newSensor = VelocitySensor(name, CS1, CS2, symbol, CSref, category)

        elif typeString == "Acceleration":
            symbol = self.graph.addVariable(name=symb, shape=(3,), varKind=category)
            newSensor = AccelerationSensor(name, CS1, CS2, symbol, CSref, category)

        elif typeString == "Orientation":
            symbol = self.graph.addVariable(name=symb, shape=(3,3), varKind=category)
            newSensor = OrientationSensor(name, CS1, CS2, symbol, CSref, category)

        elif typeString == "AngularVelocity":
            symbol = self.graph.addVariable(name=symb, shape=(3,), varKind=category)
            newSensor = AngVelocitySensor(name, CS1, CS2, symbol, CSref, category)

        elif typeString == "AngularAcceleration":
            symbol = self.graph.addVariable(name=symb, shape=(3,), varKind=category)
            newSensor = AngAccelerationSensor(name, CS1, CS2, symbol, CSref, category)

        else:
            raise ValueError("Invalid typeString for Sensor %s: %s" % (name,typeString))

        if name in self.sensorDict:
            raise ValueError("Name of Sensor already exists: %s").with_traceback(name)

        self.sensorDict[name]=newSensor
        self.usedNameDict[name]=newSensor

        return symbol


    def _addJointSensor(self, symbstr, thejoint, name=None, category=VarKind.Sensor):
        """adds a special sensor which can be used to measure
        the motion quantities of a joint, i.e. the 2x1 Matrix
        [joint.q; joint.qd]
        """
        if (name is None):
            name = symbstr

        self._checkName(name)

        assert isinstance(thejoint, Joint), "Expected Joint but got %s (%s)"%(thejoint, thejoint.__class__)
        assert isinstance(symbstr, str)

        assert thejoint.jointSensor == None,\
            "The passed joint object already has a jointSensor"

        assert isinstance(category, int)

        symb = self.graph.addVariable(name=symbstr, shape=(2,), varKind=category)

        # check, that the symbols parent already used
        assert not symb in list(self.symbolDict.keys())

        newSensor = JointSensor(name, thejoint, symb)

        # mark the jointSensorSlot as used
        thejoint.jointSensor = newSensor

        # !! CodeDuplication ;(
        self.sensorDict[name]=newSensor
        self.usedNameDict[name]=newSensor
        #self.symbolDict[symb]=newSensor

        elem=functions.element # convenience
        retObject = symbolics.Matrix([elem(symb,0,0),
                               elem(symb,1,0)])

        return retObject


    def _addJointConstraintSensor(self, typeString, symbstr, thejoint, name=None, category=VarKind.Sensor):
        """adds a special sensor which can be used to measure
        the joints constraint forces or torques
        """

        if (name is None):
            name = symbstr

        self._checkName(name)

        assert isinstance(thejoint, Joint)
        assert isinstance(symbstr, str)
        assert isinstance(typeString, str)

        assert isinstance(category, int)

        symb = self.graph.addVariable(name=symbstr, shape=(3,), varKind=category)

        # check, that the symbols parent already used
        assert not symb in list(self.symbolDict.keys())

        if (typeString == "Force"):
            newSensor = ConstraintForceSensor(name, thejoint, symb, category)
        elif (typeString == "Torque"):
            newSensor = ConstraintTorqueSensor(name, thejoint, symb, category)
        else:
            raise ValueError("Invalid typeString for Sensor %s: %s" % (name, typeString))

        # !! CodeDuplication ;(
        self.sensorDict[name]=newSensor
        self.usedNameDict[name]=newSensor
        self.symbolDict[symb]=newSensor


        elem=functions.element # convenience
        retObject = symbolics.Matrix([elem(symb,0,0),
                               elem(symb,1,0),
                               elem(symb,2,0)])

        return retObject

    def _addEnergySensor(self, symbstr, body, name=None, category=VarKind.Sensor):
        """adds a special sensor which measures
           the potential as well as the kinetic energy of a body
        """
        if (name is None):
            name = symbstr

        self._checkName(name)

        assert isinstance(body, Body), 'body is not of type Body, but %s (%s)'%(body,body.__class__)
        assert isinstance(symbstr, str)

        assert isinstance(category, int)

        symb = self.graph.addVariable(name=symbstr, shape=(), varKind=category)

        # check, that the symbols parent already used
        assert not symb in list(self.symbolDict.keys())
        newSensor = EnergySensor(name, body, symb)

        # !! CodeDuplication ;(
        self.sensorDict[name]=newSensor
        self.usedNameDict[name]=newSensor

        return symb


    def addGraphicSensors(self, cs, name=None, category = VarKind.Sensor):
        '''
        You may use this function to manually add a Position and a Orientation
        sensor for a Coordinate System.

        :param name: Name of the visualisation sensors (r\_ and T\_ are used as
                    prefix for the position and orientation sensor, respectively)
        :type name: String.
        :param cs: Coordinate System from which the position and orientation
                   shall be measured.
        :type cs: MbsSystem, Body, Frame.
        :param category: May be used to manually override the category \(not recommended\)
        :type category: list of Strings.
        :returns: list of symbols for position and orientation.
        :rtype: list of :class:`pymbs.Symbolics.Symbol`

        .. note: If you use addVisualisation, then these sensors are added
                 automatically.
        '''
        if (name is None):
            name = self._getName('sensor')

        assert(isinstance(name, str))
        assert(isinstance(cs, Frame))

        world = self

        # Position
        posSensVect = self.addSensor.Position("r_%s"%name, world, cs, world, "r_%s"%name, category)
        # Orientation
        oriSensMatr = self.addSensor.Orientation("T_%s"%name, world, cs, "T_%s"%name, category)

        # return the two symbols

        posSymbol=posSensVect
        oriSymbol=oriSensMatr

        return posSymbol, oriSymbol


    def _addVisualisation(self, name, csList, typeString, *args, **kwargs):
        """Adds a visualization Object to a Frame

        name:         name of the visual shape (also used for corresponding sensors)

        cs:            the coordinate system

        typestring:    one of:
                       'file', 'box', 'cylinder', 'sphere', 'line', 'coordsys', 'flexible_Body'

       *args:          depending of typestring
        """

        assert(isinstance(name, str))

        if not typeString in ('file', 'box', 'cylinder', 'sphere', 'line', 'coordsys','arrow','flexible_Body'):
            raise ValueError('Invalid typeString: %s' % typeString)

        # save the frame:"csList" in a list
        pre_csList = list()
        pre_csList.append(csList)
        csList = pre_csList


        if typeString == 'flexible_Body':

            assert len(args) == 1, 'expected exactly one additional '\
                                    'argument: flex_Body\n'\
                                    'got: %s' % str(args)

            if not csList[0].name == 'node_1':
                raise NotImplementedError('flexible Body should be attached to node_1')

            if len(args) == 1:
                flex_Body = args[0]
                assert isinstance(flex_Body,FlexibleBody), 'expected a '\
                                                            'flexibleBody '\
                                                            'as input\n'\
                                                            'got: %s'% str(type(flex_Body))
            # get the node-frames of the flexible body
            node_List = flex_Body.node_list
            csList = list()
            nodePositions = list()
            for node in node_List:
                cs = node.frame
                csList.append(cs)

                position = cs.p
                nodePositions.append(position)


        # create empty lists
        sensPos = [None]*len(csList)
        sensOri = [None]*len(csList)
        # run through all elements in csList
        for i in range(len(csList)):
            cs = csList[i]

            if (isinstance(cs, (Body, FlexibleBody))):
                cs=cs._CS_0
                csList[i] = cs

            assert type(cs) == Frame


            # create a unique name for the GraphRep
            #parentBody = cs.getParentBody()
            #basename = parentBody.name + '_' + cs.name
            #name = self._extendName(basename) # append a number in case it is needed

            #print name
            self._checkName(name)

            if typeString == 'flexible_Body':
               name += "_%i"%i

            sensPos[i], sensOri[i] = self.addGraphicSensors(cs=cs, name=name, category=VarKind.Graphics)

        # convert form tuple to list to have pop()
        args=list(args)

        # TODO: check (the Graph) whether the symbols have the correct dim
        # maybe a helperFunction: self._isscalar

        if typeString == 'file':
            assert (len(csList) == 1), 'File Graphreps must have one frame'
            assert (len(args) == 1) or (len(args) == 2), \
                                    'expected one or two additional '\
                                    'argument: path_to_file (, scale)'

            pathToFile=args[0]
            assert isinstance(pathToFile, str), 'path_to_file must be '\
                                   'a string, not a %s' % type(pathToFile)
            if (len(args) > 1):
                scale = args[1]
                assert self._isscalar(scale)
                #assert isinstance(scale, (int,float, Symbolics.Symbol)), \
                'scale must be an int, float or a Symbol, not a %s' % type(scale)

            graphRep = gr.File(name, sensPos, sensOri, *args)

        elif typeString == 'box':
            assert (len(csList) == 1), 'box Graphreps must have one frame'
            assert len(args) == 3, 'expected exactly three additional '\
                                    'arguments: lx, ly, lz\n'\
                                    'got: %s' % str(args)
            lx, ly, lz = args

            assert self._isscalar(lx) #isinstance(lx, (int, float, Symbolics.Symbol))
            assert self._isscalar(ly) #isinstance(ly, (int, float, Symbolics.Symbol))
            assert self._isscalar(lz) #isinstance(lz, (int, float, Symbolics.Symbol))

            graphRep = gr.Box(name, sensPos, sensOri, lx, ly, lz)

        elif typeString == 'cylinder':
            assert (len(csList) == 1), 'cylinder Graphreps must have one frame'
            assert len(args) in (2, 3), 'expected two or three additional '\
                                    'arguments: radius, l, (res)\n'\
                                    'got: %s' % str(args)

            if len(args) == 3:
                res = args.pop()
                assert self._isscalar(res) #isinstance(res, (int, float, Symbolics.Symbol))
            else:
                res = None # use the default

            radius, l = args

            graphRep = gr.Cylinder(name, sensPos, sensOri, radius, l, res)


        elif typeString == 'sphere':
            assert (len(csList) == 1), 'sphere Graphreps must have one frame'
            assert len(args) in (1, 2), 'expected one or two additional '\
                                    'argument(s): radius, (res)\n'\
                                    'got: %s' % str(args)

            if len(args) == 2:
                res = args.pop()
                assert self._isscalar(res) #isinstance(res, (int, float, Symbolics.Symbol))
            else:
                res = None # use the default

            radius = args[0]

            graphRep = gr.Sphere(name, sensPos, sensOri, radius, res)


        elif typeString == 'line':
            assert (len(csList) == 1), 'line Graphreps must have one frame'
            assert len(args) == 1, 'expected exactly one additional '\
                                    'arguments: L\n'\
                                    'got: %s' % str(args)
            L = args[0]
            assert self._isscalar(L) #isinstance(L, (int, float, Symbolics.Symbol))

            graphRep = gr.Line(name, sensPos, sensOri, L)

        elif typeString == 'coordsys':
            assert (len(csList) == 1), 'coordsys Graphreps must have one frame'
            assert len(args) == 1, 'expected exactly one additional '\
                                    'argument: size\n'\
                                    'got: %s' % str(args)

            size = args[0]
            assert self._isscalar(size) #isinstance(size, (int, float, Symbolics.Symbol))

            graphRep = gr.Frame(name, sensPos, sensOri, size)

        elif typeString == 'arrow':
            assert (len(csList) == 1), 'arrow Graphreps must have one frame'
            assert len(args) == 1, 'expected exactly one additional '\
                                    'argument: size\n'\
                                    'got: %s' % str(args)

            size = args[0]

            graphRep = gr.Arrow(name, sensPos, sensOri, size)

        elif typeString == 'flexible_Body':

                points = list()
                for position in nodePositions:
                    position_as_tuple = position[0],position[1],position[2]
                    points.append(position_as_tuple)

                graphRep = gr.Flexible_Body(name, sensPos, sensOri, points)

        else:
            # we should not end up here
            raise ValueError('Ivalid typeString')

        graphRep.color = kwargs.get('color')

        self.usedNameDict[name] = graphRep
        self.graphRepDict[name] = graphRep


    def addMotionSystemSensors(self, CS_D, name="BWS"):
        """
        Add all sensors that are needed for the motion platform, i.e.
        * Position
        * Orientation
        * Velocity
        * Angular Velocity
        * Acceleration
        * Angular Acceleration

        :param CS_D: Coordinate System where the driver sits
        :type CS_D: Body, Coordinate System
        """

        assert(isinstance(name, str))
        assert(isinstance(CS_D, Frame))

        world = self

        # Position
        self.addSensor.Position("r_%s"%name, world, CS_D, name="r_%s"%name)
        # Orientation
        self.addSensor.Orientation("T_%s"%name, world, CS_D, name="T_%s"%name)

        # Velocity
        self.addSensor.Velocity("v_%s"%name, world, CS_D, CSref=CS_D, name="v_%s"%name)
        self.addSensor.AngularVelocity("om_%s"%name, world, CS_D, CSref=CS_D, name="om_%s"%name)

        # Acceleration
        self.addSensor.Acceleration("a_%s"%name, world, CS_D, CSref=CS_D, name="a_%s"%name)
        self.addSensor.AngularAcceleration("dom_%s"%name, world, CS_D, CSref=CS_D, name="dom_%s"%name)

    def addTyreSensorsLoads(self, name, CS_C, CS_R):
        """
        Add all sensors and force elements needed for a tire model:
        * Position
        * Orientation
        * Velocity
        * Angular Velocity
        * CmpForce with 3x1 vector input
        * CmpTorque with 3x1 vector input

        :param name: Name of this sensors (using r\_, T\_, v\_, om\_ as prefixes)
        :param CS_C: Carrier-Frame - Wheel centre, attached to the vehicle
                    (does not rotate with the wheel)
        :type CS_C: Body or Coordinate System
        :param CS_R: Rim-Frame - Wheel centre, attached to the wheel
                    (does rotate with the wheel)
        """
        assert(isinstance(name, str))
        assert(isinstance(CS_C, Frame))
        assert(isinstance(CS_R, Frame))

        world = self

        # Position
        self.addSensor.Position("r_%s"%name, world, CS_C, name="r_%s"%name)
        self.addSensor.Orientation("T_%s"%name, CS_C, world, name="T_%s"%name)
        # Velocity
        self.addSensor.Velocity("v_%s"%name, world, CS_C, CSref=CS_C, name="v_%s"%name)
        self.addSensor.AngularVelocity("om_%s"%name, world, CS_C, CSref=CS_C, name="om_%s"%name)

        # Inputs
        F = self.addInput("F_%s"%name, (3,))
        T = self.addInput("M_%s"%name, (3,))

        # Force and Torque
        self.addLoad.CmpForce(F, CS_R, world, CSref=CS_C, name="CmpForce_%s"%name)
        self.addLoad.CmpTorque(T, CS_R, world, CSref=CS_C, name="CmpTorque_%s"%name)

    def addHydCylSensorsLoads(self, name, CS1, CS2):
        """
        Add all Sensors and Load Elements needed for a hydraulic cylinder:
        * DistanceSensor (distance and velocity at which the distance changes)
        * PtP-Force with scalar input

        :param CS1: Coordinate System at the bottom of a hydraulic cylinder
        :type CS1: Body or Coordinate System
        :param CS2: Coordinate System at the end of the rod
        :type CS2: Body or Coordinate System
        """

        assert( isinstance(name, str) )
        assert( isinstance(CS1, Frame) )
        assert( isinstance(CS2, Frame) )

        self.addSensor.Distance("d_%s"%name, CS1, CS2, name="d_%s"%name)
        F = self.addInput("F_%s"%name, shape=None)
        self.addLoad.PtPForce(F, CS1, CS2, name="PtPForce_%s"%name)

    def addContactSensorsLoads(self, name, CS_C):
        """
        Add all sensors and force elements needed for a contact:
        * Position
        * Orientation
        * Velocity
        * Angular Velocity
        * CmpForce with 3x1 vector input
        * CmpTorque with 3x1 vector input

        :param name: Name of this sensors (using r\_, T\_, v\_, om\_ as prefixes)
        :param CS_C: Contact-Frame
        :type CS_C: Body or Coordinate System
        """

        assert(isinstance(name, str))
        assert(isinstance(CS_C, Frame))

        world = self

        # Position
        self.addSensor.Position("r_%s"%name, world, CS_C, name="r_%s"%name)
        self.addSensor.Orientation("T_%s"%name, world, CS_C, name="T_%s"%name)
        # Velocity
        self.addSensor.Velocity("v_%s"%name, world, CS_C, name="v_%s"%name)
        self.addSensor.AngularVelocity("om_%s"%name, world, CS_C, name="om_%s"%name)

        # Inputs
        F = self.addInput("F_%s"%name, (3,))
        T = self.addInput("M_%s"%name, (3,))

        # Force and Torque
        self.addLoad.CmpForce(F, CS_C, world, CSref=world, name="CmpForce_%s"%name)
        self.addLoad.CmpTorque(T, CS_C, world, CSref=world, name="CmpTorque_%s"%name)

    def _checkSymbol(self, symbol):
        """
        raises an exception if the symbol already exists
        """
        assert isinstance(symbol, symbolics.Symbol)

        if symbol in self.symbolDict:
            raise ValueError("Symbol %s already exists" % symbol)

    def _extendName(self, name):
        """Auxiliary method.
        If the name was already used it appends a number to make it unique
        if not, name is returned without change
        """

        assert isinstance(name, str)

        if name in self.usedNameDict:
            nbr = self.extendedNamesDict.get(name, 0) + 1
            self.extendedNamesDict[name] = nbr

            return "%s_%d" % (name, nbr)
        else:
            return name

    def _checkName(self, nameStr):
        """
        checks if nameStr is a valid name
        """

        assert isinstance(nameStr, str), '%s is not a string!'%nameStr

        # idea: everything, that is could be a python identifier
        # is a valid name

        testSource = "%s = 5" % nameStr

        try:
            # will raise a SyntaxError if nameStr is no valid identifier
            compile(testSource, '<no source file>', 'single' )
        except SyntaxError:
            raise ValueError("'%s´ is an invalid name" % nameStr)

        # TODO: fill that dict in the appr. methods
        if nameStr in self.usedNameDict:
            raise ValueError("Name '%s´ is already used by %s"\
                                     % (nameStr, self.usedNameDict[nameStr]))

    def _isscalar(self, arg):
        """
        check whether arg is an int or float instance or
        a known symbol of a scalar expression
        """
        if isinstance(arg, (int, float)):
            return True

        if isinstance(arg, symbolics.Symbol):
            # test whether the symbol is known by Graph
            try:
                shape = self.graph.getShape(arg)

            except ValueError:
                raise ValueError("unknown Symbol")

            # Shape==(0,0) represents a scalar symbol
            if shape == (0, 0):
                return True

            else:
                return False

        if isinstance(arg, symbolics.Matrix):
            return False

        raise TypeError("arg has an unexpected type: %s (not int, float, Symbol)"\
                % type(arg))


    # shortcuts to the public Functions of the transformation module
    # most of them need the MbsSystem instance (self)to be passed as argument

    def _genEquations(self, mode=None, diff=0, kinematicsOnly=False, \
                     simplify=True, graphOptimizations=False):
        '''
        This function derives equations of motion for the given system. It
        should be run after the system has been assembled, i.e. all bodies,
        joints, load elements and sensors are defined.

        Currently there are two schemes to generate the equations:
        Explicit and Recursive (see :ref:`Background`).
        The first can be chosen by setting *explicit* to true while the
        recursive scheme is used if *explicit* is false.
        The explicit scheme is only recommended for small systems with up to
        three degree of freedoms. If the mechanism is planar, then it might be
        suitable for up to five degrees of freedom.

        :param explicit: If false, then the recursive scheme is used otherwise
                         the equations of motion will be derived explicitly.
        :type explicit: Boolean
        :param diff: With diff the user can specify how often the algebraic
                     constraints (on position level) shall be differentiated
                     with respect to time (if there are any kinematic loops
                     present). Valid values are:
                     * 0 - position level
                     * 1 - velocity level
                     * 2 - acceleration level
                     If you need the constraints on more than one level, than
                     a list of may also be supplied, i.e. [0, 1, 2]
        :type diff: list of int
        :param kinematicsOnly: If set to true, no equations for forces and
                               accelarations are generated, only position and
                               velocities are available
        :type kinematicsOnly: Boolean
        :param simplify: If set to true, simplification of expression is
                         performed which can be very time consuming.
        :type simplify: Boolean
        :param graphOptimizations: If set to False, Graph simplification is skipped.
        :type graphOptimizations: Boolean

        '''
        # CS 27.06.2011, En-/Disable simplify
        assert isinstance(mode,str)
        import pymbs.common.simple
        pymbs.common.simple.DO_NOT_SIMPLIFY = not simplify

        return trafo.genEquations(self, mode, diff, kinematicsOnly,
                graphOptimizations)


    def show(self, modelname=None, options={}, **kwargs):
        '''
        Produces a GUI (graphical user interface) in which your model will be
        displayed as long as you provided graphic objects using
        :meth:`pymbs.input.MbsSystem.addVisualisation`.
        Next to the model there will be sliders with which you can manipulate
        each joint coordinate directly and thus test your assembly.

        .. note::

            You must call :meth:`pymbs.input.MbsSystem.genEquations`
            before you may call show!

        :param modelname: Name of the model - will be displayed in the title bar.
        :type modelname: String.

        :param options: used to customise some gui features. Example showing
                the default values of the keys

                >>> options = {'qFlexScaling':20,
                               'qTransRange' :[-3, 3],
                               'qRotRange':[-3.14, 3.14]}

                  qFlexScaling - a factor to scale the state of a flexible body

                  qTransRange - 2-list or tuple setting the limits of
                  translational / prismatic joints values are interpreted as
                  SI-unit [m]

                  qRotRange - dito for rotational /revoule joints values are
                  interpreted as SI-unit [rad]

        :type options: Dict.
        '''

        if (modelname is None):
            modelname = 'pymbsModel'

        # make sure that there is no space in the modelname
        testSource = "%s = 0" % modelname
        try:
            # will raise a SyntaxError if nameStr is no valid identifier
            compile(testSource, '<no source file>', 'single' )
        except SyntaxError:
            print('"%s" is not a valid modelname. It must not contain any spaces or umlauts!'%modelname)
            return

        trafo.show(self, modelname, options, **kwargs)

    def simulate(self, q0, t, modelname=None, text_output=False):
        '''
        Simulates the motion of your system with the initial conditions given
        by `q0` up to the time `t` and outputs the coordinates to a file.

        :param q0: Initial conditions
        :type q0: List

        :param t: Length of simulation
        :type t: Float

        :param modelname: Name of the model - will be used as output filename
        :type modelname: String.

        :param text_output: Set to `True` for CSV output instead of binary
            numpy file
        :type text_output: Boolean
        '''

        if modelname is None:
            modelname = 'pymbsModel'

        _Graph = self.graph
        _Graph.writeCode('py', modelname, tempfile.gettempdir())

        sys.path.insert(0, tempfile.gettempdir())

        try:
            der_state_file = __import__('%s_der_state'%modelname, globals(),
                                        locals(), [])
            mod_der_state = getattr(der_state_file, '%s_der_state'%modelname)
        except:
            print("state derivative file '%s_der_state.py' or "\
                "needed function inside this file not found! Something went "\
                "wrong during code generation.\n%s" \
                % (modelname,sys.exc_info()), file=sys.stderr)
            return

        qd0 = list(np.zeros(len(q0)))
        integrator = Integrator(mod_der_state, q0, qd0)
        curr_time = 0
        values = np.array(q0 + qd0)
        while curr_time < t:
            q = integrator.step()
            curr_time = integrator.t
            values = np.vstack((values, q))

        outfile = modelname + '_values'
        if text_output is False:
            np.save(outfile, values)
        else:
            outfile = outfile + '.csv'
            np.savetxt(outfile, values)

    def genSarturisXml(self, modelname, dirname = '.'):
        '''
        This function writes an XML-File for the Simulation Framework SARTURIS
        (ref:'http://www.sarturis.de') developed at the institute of mobile machinery
        at Dresden University. The XML-File includes a sample definition of the
        HDAE-System as well as all graphical nodes for OpenSceneGraph.
        In order to use this output lxml needs to be installed.

        :param modelname: Name of the model - will be used as filename.
        :type modelname: String.
        :param dirname: Directory where the XML-File will be put.
        :type dirname: String.
        '''
        trafo.genSarturisXml(self, modelname, dirname)

    def genMatlabAnimation(self, modelname, dirname = '.', **kwargs):
        '''
        Use *genMatlabAnimation* to generate a MATLAB Animation file which will
        visualise your graphical objects using the simulation results from the
        MATLAB script which has been generated using *genCode*.

        :param modelname: Name of the model - will be used as filename.
        :type modelname: String.
        :param dirname: Directory where the MATLAB-Files will be put.
        :type dirname: String.
        :param kwargs: Additional Settings may be passed as kwargs:
                        * axislimits = list of six values [Xmin, Xmax, Ymin, Ymax, Zmin, Zmax]
        :type kwargs: Dictionary.
        '''
        trafo.genMatlabAnimation(self, modelname, dirname, **kwargs)

    def exportGraphReps(self, fileName):
        '''
        Create a text file for saving the information / parameters of the
        graphical representations of the system. Data is stored plain text in
        a \*.cfg file which can be used by other tools.

        :param fileName: Path and file name for saving the generated ouput.
        :type fileName: String.
        '''
        trafo.exportGraphReps(self, fileName)

    def getParameters(self):
        '''
        Returns dictionary with all the parameters (added with `addParam`) and
        their values.
        '''
        return self.paramDict

    def getController(self):
        '''
        Returns symbol and shape of the controller in a dictionary.
        '''
        return self.controllerDict

    def getMotionEquations(self):
        '''
        liefert eine Liste von symbolischen Ausdrücken aus denen man qdd für gegebene q, qd berechnen kann
        '''
        return self.graph.getAssignments(VarKind.Der_State)


    def getSensorEquations(self):
        '''
        liefert eine Liste von symbolischen Ausdrücken aus denen man jeden Sensor-Wert für gegebene q, qd Werte berechnen kann
        '''
        return self.graph.getAssignments(VarKind.Sensor)


# End of class: MbsSystem

# publish this class in the trafo module (look there for explanation)
setMbsSystemType(MbsSystem)


class AddSensor(object):
    '''
    Class that provides functions to create sensors
    '''
    # Sensor Counter - for automatic naming
    count = 0

    def _getName(self):
        '''
        returns automatically generated name
        '''
        # increase counter
        self.count = self.count + 1
        # generate sensor name
        return 'sensor_%i'%self.count

    def __init__(self, world):
        '''
        Constructor, needs reference to MbsSystem Object
        '''
        # Type Checking
        assert( isinstance(world, MbsSystem) )
        # Save Reference to World
        self.world = world

    def Distance(self, symbol, CS1, CS2, name=None, category=VarKind.Sensor):
        '''
        A distance sensor measures the scalar distance (always positive) between
        two coordinate systems. Furthermore it measures the velocity, i.e. the
        rate at which the distance changes with time. If you want to measure the
        distance vector have a look at the PositionSensor (ref:'Position').

        :param CS1: Parent Coordinate System.
        :type CS1: MbsSystem, Body, Coordinate System.
        :param CS2: Child Coordinate System.
        :type CS2: MbsSystem, Body, Coordinate System.

        :param symbol: Symbol representing a the sensor result.
        :type symbol: String.

        :param name: A name may be assigned to each sensor. If no name is given,
                     then a name like sensor_1 is generated automatically.
        :type name: String.

        :return: Symbol of the sensor
        :rtype: :class:`pymbs.Symbolics.Symbol`
        '''
        # generate name if necessary
        if (name is None): name = self._getName()
        # call world function
        return self.world._addSensor('Distance', symbol, CS1, CS2, None, name, category)

    def Position(self, symbol, CS1, CS2, CSref=None, name=None, category=VarKind.Sensor):
        '''
        A position sensor measuring the distance vector between *CS1* and *CS2*
        with the arrow pointing to *CS2*. The vector is written with respect to
        the reference frame specified by *CSref*. If *CSref* has been omitted
        then *CS1* is used.

        :param CS1: Parent Coordinate System.
        :type CS1: MbsSystem, Body, Coordinate System.
        :param CS2: Child Coordinate System.
        :type CS2: MbsSystem, Body, Coordinate System.
        :param CSref: Reference Frame.
        :type CSref: MbsSystem, Body, Coordinate System.

        :param symbol: Symbol representing a the sensor result.
        :type symbol: String.

        :param name: A name may be assigned to each sensor. If no name is given,
                     then a name like sensor_1 is generated automatically.
        :type name: String.

        :return: Symbol of the sensor
        :rtype: :class:`pymbs.Symbolics.Symbol`
        '''

        # generate name if necessary
        if (name is None): name = self._getName()
        # call world function
        return self.world._addSensor('Position', symbol, CS1, CS2, CSref, name, category)

    def Velocity(self, symbol, CS1, CS2, CSref=None, name=None, category=VarKind.Sensor):
        '''
        The velocity sensor measures the relative velocity between *CS1* and *CS2*
        with respect to the reference frame specified by *CSref*.
        If *CSref* has been omitted then *CS1* is used.

        :param CS1: Parent Coordinate System.
        :type CS1: MbsSystem, Body, Coordinate System.
        :param CS2: Child Coordinate System.
        :type CS2: MbsSystem, Body, Coordinate System.
        :param CSref: Reference Frame.
        :type CSref: MbsSystem, Body, Coordinate System.

        :param symbol: Symbol representing a the sensor result.
        :type symbol: String.

        :param name: A name may be assigned to each sensor. If no name is given,
                     then a name like sensor_1 is generated automatically.
        :type name: String.

        :return: Symbol of the sensor
        :rtype: :class:`pymbs.Symbolics.Symbol`
        '''
        # generate name if necessary
        if (name is None): name = self._getName()
        # call world function
        return self.world._addSensor('Velocity', symbol, CS1, CS2, CSref, name, category)

    def Acceleration(self, symbol, CS1, CS2, CSref=None, name=None, category=VarKind.Sensor):
        '''
        The acceleration sensor measures the relative acceleration between *CS1*
        and *CS2* with respect to the reference frame specified by *CSref*.
        If *CSref* has been omitted then *CS1* is used.

        :param CS1: Parent Coordinate System.
        :type CS1: MbsSystem, Body, Coordinate System.
        :param CS2: Child Coordinate System.
        :type CS2: MbsSystem, Body, Coordinate System.
        :param CSref: Reference Frame.
        :type CSref: MbsSystem, Body, Coordinate System.

        :param symbol: Symbol representing a the sensor result.
        :type symbol: String.

        :param name: A name may be assigned to each sensor. If no name is given,
                     then a name like sensor_1 is generated automatically.
        :type name: String.

        :return: Symbol of the sensor
        :rtype: :class:`pymbs.Symbolics.Symbol`
        '''
        # generate name if necessary
        if (name is None): name = self._getName()
        # call world function
        return self.world._addSensor('Acceleration', symbol, CS1, CS2, CSref, name, category)

    def Orientation(self, symbol, CS1, CS2, name=None, category=VarKind.Sensor):
        '''
        The Orientation sensor measures the relative orientation between *CS1*
        and *CS2*, described by a transformation matrix.

        :param CS1: Parent Coordinate System.
        :type CS1: MbsSystem, Body, Coordinate System.
        :param CS2: Child Coordinate System.
        :type CS2: MbsSystem, Body, Coordinate System.

        :param symbol: Symbol representing a the sensor result.
        :type symbol: String.

        :param name: A name may be assigned to each sensor. If no name is given,
                     then a name like sensor_1 is generated automatically.
        :type name: String.

        :return: Symbol of the sensor
        :rtype: :class:`pymbs.Symbolics.Symbol`
        '''
        # generate name if necessary
        if (name is None): name = self._getName()
        # call world function
        return self.world._addSensor('Orientation', symbol, CS1, CS2, None, name, category)

    def AngularVelocity(self, symbol, CS1, CS2, CSref=None, name=None, category=VarKind.Sensor):
        '''
        The AngularVelocity sensor measures the relative angular velocity between
        *CS1* and *CS2* with respect to the reference frame specified by *CSref*.
        If *CSref* has been omitted then *CS1* is used.

        :param CS1: Parent Coordinate System.
        :type CS1: MbsSystem, Body, Coordinate System.
        :param CS2: Child Coordinate System.
        :type CS2: MbsSystem, Body, Coordinate System.
        :param CSref: Reference Frame.
        :type CSref: MbsSystem, Body, Coordinate System.

        :param symbol: Symbol representing a the sensor result.
        :type symbol: String.

        :param name: A name may be assigned to each sensor. If no name is given,
                     then a name like sensor_1 is generated automatically.
        :type name: String.

        :return: Symbol of the sensor
        :rtype: :class:`pymbs.Symbolics.Symbol`
        '''
        # generate name if necessary
        if (name is None): name = self._getName()
        # call world function
        return self.world._addSensor('AngularVelocity', symbol, CS1, CS2, CSref, name, category)

    def AngularAcceleration(self, symbol, CS1, CS2, CSref=None, name=None, category=VarKind.Sensor):
        '''
        The AngularAcceleration sensor measures the relative angular acceleration
        between *CS1* and *CS2* with respect to the reference frame specified by
        *CSref*. If *CSref* has been omitted then *CS1* is used.

        :param CS1: Parent Coordinate System.
        :type CS1: MbsSystem, Body, Coordinate System.
        :param CS2: Child Coordinate System.
        :type CS2: MbsSystem, Body, Coordinate System.
        :param CSref: Reference Frame.
        :type CSref: MbsSystem, Body, Coordinate System.

        :param symbol: Symbol representing a the sensor result.
        :type symbol: String.

        :param name: A name may be assigned to each sensor. If no name is given,
                     then a name like sensor_1 is generated automatically.
        :type name: String.

        :return: Symbol of the sensor
        :rtype: :class:`pymbs.Symbolics.Symbol`
        '''
        # generate name if necessary
        if (name is None): name = self._getName()
        # call world function
        return self.world._addSensor('AngularAcceleration', symbol, CS1, CS2, CSref, name, category)

    def Energy(self, symbol_str, body, name=None, category=VarKind.Sensor):
        '''
        The Energy sensor measures the potential and the kinetic energy of *body*.
        If *body* is the world (MbsSystem) then the sensor measures the sum of
        the energies of each body in the system.

        :param body: Body of which the energy shall be measured. Pass the world/
                     MbsSystem if the energy of all bodies shall be measured.
        :type body: MbsSystem, Body, Coordinate System

        :param symbol: Symbol representing a the sensor result.
        :type symbol: String.

        :return: Symbol of the sensor
        :rtype: :class:`pymbs.Symbolics.Symbol`

        .. note: Only the kinetic and the potential energy of a body is considered.
                 The potential energy stored in springs or energy "lost" in dampers
                 is neglected!
        '''
        # generate name if necessary
        if (name is None): name = self._getName()

        # if body is the MbsSystem, then generate an EnergySensor for the whole System
        if ( isinstance(body, MbsSystem) ):
            # Add Total Energy
            E = 0

            # loop through all bodies
            for key in list(body.bodyDict.keys()):
                b = body.bodyDict[key]

                # add an EnergySensor for each body
                EName = 'E_%s'%b.name
                E += body.addSensor.Energy(EName, b, name=EName)

            # Add a "sensor" for the total energy
            E_sym = body.addExpression(name=name, symbol_str=symbol_str, exp=E, category=category)

            return E_sym

        # call world function
        return self.world._addEnergySensor(symbol_str, body, name, category)

    def CenterofMass(self, symbol_str, Bodies=None, name=None, category=VarKind.Sensor):
        '''
        The CenterofMass sensor measures the total mass and the position of it.
        If *body* is the world (MbsSystem) then the sensor measures the total
        mass of the system.

        :param Bodies: Bodies of which the total mass shall be measured. Pass the world/
                     MbsSystem if the total mass of all bodies shall be measured.
        :type body: list of MbsSystem, Body, Coordinate System

        :param symbol: Symbol representing a the sensor result.
        :type symbol: String.

        :return: Symbol of the sensor
        :rtype: :class:`pymbs.Symbolics.Symbol`

        .. note: Only the kinetic and the potential energy of a body is considered.
                 The potential energy stored in springs or energy "lost" in dampers
                 is neglected!
        '''
        # generate name if necessary
        if (name is None): name = self._getName()
        if (Bodies is None):
            Bodies = list(self.world.bodyDict.values())

        # Add Total Mass
        tM = 0

        r_cg = zeros((3,))
        # loop through all bodies
        for b in Bodies:

            # add Sensor for each body
            tM += b.mass

            if not hasattr(b,'_CG'):
                b.addFrame('_CG',p=b.cg)

            r = self.world.addSensor.Position('%s_%s_r'%(name,b.name), self.world, b._CG, category=VarKind.Variable)
            r_cg += r*b.mass


        tm = self.world.addExpression(name='%s_Mass'%name, symbol_str='%s_Mass'%symbol_str, exp=tM, category=category)
        return tm, self.world.addExpression(name='%s_r'%name, symbol_str='%s_r'%symbol_str, exp=r_cg/tM, category=category)

    def Joint(self, symbol, joint, name=None, category=VarKind.Sensor):
        '''
        The Joint sensor returns the joint coordinate as well as the joint
        velocity.

        :param joint: Joint of which the coordinate shall be extracted.
        :type joint: Joint-Object.

        :param symbol: Symbol representing a the sensor result.
        :type symbol: String.

        :return: Symbol of the sensor
        :rtype: :class:`pymbs.Symbolics.Symbol`
        '''
        # generate name if necessary
        if (name is None): name = self._getName()
        # call world function
        return self.world._addJointSensor(symbol, joint, name, category)


    def ConstraintForce(self, symbol, joint, name=None, category=VarKind.Sensor):
        '''
        The Joint sensor returns the joint's constraint force

        :param joint: Joint of which the constraint force shall be measured.
        :type joint: Joint-Object.

        :param symbol: Symbol representing a the sensor result.
        :type symbol: String.

        :return: Symbol of the sensor
        :rtype: :class:`pymbs.Symbolics.Symbol`
        '''
        # generate name if necessary
        if (name is None): name = self._getName()
        # call world function
        return self.world._addJointConstraintSensor('Force', symbol, joint, name, category)


    def ConstraintTorque(self, symbol, joint, name=None, category=VarKind.Sensor):
        '''
        The Joint sensor returns the joint's constraint torque

        :param joint: Joint of which the constraint torque shall be measured.
        :type joint: Joint-Object.

        :param symbol: Symbol representing a the sensor result.
        :type symbol: String.

        :return: Symbol of the sensor
        :rtype: :class:`pymbs.Symbolics.Symbol`
        '''
        # generate name if necessary
        if (name is None): name = self._getName()
        # call world function
        return self.world._addJointConstraintSensor('Torque', symbol, joint, name, category)


class AddLoad(object):
    '''
    Class that provides functions to create load elements
    '''
    # Load Element Counter - for automatic naming
    count = 0

    def _getName(self):
        '''
        returns automatically generated name
        '''
        # increase counter
        self.count = self.count + 1
        # generate sensor name
        return 'load_%i'%self.count

    def __init__(self, world):
        '''
        Constructor, needs reference to MbsSystem Object
        '''
        # Type Checking
        assert( isinstance(world, MbsSystem) )
        # Save Reference to World
        self.world = world

    def PtPForce(self, symbol, CS1, CS2, name=None):
        '''
        Use *addLoad.PtPForce* to add a scalar force, acting between two
        coordinate systems along a connecting line. A positive force means that
        the coordinate systems are pushed apart.

        :param CS1: Reference to parent coordinate system / parent frame.
        :type CS1: Coordinate System, Body or MbsSystem.
        :param CS2: Reference to child coordinate system / child frame.
        :type CS2: Coordinate System, Body or MbsSystem.

        :param symbol: Symbol representing a scalar variable whose value is
                       taken as force between the two coordinate systems.
        :type symbol: Expression as returned by addInput, addExpression or
                      addSensor.

        :param name: A name may be assigned to each force. If no name is given,
                     then a name like load_1 is generated automatically.
                     The name is used for code generation only, i.e. the
                     symbols connected with this force will contain the name.
        :type name: string

        :return: Reference to the generated LoadElement
        :rtype: LoadElement
        '''
        # generate name if necessary
        if (name is None): name = self._getName()
        # call world function
        return self.world._addLoad('PtPForce', symbol, CS1, CS2, name=name)

    def CmpForce(self, symbol, CS1, CS2, name=None, CSref=None):
        '''
        Use *addLoad.CmpForce* to add a vectorial force, acting between two
        coordinate systems. The force, specified with respect to the parent or
        reference frame, acts in positive direction on the parent coordinate
        system (CS1) and in negative direction on the child coordinate system.

        :param CS1: Reference to parent coordinate system / parent frame.
        :type CS1: Coordinate System, Body or MbsSystem.
        :param CS2: Reference to child coordinate system / child frame.
        :type CS2: Coordinate System, Body or MbsSystem.

        :param symbol: Symbol representing a three dimensional vector variable
                       whose components are interpreted as force values in
                       x, y, and z-direction. The direction of x,y and z is
                       given by the parent frame (CS1) or by the reference frame
                       (CSref).
        :type symbol: Expression as returned by addInput, addExpression or
                      addSensor.

        :param CSref: Reference to reference coordinate system / reference frame.
        :type CSref: Coordinate System, Body or MbsSystem.

        :param name: A name may be assigned to each force. If no name is given,
                     then a name like load_1 is generated automatically.
                     The name is used for code generation only, i.e. the
                     symbols connected with this force will contain the name.
        :type name: string

        :return: Reference to the generated LoadElement
        :rtype: LoadElement
        '''
        # generate name if necessary
        if (name is None): name = self._getName()
        # call world function
        return self.world._addLoad('CmpForce', symbol, CS1, CS2, CSref, name)

    def CmpTorque(self, symbol, CS1, CS2, CSref=None, name=None):
        '''
        Use *addLoad.CmpTorque* to add a vectorial torque, acting between
        two coordinate systems. The torque, specified with respect to the parent
        or reference frame, acts in positive direction on the parent coordinate
        system (CS1) and in negative direction on the child coordinate system.

        :param CS1: Reference to parent coordinate system / parent frame.
        :type CS1: Coordinate System, Body or MbsSystem.
        :param CS2: Reference to child coordinate system / child frame.
        :type CS2: Coordinate System, Body or MbsSystem.

        :param symbol: Symbol representing a three dimensional vector variable
                       whose components are interpreted as torque values around
                       the x, y, and z-axis. The direction of x,y and z is
                       given by the parent frame (CS1) or by the reference frame
                       (CSref).
        :type symbol: Expression as returned by addInput, addExpression or
                      addSensor.

        :param CSref: Reference to reference coordinate system / reference frame.
        :type CSref: Coordinate System, Body or MbsSystem.

        :param name: A name may be assigned to each force. If no name is given,
                     then a name like load_1 is generated automatically.
                     The name is used for code generation only, i.e. the
                     symbols connected with this force will contain the name.
        :type name: string

        :return: Reference to the generated LoadElement
        :rtype: LoadElement
        '''
        # generate name if necessary
        if (name is None):
            name = self._getName()
        # call world function
        return self.world._addLoad('CmpTorque', symbol, CS1, CS2, CSref, name)

    def Joint(self, symbol, joint, name=None):
        '''
        Use *addLoad.Joint* to add a torque, acting on a joint.
        In case of a translational joint, a force has to be supplied.
        In case of a rotational joint, the load represents a torque.

        :param joint: Reference to joint.
        :type joint: Joint.

        :param symbol: Symbol representing a scalar. Force or Torque depending
                       on whether it is a translational or rotational joint.
        :type symbol: Expression as returned by addInput, addExpression or
                      addSensor.

        :param name: A name may be assigned to each force. If no name is given,
                     then a name like load_1 is generated automatically.
                     The name is used for code generation only, i.e. the
                     symbols connected with this force will contain the name.
        :type name: string

        :return: Reference to the generated LoadElement
        :rtype: LoadElement
        '''
        # generate name if necessary
        if (name is None): name = self._getName()
        # call world function
        return self.world.addJointLoad(symbol, joint, name)

class AddLoop(object):
    '''
    Class that provides functions to create loops
    '''
    # Loop Element Counter - for automatic naming
    count = 0

    def _getName(self):
        '''
        returns automatically generated name
        '''
        # increase counter
        self.count = self.count + 1
        # generate sensor name
        return 'loop_%i'%self.count

    def __init__(self, world):
        '''
        Constructor, needs reference to MbsSystem Object
        '''
        # Type Checking
        assert( isinstance(world, MbsSystem) )
        # Save Reference to World
        self.world = world

    # def _addLoop(self, name, typeString, *args):

    def ThreeBarTrans(self, CS1, CS2, name=None):
        '''
        Close a loop which we call a ThreeBarTrans. It is a planar mechanism
        consisting of a series of three revolute and one translational joints.
        It often occurs in conjunction with hydraulic cylinders.

        :param CS1: Reference to parent coordinate system / parent frame.
        :type CS1: Coordinate System, Body or MbsSystem.
        :param CS2: Reference to child coordinate system / child frame.
        :type CS2: Coordinate System, Body or MbsSystem.

        :param name: A name may be assigned to each loop. If no name is given,
                     then a name like loop_1 is generated automatically.
                     The name is used for code generation only, i.e. the
                     symbols connected with this force will contain the name.
        :type name: string
        '''

        # generate name if necessary
        if (name is None): name = self._getName()
        # call world function
        return self.world._addLoop(name, 'ThreeBarTrans', CS1, CS2)

    def Steering(self, CS1, CS2, setUpH=1, setUpS=1, name=None):
        '''
        Close a loop which we call Steering. More detailed explanation follows.

        :param CS1: Reference to parent coordinate system / parent frame.
        :type CS1: Coordinate System, Body or MbsSystem.
        :param CS2: Reference to child coordinate system / child frame.
        :type CS2: Coordinate System, Body or MbsSystem.

        :param setUp: Specifying the solution which shall be used -
                        most time it means, if beams are crossed
        :type setUpH: int - either 1 or -1
        :type setUpS: int - either 1 or -1

        :param name: A name may be assigned to each loop. If no name is given,
                     then a name like loop_1 is generated automatically.
                     The name is used for code generation only, i.e. the
                     symbols connected with this force will contain the name.
        :type name: string
        '''

        # generate name if necessary
        if (name is None): name = self._getName()
        # call world function
        return self.world._addLoop(name, 'Steering', CS1, CS2, setUpH, setUpS)

    def Hexapod(self, CS1, CS2, name=None):
            '''
            Close a loop which we call a ThreeBarTrans. It is a planar mechanism
            consisting of a series of three revolute and one translational joints.
            It often occurs in conjunction with hydraulic cylinders.

            :param CS1: Reference to parent coordinate system / parent frame.
            :type CS1: Coordinate System, Body or MbsSystem.
            :param CS2: Reference to child coordinate system / child frame.
            :type CS2: Coordinate System, Body or MbsSystem.

            :param name: A name may be assigned to each loop. If no name is given,
                         then a name like loop_1 is generated automatically.
                         The name is used for code generation only, i.e. the
                         symbols connected with this force will contain the name.
            :type name: string
            '''

            # generate name if necessary
            if (name is None): name = self._getName()
            # call world function
            return self.world._addLoop(name, 'Hexapod', CS1, CS2)

    def Hexapod_m_AV(self, CS1, CS2, name=None):
            '''
            Close a loop which we call a ThreeBarTrans. It is a planar mechanism
            consisting of a series of three revolute and one translational joints.
            It often occurs in conjunction with hydraulic cylinders.

            :param CS1: Reference to parent coordinate system / parent frame.
            :type CS1: Coordinate System, Body or MbsSystem.
            :param CS2: Reference to child coordinate system / child frame.
            :type CS2: Coordinate System, Body or MbsSystem.

            :param name: A name may be assigned to each loop. If no name is given,
                         then a name like loop_1 is generated automatically.
                         The name is used for code generation only, i.e. the
                         symbols connected with this force will contain the name.
            :type name: string
            '''

            # generate name if necessary
            if (name is None): name = self._getName()
            # call world function
            return self.world._addLoop(name, 'Hexapod_m_AV', CS1, CS2)


    def CrankSlider(self, CS1, CS2, name=None):
        '''
        Close a loop which we call a CrankSlider. It is a planar mechanism
        consisting of a series of three revolute and one translational joints.
        Its best example is the piston in an engine.

        :param CS1: Reference to parent coordinate system / parent frame.
        :type CS1: Coordinate System, Body or MbsSystem.
        :param CS2: Reference to child coordinate system / child frame.
        :type CS2: Coordinate System, Body or MbsSystem.

        :param name: A name may be assigned to each loop. If no name is given,
                     then a name like loop_1 is generated automatically.
                     The name is used for code generation only, i.e. the
                     symbols connected with this force will contain the name.
        :type name: string
        '''

        # generate name if necessary
        if (name is None): name = self._getName()
        # call world function
        return self.world._addLoop(name, 'CrankSlider', CS1, CS2)


    def FourBar(self, CS1, CS2, posture=1, name=None):
        '''
        Handles a classic planar four bar linkage mechanism comprising four
        revolute joints. The *posture* parameter specifies which solution shall
        be used (crossing beams or not).

        :param CS1: Reference to parent coordinate system / parent frame.
        :type CS1: Coordinate System, Body or MbsSystem.
        :param CS2: Reference to child coordinate system / child frame.
        :type CS2: Coordinate System, Body or MbsSystem.
        :param posture: Specifying the solution which shall be used -
                        most time it means, if beams are crossed
        :type posture: int - either 1 or -1

        :param name: A name may be assigned to each loop. If no name is given,
                     then a name like loop_1 is generated automatically.
                     The name is used for code generation only, i.e. the
                     symbols connected with this force will contain the name.
        :type name: string
        '''

        # generate name if necessary
        if (name is None): name = self._getName()
        # call world function
        return self.world._addLoop(name, 'FourBar', CS1, CS2, posture)

    def FourBarTrans(self, CS1, CS2, posture=1, name=None):
        '''
        The FourBarTrans is an extension to the FourBarLinkage. Whereas the
        classical four bar linkage consists of only four revolute joints, the
        FourBarTrans is extended by a translational joint. This mechanism is
        often found in wheel loaders.

        :param CS1: Reference to parent coordinate system / parent frame.
        :type CS1: Coordinate System, Body or MbsSystem.
        :param CS2: Reference to child coordinate system / child frame.
        :type CS2: Coordinate System, Body or MbsSystem.

        :param name: A name may be assigned to each loop. If no name is given,
                     then a name like loop_1 is generated automatically.
                     The name is used for code generation only, i.e. the
                     symbols connected with this force will contain the name.
        :type name: string
        '''

        # generate name if necessary
        if (name is None): name = self._getName()
        # call world function
        return self.world._addLoop(name, 'FourBarTrans', CS1, CS2, posture)

    def Transmission(self, j1, j2, ratio=1, name=None):
        '''
        The transmission loop introduces a relation between two joints, such
        that their joint coordinates q1 and q2 are related by the following
        equation: j1 = *ratio* \* v2;
        This can either be used to synchronise joints or to create a fixed
        joint by choosing *ratio* =0.

        :param j1: First joint.
        :type j1: Joint-Object.
        :param j2: Second Joint.
        :type j2: Joint-Object.
        :param ratio: Ratio between two joints.
        :type ratio: int/float.

        :param name: A name may be assigned to each loop. If no name is given,
                     then a name like loop_1 is generated automatically.
                     The name is used for code generation only, i.e. the
                     symbols connected with this force will contain the name.
        :type name: string
        '''
        # generate name if necessary
        if (name is None): name = self._getName()
        # call world function
        return self.world._addLoop(name, 'Transmission', j1, j2, ratio)


    def ExpJoint(self, j, exp, name=None):
        '''
        The ExpJoint allows the user to provide an expression for the joint
        coordinate. Currently, only kinematic calculations is supported.

        :param j: joint.
        :type j: Joint-Object.
        :param exp: Expression for joint coordinate
        :type exp: Expression.

        :param name: A name may be assigned to each loop. If no name is given,
                     then a name like loop_1 is generated automatically.
                     The name is used for code generation only, i.e. the
                     symbols connected with this force will contain the name.
        :type name: string
        '''
        # generate name if necessary
        if (name is None): name = self._getName()
        # call world function
        return self.world._addLoop(name, 'ExpJoint', j, exp)


class AddVisualisation(object):
    '''
    Class that provides functions to create visual shapes
    '''
    # Visualisation Element Counter - for automatic naming
    count = 0

    def _getName(self):
        '''
        returns automatically generated name
        '''
        # increase counter
        self.count = self.count + 1
        # generate sensor name
        return 'visual_%i'%self.count

    def __init__(self, world):
        '''
        Constructor, needs reference to MbsSystem Object
        '''
        # Type Checking
        assert( isinstance(world, MbsSystem) )
        # Save Reference to World
        self.world = world

    # _addVisualisation(self, name, cs, typeString, *args):

    def File(self, cs, fileName, scale=1, name=None, color=None):
        '''
        Attach a File-Object given by *fileName* to the Coordinate System given
        by *cs*. Since the python visualisation is based on VTK, all file formats
        that are compatible to VTK are supported. MATLAB Animations, however, only
        support STL-Files.

        :param cs: Coordinate System to which the Graphical Object shall be attached.
        :type cs: MbsSystem, Body, Coordinate System.
        :param fileName: Filename of the graphics file.
        :type fileName: String.
        :param scale: Can be used to scale down the Graphic. Use 1000 if the
                      graphic information is stored in millimeters, for example.
        :type scale: int/float.
        :param name: A name may be assigned to each visualistion. If no name is
                     given, then a name like visual_1 is generated automatically.
                     The name is used for the sensor and thus for code generation.
        :type name: String.
        :param color: Apply a specific color to a visualisation. You can use
                      either an RGB list, e.g. color=[0.7, 0.3, 0.1], or
                      any VTK color (from vtk.util.colors).
        :type color: List, Tuple, VTK Color
        '''
        # generate name if necessary
        if (name is None): name = self._getName()
        # call world function
        return self.world._addVisualisation(name, cs, 'file', fileName, scale, color=color)

    def Line(self, cs, length=1, name=None, color=None):
        '''
        Add a line object which starts at [0,0,0] and ends at [*length*,0,0],
        i.e. spans in x-direction.

        :param cs: Coordinate System to which the Graphical Object shall be attached.
        :type cs: MbsSystem, Body, Coordinate System.
        :param length: Length of the line.
        :type length: int/float.
        :param name: A name may be assigned to each visualistion. If no name is
                     given, then a name like visual_1 is generated automatically.
                     The name is used for the sensor and thus for code generation.
        :type name: String.
        :param color: Apply a specific color to a visualisation. You can use
                      either an RGB list, e.g. color=[0.7, 0.3, 0.1], or
                      any VTK color (from vtk.util.colors).
        :type color: List, Tuple, VTK Color
        '''
        # generate name if necessary
        if (name is None): name = self._getName()
        # call world function
        return self.world._addVisualisation(name, cs, 'line', length, color=color)

    def Box(self, cs, length=1, width=1, height=1, name=None, color=None):
        '''
        Add a line object which whose dimensions are called:
        * x - *length*
        * y - *width*
        * z - *height*
        The origin of this graphical representation lies in the centre of the objet.

        :param cs: Coordinate System to which the Graphical Object shall be attached.
        :type cs: MbsSystem, Body, Coordinate System.
        :param length: Length (x-dimension) of the box.
        :type length: int/float.
        :param width: Width (y-dimension) of the box.
        :type width: int/float.
        :param height: Height (z-dimension) of the box.
        :type height: int/float.
        :param name: A name may be assigned to each visualistion. If no name is
                     given, then a name like visual_1 is generated automatically.
                     The name is used for the sensor and thus for code generation.
        :type name: String.
        :param color: Apply a specific color to a visualisation. You can use
                      either an RGB list, e.g. color=[0.7, 0.3, 0.1], or
                      any VTK color (from vtk.util.colors).
        :type color: List, Tuple, VTK Color
        '''
        # generate name if necessary
        if (name is None): name = self._getName()
        # call world function
        return self.world._addVisualisation(name, cs, 'box', length, width, height, color=color)

    def Cylinder(self, cs, radius=1, height=1, res=20, name=None, color=None):
        '''
        Add a cylindrical graphical representation to a coordinate system given
        by *cs*. Its groundplane forms a circle with the radius given by *radius*
        and lies in the x-y-plane. The origin lies directly in the centre, at
        half of the height.

        :param cs: Coordinate System to which the Graphical Object shall be attached.
        :type cs: MbsSystem, Body, Coordinate System.
        :param radius: Radius of the circular groundplane.
        :type radius: int/float.
        :param height: Height of the cylinder.
        :type height: int/float.
        :param name: A name may be assigned to each visualistion. If no name is
                     given, then a name like visual_1 is generated automatically.
                     The name is used for the sensor and thus for code generation.
        :type name: String.
        :param color: Apply a specific color to a visualisation. You can use
                      either an RGB list, e.g. color=[0.7, 0.3, 0.1], or
                      any VTK color (from vtk.util.colors).
        :type color: List, Tuple, VTK Color
        '''
        # generate name if necessary
        if (name is None): name = self._getName()
        # call world function
        return self.world._addVisualisation(name, cs, 'cylinder', radius, height, res, color=color)

    def Sphere(self, cs, radius=1, res=50, name=None, color=None):
        '''
        Attaches a sphere with the radius *radius* to a given coordinate system
        *cs*. The origin lies directly in the middle of the sphere.

        :param cs: Coordinate System to which the Graphical Object shall be attached.
        :type cs: MbsSystem, Body, Coordinate System.
        :param radius: Radius of the sphere.
        :type radius: int/float.
        :param name: A name may be assigned to each visualistion. If no name is
                     given, then a name like visual_1 is generated automatically.
                     The name is used for the sensor and thus for code generation.
        :type name: String.
        :param color: Apply a specific color to a visualisation. You can use
                      either an RGB list, e.g. color=[0.7, 0.3, 0.1], or
                      any VTK color (from vtk.util.colors).
        :type color: List, Tuple, VTK Color
        '''
        # generate name if necessary
        if (name is None): name = self._getName()
        # call world function
        return self.world._addVisualisation(name, cs, 'sphere', radius, res, color=color)

    def Frame(self, cs, size=1, name=None):
        '''
        Attaches a graphical representation of a coordinate system. It consists
        of three lines, each pointing in the direction of an axis. Use size to
        vary the length of the lines.

        The axes can be distinguished by their color:
            x-axis: red
            y-axis: yellow
            z-axis: green

        :param cs: Coordinate System to which the Graphical Object shall be attached.
        :type cs: MbsSystem, Body, Coordinate System.
        :param size: length of an axis of this coordinate system.
        :type size: int/float.
        :param name: A name may be assigned to each visualistion. If no name is
                     given, then a name like visual_1 is generated automatically.
                     The name is used for the sensor and thus for code generation.
        :type name: String.
        '''
        # generate name if necessary
        if (name is None): name = self._getName()
        # call world function
        return self.world._addVisualisation(name, cs, 'coordsys', size)

    def Arrow(self, cs, size=1, name=None):
        '''
        Add a arrow graphical representation to a coordinate system given
        by *cs*.

        :param cs: Coordinate System to which the Graphical Object shall be attached.
        :type cs: MbsSystem, Body, Coordinate System.
        :param radius: Radius of the circular groundplane.
        :type radius: int/float.
        :param length: Length of the Arrow.
        :param name: A name may be assigned to each visualistion. If no name is
                     given, then a name like visual_1 is generated automatically.
                     The name is used for the sensor and thus for code generation.
        :type name: String.
        '''
        # generate name if necessary
        if (name is None): name = self._getName()
        # call world function
        return self.world._addVisualisation(name, cs, 'arrow', size)


    def flexible_Body(self, cs, flex_Body, name=None):
        '''
        Add a flexible body graphical representation to a coordinate system given
        by *cs*.

        :param cs: Coordinate System to which the Graphical Object shall be attached.
        :type cs: MbsSystem, Body, Coordinate System.
        :param pos_nodes: position of the node frames (bodyframe/inertial?)
        :type pos_nodes: tupel?
        :param name: A name may be assigned to each visualistion. If no name is
                     given, then a name like visual_1 is generated automatically.
                     The name is used for the sensor and thus for code generation.
        :type name: String.
        '''
         # generate name if necessary
        if (name is None): name = self._getName()
        # call world function
        return self.world._addVisualisation(name, cs, 'flexible_Body', flex_Body)

class GenEqns(object):
    '''
    Class that provides interface to equation generation
    '''

    def __init__(self, world):
        '''
        Constructor, needs reference to MbsSystem Object
        '''
        # Type Checking
        assert( isinstance(world, MbsSystem) )
        # Save Reference to World
        self.world = world

    def Explicit(self, diff=0, kinematicsOnly=False, \
                     simplify=False, graphOptimizations=False):
        '''
        This function derives equations of motion for the given system. It
        should be run after the system has been assembled, i.e. all bodies,
        joints, load elements and sensors are defined.

        The explicit scheme is only recommended for small systems with up to
        three degree of freedoms. If the mechanism is planar, then it might be
        suitable for up to five degrees of freedom.

        :param diff: With diff the user can specify how often the algebraic
                     constraints (on position level) shall be differentiated
                     with respect to time (if there are any kinematic loops
                     present). Valid values are:
                     * 0 - position level
                     * 1 - velocity level
                     * 2 - acceleration level
                     If you need the constraints on more than one level, than
                     a list of may also be supplied, i.e. [0, 1, 2]
        :type diff: list of int
        :param kinematicsOnly: If set to true, no equations for forces and
                               accelarations are generated, only position and
                               velocities are available
        :type kinematicsOnly: Boolean
        :param simplify: If set to true, simplification of expression is
                         performed which can be very time consuming. Especially
                         the simplifications done by pymbs.Symbolics are not
                         very efficient.
        :type simplify: Boolean
        :param graphOptimizations: If set to False, Graph simplification is
                                   skipped.
        :type simplify: Boolean

        '''
        return self.world._genEquations(mode="explicit", diff=diff, kinematicsOnly=kinematicsOnly, \
                                        simplify=simplify, graphOptimizations=graphOptimizations)



    def Recursive(self, diff=0, kinematicsOnly=False, \
                     simplify=False, graphOptimizations=True):
        '''
        This function derives equations of motion for the given system. It
        should be run after the system has been assembled, i.e. all bodies,
        joints, load elements and sensors are defined.

        The recursive scheme is recommended for medium systems with more than
        three degree of freedoms.

        :param diff: With diff the user can specify how often the algebraic
                     constraints (on position level) shall be differentiated
                     with respect to time (if there are any kinematic loops
                     present). Valid values are:
                     * 0 - position level
                     * 1 - velocity level
                     * 2 - acceleration level
                     If you need the constraints on more than one level, than
                     a list of may also be supplied, i.e. [0, 1, 2]
        :type diff: list of int
        :param kinematicsOnly: If set to true, no equations for forces and accelarations
                               are generated, only position and velocities are available
        :type kinematicsOnly: Boolean
        :param simplify: If set to true, simplification of expression is performed which can
                         be very time consuming. Especially the simplifications done by pymbs.Symbolics
                         are not very efficient.
        :type simplify: Boolean
        :param graphOptimizations: If set to False, Graph simplification is skipped.
        :type simplify: Boolean

        '''
        return self.world._genEquations(mode="recursive", diff=diff, kinematicsOnly=kinematicsOnly, \
                                        simplify=simplify, graphOptimizations=graphOptimizations)



    def OrderN(self, diff=0, kinematicsOnly=False, \
                     simplify=False, graphOptimizations=True):
        '''
        This function derives equations of motion for the given system. It
        should be run after the system has been assembled, i.e. all bodies,
        joints, load elements and sensors are defined.

        The OrderN scheme is recommended for large systems with more than
        twenty degree of freedoms.

        :param diff: With diff the user can specify how often the algebraic
                     constraints (on position level) shall be differentiated
                     with respect to time (if there are any kinematic loops
                     present). Valid values are:
                     * 0 - position level
                     * 1 - velocity level
                     * 2 - acceleration level
                     If you need the constraints on more than one level, than
                     a list of may also be supplied, i.e. [0, 1, 2]
        :type diff: list of int
        :param kinematicsOnly: If set to true, no equations for forces and accelarations
                               are generated, only position and velocities are available
        :type kinematicsOnly: Boolean
        :param simplify: If set to true, simplification of expression is performed which can
                         be very time consuming. Especially the simplifications done by pymbs.Symbolics
                         are not very efficient.
        :type simplify: Boolean
        :param graphOptimizations: If set to False, Graph simplification is skipped.
        :type simplify: Boolean

        '''
        return self.world._genEquations(mode="ordern", diff=diff, kinematicsOnly=kinematicsOnly, \
                                        simplify=simplify, graphOptimizations=graphOptimizations)



class GenCode(object):
    '''
    Class that provides functions to write source code of the obtained
    equations of motion for various languages
    '''

    def __init__(self, world):
        '''
        Constructor, needs reference to MbsSystem Object
        '''
        # Type Checking
        assert( isinstance(world, MbsSystem) )
        # Save Reference to World
        self.world = world


    def Python(self, modelname, dirname='.'):
        '''
        After the equations of motion have been generated, the simulation code
        for Python can be generated with this function.

        :param modelname: Name of the model - will be used as a filename at least
        :type modelname: String
        :param dirname: Directory where the model shall be exported to
        :type dirname: String.
        '''
        return trafo.genCode(self.world, "py", modelname, dirname)


    def Matlab(self, modelname, dirname='.', symbolic_mode=False):
        '''
        After the equations of motion have been generated, the simulation code
        for Matlab can be generated with this function.

        :param modelname: Name of the model - will be used as a filename at least
        :type modelname: String
        :param dirname: Directory where the model shall be exported to
        :type dirname: String.
        :param symbolic_mode: If symbolic_mode is True, code for the symbolic toolbox will be written
        :type symbolic_mode: Boolean.
        '''
        return trafo.genCode(self.world, "m", modelname, dirname,
                symbolic_mode=symbolic_mode)


    def Modelica(self, modelname, dirname='.', inputsAsInputs=True, genVisualFile=None, **kwargs):
        '''
        After the equations of motion have been generated, the simulation code
        for Modelica can be generated with this function.

        :param modelname: Name of the model - will be used as a filename at least
        :type modelname: String
        :param dirname: Directory where the model shall be exported to
        :type dirname: String
        :param genVisualFile: Name of the Visual File; if no name is supplied,
            no visual file is generated
        :type genVisualFile: String
        :param inputsAsInputs: *Missing*
        :type inputsAsInputs: Bool
        :param package: Package name
        :type package: String
        '''

        if (genVisualFile is not None):


            f = open( '%s/%s.mo'%(dirname,genVisualFile), 'w' )

            if ('package' in kwargs):
               f.write('within %s;\n'%kwargs['package'])
            f.write('model %s "Defines Connections for Model"\n'%genVisualFile)
            f.write('  extends %s;\n'%modelname)
            f.write('  import SI = Modelica.SIunits;\n')

            # this is the list of graphRep objects:
            grList = list(self.world.graphRepDict.values())

            x=-80
            y=-100
            size=20
            i=0
            for gr in grList:
                f.write('  Modelica.Mechanics.MultiBody.Visualizers.Advanced.Shape shape%s(\n'%i)
                f.write('    r=%s[:, 1],\n'%gr.r)
                f.write('    R=Modelica.Mechanics.MultiBody.Frames.Orientation(T=transpose(%s), w={0,0,0}),\n'%gr.T)
                if (isinstance(gr,gr.Cylinder)):
                    f.write('    shapeType="cylinder",\n')
                    f.write('    height=%s,\n'%gr.radius)
                    f.write('    length=%s,\n'%gr.l)
                    f.write('    width=%s,\n'%gr.radius)
                elif (isinstance(gr,gr.Sphere)):
                    f.write('    shapeType="sphere",\n')
                    f.write('    height=%s,\n'%gr.radius)
                    f.write('    length=%s,\n'%gr.radius)
                    f.write('    width=%s,\n'%gr.radius)
                else:
                    print('Failed to generate Visualisationfile for %s'%gr.__class__)
                    return
                f.write('    lengthDirection={0,1,0},\n')
                f.write('    widthDirection={0,0,1})\n')
                f.write('    annotation (Placement(transformation(extent={{%s,%s},{%s,%s}})));\n'%(x,y,x+size,y+size))
                i = i+1
                x = x+size
                if (x>80):
                    x=-100
                    y = y+size

            f.write('  inner Modelica.Mechanics.MultiBody.World world(n={%s,%s,%s})\n'%( self.world.gravity_vect[0], self.world.gravity_vect[1], self.world.gravity_vect[2]))
            f.write('    annotation (Placement(transformation(extent={{-100,-100},{-80,-80}})));\n')

            f.write('end %s;\n'%genVisualFile)

        return trafo.genCode(self.world, "mo", modelname, dirname,
                inputsAsInputs=inputsAsInputs, **kwargs)

    def C(self, modelname, dirname='.', **kwargs):
        '''
        After the equations of motion have been generated, the simulation code
        for C can be generated with this function.

        :param modelname: Name of the model - will be used as a filename at least
        :type modelname: String
        :param dirname: Directory where the model shall be exported to
        :type dirname: String.
        :param pymbs_wrapper: Set to ``True`` to write python wrapper as well
        :type pymbs_wrapper: Bool
        :param sfunction: Generate s-function for Matlab-Simulink
        :type sfunction: Bool
        :param include_visual: Generate code for visualisation
        :type include_visual: Bool
        '''
        return trafo.genCode(self.world, "c", modelname, dirname, **kwargs)

    def FMU(self, modelname, dirname='.', compile=True, **kwargs):
        '''
        .. deprecated:: 0.2.2
            Currently the FMU-Writer is not working. All the code is still in
            place, but we are unable to maintain it at the moment. If this
            functionality would be useful to you, and you are willing to
            improve/maintain it, feel free to get in touch.

        After the equations of motion have been generated, the simulation code
        for FMU can be generated with this function.

        :param modelname: Name of the model - will be used as a filename at least
        :type modelname: String
        :param dirname: Directory where the model shall be exported to
        :type dirname: String.
        :param compile: If a binary of the model shall be created. Compillation is done by gcc which has to be in PATH
        :type compile: Boolean.
        :param util_folder_path: Path to FMU templates and includes
        :type util_folder_path: String
        '''
        return trafo.genCode(self.world, "fmu", modelname, dirname,
                compile=compile, **kwargs)

    def Fortran90(self, modelname, dirname='.', **kwargs):
        '''
        After the equations of motion have been generated, the simulation code
        for Fortran can be generated with this function.

        :param modelname: Name of the model - will be used as a filename at least
        :type modelname: String
        :param dirname: Directory where the model shall be exported to
        :type dirname: String.
        :param pymbs_wrapper: Export python wrapper as well
        :type pymbs_wrapper: Bool
        '''
        return trafo.genCode(self.world, "f90", modelname, dirname, **kwargs)
