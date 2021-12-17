from .input_element import MbsInputElement

from pymbs.symbolics import VarKind

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
