from pymbs.processing.loads.load import Load
from pymbs.common.functions import transpose, norm

from pymbs.processing import Joint

from pymbs.symbolics import eye


class JointLoad(Load):
    '''
    Load used for actuated Joints
    '''


    def __init__(self, name, joint, load):
        '''
        Constructor
        '''

        # Assertions
        assert( isinstance(joint, Joint) )

        # Get Type of Joint
        if ((norm(joint.Phi) == 1) and (norm(joint.Psi) == 0)):
            torque = load
            force = None
        elif ((norm(joint.Phi) == 0) and (norm(joint.Psi) == 1)):
            torque = None
            force = load
        else:
            raise AttributeError('Joint %s is neither pure rotational nor translational!'%joint.name)


        # Constructor of Super Class
        # BugFix for 014:
        if (joint.body.CS_0.R != eye(3)):
            if (not hasattr(joint.body, '_CS_0')):
                joint.body._CS_0 = joint.body.addFrame('_int_CS0_2')
            Load.__init__(self, name, joint.body._CS_0, joint.coordSys, force, torque)

        else:
            Load.__init__(self, name, joint.body.CS_0, joint.coordSys, force, torque)

        self.joint = joint


    def calc(self, graph = None, states = None):
        '''
        Calculcate Wf and Wt
        Overwrites Function in Super Class
        '''

        self.Wf = self.joint.Psi
        self.Wt = self.joint.Phi
