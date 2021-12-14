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


Module containing classes modelling mechanical sensors
'''

from .MbsInputElement import MbsInputElement

from PyMbs.Symbolics import VarKind

class AbstractSensor(MbsInputElement):

    def __init__(self, name, CS1, CS2, symb, CSref=None, category=VarKind.Sensor):

        #!! TODO: asserts
        MbsInputElement.__init__(self, name, CS1)
        self.child=CS2

        self.symb = symb
        self.CSref = CSref
        self.category = category


class DistanceSensor(AbstractSensor):
    pass

class PositionSensor(AbstractSensor):
    pass

class VelocitySensor(AbstractSensor):
    pass

class AccelerationSensor(AbstractSensor):
    pass

class OrientationSensor(AbstractSensor):
    pass

class AngVelocitySensor(AbstractSensor):
    pass

class AngAccelerationSensor(AbstractSensor):
    pass

class JointSensor(MbsInputElement):
    # this class has almost nothing in common with the other SensorClasses
    # therefore it is not derived from AbstractSensor

    def __init__(self, name, joint, symb, category=VarKind.Sensor):
        self.name = name
        self.joint = joint
        self.symb = symb
        self.category = category

class ConstraintForceSensor(JointSensor):
    pass

class ConstraintTorqueSensor(JointSensor):
    pass

class EnergySensor(MbsInputElement):
    # this class has almost nothing in common with the other SensorClasses
    # therefore it is not derived from AbstractSensor

    def __init__(self, name, body, symb, category=VarKind.Sensor):
        self.name = name
        self.body = body
        self.symb = symb
        self.category = category
