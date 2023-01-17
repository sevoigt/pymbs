'''
Module for the transformation of input-data-structures to
processing-data-structures
'''

import os
import tempfile

from pymbs.symbolics import Symbol, eye, Matrix

from pymbs.input.body import Body as iBody
from pymbs.input.body import FlexibleBody as iFlexibleBody
from pymbs.input.body import FlexibleBody

from pymbs.input.constraints import FourBar as iFourBar
from pymbs.input.constraints import ThreeBarTrans as iThreeBarTrans
from pymbs.input.constraints import Steering as iSteering
from pymbs.input.constraints import CrankSlider as iCrankSlider
from pymbs.input.constraints import FourBarTrans as iFourBarTrans
from pymbs.input.constraints import Transmission as iTransmission
from pymbs.input.constraints import ExpJoint as iExpJoint
from pymbs.input.constraints import Hexapod as iHexapod
from pymbs.input.constraints import Hexapod_m_AV as iHexapod_m_AV

from . import loads as iLoads

from pymbs.common.functions import transpose, skew, skew_numpy
from pymbs.common.pymbserror import PyMbsError

from pymbs.processing.body import Body as pBody
from pymbs.processing.body import FlexibleBody as pFlexibleBody
from pymbs.processing.frame import Frame as pFrame

from pymbs.processing.loops.fourbar import FourBar as pFourBar
from pymbs.processing.loops.threebar_trans import ThreeBarTrans as pThreeBarTrans
from pymbs.processing.loops.steering import Steering as pSteering
from pymbs.processing.loops.crank_slider import CrankSlider as pCrankSlider
from pymbs.processing.loops.fourbar_trans import FourBarTrans as pFourBarTrans
from pymbs.processing.loops.transmission import Transmission as pTransmission
from pymbs.processing.loops.exp_joint import ExpJoint as pExpJoint
from pymbs.processing.loops.hexapod import Hexapod as pHexapod
from pymbs.processing.loops.hexapod_with_axes_offset import Hexapod_m_AV as pHexapod_m_AV

from pymbs.processing.generator_explicit import Generator_Explicit
from pymbs.processing.generator_recursive import Generator_Recursive
from pymbs.processing.generator_order_n import Generator_OrderN

from . import sensors as iSensors
import pymbs.processing.sensors as pSensors
import pymbs.processing.loads as pLoads

from pymbs.processing.loads.constraint import Constraint as pConstraint

from pymbs.ui.gui import launchGui
import numpy as np


# containers (global on module-level)
_userExpressions=None
_gravity=None
_pInertialBody=None
_loops=None
#_constraints=None # the creation of a pConstraint suffices
_loads=None
_sensors=None
_inputs=None
_jointRangeDict=None

# hack!!
# motivation: avoid importing MbsSystem in this module (due to circular imports)
# but still having the type >MbsSystem< available here
# solution: create a module-global placeholder that will be filled by
# the MbsSystemModule

_MbsSystemType=None

def setMbsSystemType(t):
    assert isinstance(t, type)
    global _MbsSystemType
    _MbsSystemType = t


def _transformModel(model):
    """
    model: MbsSystem instance (input model)

    returns processing.Body instance (processing model)
    """

    print("start transformation")

    # globals of this module:
    global _userExpressions
    global _gravity
    global _pInertialBody
    global _loops
    #global _constraints
    global _sensors
    global _loads
    global _inputs
    global _jointRangeDict


    # initialize the global containiners
    _userExpressions=None
    _gravity=None
    _pInertialBody=None
    _loops=None
    #_constraints=None
    _loads=None
    _sensors=None
    _inputs=None

    assert isinstance(model, _MbsSystemType)

    _Graph = model.graph

    bodies = [model]+list(model.bodyDict.values())

    # Flatten CS Structure
    for i_body in bodies:
        flattenCSStructure(i_body)

    # Convert Fixed Bodies to Coordinate Systems ( == 1)
    applyFixedJoints(model, 1)

    # Flatten CS Structure - again
    for i_body in bodies:
        flattenCSStructure(i_body)

    # Recalculate mass, cg, Inertia ( == 2)
    applyFixedJoints(model, 2)

    # Bodies are reduced by "Fixed Bodies"
    bodies = [model]+list(model.bodyDict.values())
    # Generate processing Bodies and CS
    for i_body in bodies:
        if i_body is model:
            pWorld=pBody(model.name, graph=_Graph)
            model.referenceToNew=pWorld
            new_pBody=pWorld


        elif isinstance(i_body, FlexibleBody):

            # node list
            nList = i_body.sid.modal.frame.Knoten

            for node in nList:
                '''
                linear transformation of Phi- and Psi matrices
                displacement of node 1 is not allowed (u and theta must be equal to zero), thus using rigid body motion to compensate the displacement
                R_tilde... position of node (skew-symetric matrix)
                node.A = [I R_tilde^T; 0 I]
                node.phi_psi = [Phi;Psi]
                '''
                Phi = node.phi.phimatrix.M0
                Psi = node.psi.psimatrix.M0
                node.phi_psi = np.concatenate([Phi,Psi],axis = 0)

                pos_node_numpy = node.origin.originmatrix.M0
                R_tilde = skew_numpy(pos_node_numpy)

                node.A = np.concatenate([np.concatenate([np.matrix(np.eye(3,3)),np.transpose(R_tilde)],axis = 1),np.concatenate([np.matrix(np.zeros([3,3])),np.matrix(np.eye(3,3))],axis = 1)],axis = 0)

            # number of nodes
            len_of_nList = i_body.sid.nNodes
            number_of_modes = i_body.sid.nModes

            for i in range(len_of_nList-1):
                '''
                calculate the new Phi and Psi matrices from node 2 up to node n
                [Phi_dash_i;Psi_dash_i]=A_i*A_1^(-1)*[Phi_1;Psi_1]+[Phi_i;Psi_i]
                '''
                phi_psi_dash = -nList[i+1].A * np.linalg.inv(nList[0].A) * nList[0].phi_psi + nList[i+1].phi_psi
                phi = phi_psi_dash[0:3,0:number_of_modes]
                psi = phi_psi_dash[3:6,0:number_of_modes]

                nList[i+1].phi.phimatrix.M0 = phi
                nList[i+1].psi.psimatrix.M0 = psi

                for column in range(number_of_modes):
                    '''
                    generate the new origin matrix (eigenvector), each column of Phi matrix is a origin eigenvector,
                    which has an amplitude "q"
                    '''
                    nList[i+1].origin.originmatrix.M1[column] = phi[:,column]
                    nList[i+1].ap.apmatrix.M1[column] = skew_numpy(psi[:,column])



            # last but not least, calculating the Phi, Psi and origin matrices for node 1
            phi_psi_dash = -nList[0].A * np.linalg.inv(nList[0].A) * nList[0].phi_psi + nList[0].phi_psi
            phi = phi_psi_dash[0:3,0:number_of_modes]
            psi = phi_psi_dash[3:6,0:number_of_modes]
            nList[0].phi.phimatrix.M0 = phi
            nList[0].psi.psimatrix.M0 = psi

            for column in range(number_of_modes):
                '''
                generate the new origin matrix (eigenvector), each column of Phi matrix is a origin eigenvector,
                which has an amplitude "q"
                node 1 only
                '''
                nList[0].origin.originmatrix.M1[column] = phi[:,column]
                nList[0].ap.apmatrix.M1[column] = skew_numpy(psi[:,column])


            new_pFlexibleBody=pFlexibleBody(i_body.sid,i_body.name, i_body.mass,i_body.cg,i_body.inertia,graph=_Graph)
            i_body.referenceToNew=new_pFlexibleBody

        else:
            new_pBody=pBody(i_body.name, i_body.mass, i_body.cg, i_body.inertia, graph=_Graph)
            i_body.referenceToNew=new_pBody

        # add the Frametems of that body
        for cs in i_body.coordList:
            if isinstance(i_body,FlexibleBody):
                new_CS=new_pFlexibleBody.addFrame(cs.name, cs.p, cs.R)
                cs.referenceToNew=new_CS
                #cs.frame = cs.reference
                if len(cs.coordList) > 0:
                    raise ValueError("There should not exist any sublevelCS")

            else:
                new_CS=new_pBody.addFrame(cs.name, cs.p, cs.R)
                cs.referenceToNew=new_CS
                if len(cs.coordList) > 0:
                    raise ValueError("There should not exist any sublevelCS")

        # update frames stored in node objects
        if isinstance(i_body, FlexibleBody):
            p_Body = i_body.referenceToNew
            for node in p_Body.sid.node_List:
                node.frame = node.frame.referenceToNew


    # create the joints:
    for i_joint in list(model.jointDict.values()):
        parentCS=i_joint.parent.referenceToNew
        childCS=i_joint.child.referenceToNew

        # get the parent body of the joint
        target_body=i_joint.child.parent
        assert isinstance(target_body, (iBody,iFlexibleBody))


        dofString=i_joint.dof

        if dofString.startswith('R'):
            typestring='Rot'
        elif dofString.startswith('T'):
            typestring='Trans'
        else:
            ValueError, "Invalid dofString: %s" % dofString

        if dofString.endswith('x'):
            axis=[1,0,0]
        elif dofString.endswith('y'):
            axis=[0,1,0]
        elif dofString.endswith('z'):
            axis=[0,0,1]
        else :
            raise ValueError("Invalid dofString: %s" % dofString)


        new_Body=target_body.referenceToNew

        # the inertial frame of that body has to lie in the joint
        # -> adapt all following Frametems, the cg and the inertia tensor

        # JointEndFrame ^= "jef"
        # BodyInertialFrame (of the inputBody) ^= "bif"
        jef=i_joint.child
        p=jef.p
        R=jef.R


        # p, R define jef with respect to bif
        # now we need p2, R2 which define bif with respect to jef

        # CS 25.06.2011, Bugfix: It must be p2 = R.tranpsose()*(-p) since
        # R resolves a vector of jef -> bif, and p lives in bif

        R2 = transpose(R)
        p2 = R2*(-p)

        # cg was defined with respect to bif
        # (and new_Body.cg still has this value)
        # but the inertial frame of the new body lies in jef

        new_Body.cg= p2 + R2*new_Body.cg

        # the same must be done for the orientation of inertia
        # right??

        new_Body.inertia= R2*new_Body.inertia*transpose(R2)

        # now all child-CS of this body have to be redefined
        updateChildCS(new_Body, p2, R2)

        # TODO: here the name suggestion has to be passed
        # (stored in i_joint.name)
        p_joint=parentCS.connectTo(target_body.referenceToNew,
                                   typestring, axis, i_joint.name,
                                   q0=i_joint.q0, qd0=i_joint.qd0)

        # now make the new joint known in the old structure
        i_joint.referenceToNew=p_joint

    # end of for i_joint in ...

    # transform model.jointRangeMap such that it maps p_joint.q -> (min, max)
    # instead of i_joint -> (min, max)

    _jointRangeDict={} # the new mapping
    for i_joint, rangeTuple in list(model.jointRangeDict.items()):
        newKey = i_joint.referenceToNew.q
        _jointRangeDict[newKey] = rangeTuple

    _userExpressions = list(model.expressionDict.values())
    _gravity = model.gravity_vect*model.gravity_const
    _pInertialBody = pWorld
    _loops = convertloops(list(model.loopDict.values()))
    _loads = convertLoads(list(model.loadDict.values()))
    _sensors = convertSensors(list(model.sensorDict.values()))
    #_inputs=model.inputSymbolList

    convertConstraints(list(model.constraintDict.values()))

    print("End of (almost complete) Transformation")

def convertConstraints(constrList):
    """
    Takes a List of iConstraints and creates a pConstraint for
    each Element

    return nothing
    """

    for i_constr in constrList:
        newCS1 = i_constr.parent.referenceToNew
        newCS2 = i_constr.child.referenceToNew

        p_constr = pConstraint(i_constr.name, newCS1, newCS2,
                               i_constr.transLock, i_constr.rotLock)

def convertLoads(loadList):
    """
    converts loads from input to processing form
    """


    # special treating of JointLoads:
    # first: separte them
    isJL = lambda load: isinstance(load, iLoads.JointLoad)

    jointLoadList = list(filter(isJL, loadList))

    isNotJL = lambda load: not(isJL(load))
    normalLoadList = list(filter(isNotJL, loadList))

    assert len(normalLoadList) + len(jointLoadList) == len(loadList)

    newLoadList=[]

    # process the JointLoads

    for jL in jointLoadList:
        name = jL.name
        newJoint = jL.joint.referenceToNew
        assert not newJoint == None
        loadsymb = jL.loadsymb
        newLoad = pLoads.JointLoad(name, newJoint, loadsymb)

        newLoadList.append(newLoad)

    # now process the "normal loads"

    for i_load in normalLoadList:

        # mapping from oldJoints to new joints
        newCS1 = i_load.parent.referenceToNew
        newCS2 = i_load.child.referenceToNew
        newCSref = i_load.CSref

        name = i_load.name
        symb = i_load.symb

        if i_load.CSref is not None:
            newCSref =  i_load.CSref.referenceToNew
        else:
            newCSref =  None

        className = i_load.__class__.__name__

        # get the new class as attribute of the module
        # using the fact that the names of the sensorclasses
        # are equal
        newClass = getattr(pLoads, className)

        newLoad=newClass(name, newCS1, newCS2, symb, newCSref)
        newLoadList.append(newLoad)

    return newLoadList

def convertSensors(sensorList):
    """
    converts Sensors from input to processing form
    """

    # special treating of JointSensors (analog to JointLoads):
    # first: separate them
    isJS = lambda sens: isinstance(sens, iSensors.JointSensor)
    isES = lambda sens: isinstance(sens, iSensors.EnergySensor)

    jointSensorList = list(filter(isJS, sensorList))
    energySensorList = list(filter(isES, sensorList))

    isNotJSES = lambda sens: not(isJS(sens) or isES(sens))
    normalSensorList = list(filter(isNotJSES, sensorList))

    assert len(normalSensorList) + len(jointSensorList) + len(energySensorList) == len(sensorList)

    newSensorList=[]

    # process the JointSensors
    for jS in jointSensorList:
        name = jS.name
        newJoint = jS.joint.referenceToNew
        assert not newJoint == None
        symb = jS.symb
        cat = jS.category
        # get the new class as attribute of the module
        # using the fact that the names of the sensorclasses
        # are equal
        className = jS.__class__.__name__
        newClass = getattr(pSensors, className)
        newSensor = newClass(name, newJoint, symb, cat)
        newSensorList.append(newSensor)


    # process energy sensors
    for eS in energySensorList:
        name = eS.name
        newBody = eS.body.referenceToNew;
        assert not newBody == None
        symb = eS.symb
        cat = eS.category
        newSensor = pSensors.EnergySensor(name, newBody, symb, _gravity, cat)
        newSensorList.append(newSensor)


    # now process the "normal Sensors"
    for i_sensor in normalSensorList:

        # mapping from oldJoints to new joints
        newCS1 = i_sensor.parent.referenceToNew
        newCS2 = i_sensor.child.referenceToNew
        name = i_sensor.name
        symb = i_sensor.symb
        cat = i_sensor.category

        if i_sensor.CSref is not None:
            newCSref =  i_sensor.CSref.referenceToNew
        else:
            newCSref =  None

        className = i_sensor.__class__.__name__
        # get the new class as attribute of the module
        # using the fact that the names of the sensorclasses
        # are equal

        newClass = getattr(pSensors, className)

        try:
            newSensor=newClass(name, newCS1, newCS2, symb, newCSref, cat)
        except TypeError:
            raise TypeError("There was an error calling the Constructor of %s"%className)

        newSensorList.append(newSensor)


    return newSensorList

def convertloops(loopList):
    """
    converts loops from input to processing form
    """
    newLoopList=[]
    for i_loop in loopList:
        if isinstance(i_loop, iFourBar):

            # mapping from oldJoints to new joints
            newJoints=[j.referenceToNew for j in i_loop.joints]

            # mapping from oldFrametems to new Frametems
            newCSList=[cs.referenceToNew for cs in i_loop.CSList]

            args=[i_loop.name]+newJoints+newCSList+[i_loop.posture]

            p_fourBar=pFourBar(*args)
            newLoopList.append(p_fourBar)


        elif isinstance(i_loop, iThreeBarTrans):

            # mapping from oldJoints to new joints
            newJoints=[j.referenceToNew for j in i_loop.joints]

            # mapping from oldFrametems to new Frametems
            newCSList=[cs.referenceToNew for cs in i_loop.CSList]

            args=[i_loop.name]+newJoints+newCSList

            p_threeBarTrans=pThreeBarTrans(*args)
            newLoopList.append(p_threeBarTrans)


        elif isinstance(i_loop, iSteering):

            # mapping from oldJoints to new joints
            newJoints=[j.referenceToNew for j in i_loop.joints]

            # mapping from oldFrametems to new Frametems
            newCSList=[cs.referenceToNew for cs in i_loop.CSList]

            args=[i_loop.name]+newJoints+newCSList+[i_loop.setUpH,i_loop.setUpS]

            p_steering=pSteering(*args)
            newLoopList.append(p_steering)


        elif isinstance(i_loop, iCrankSlider):

            # mapping from oldJoints to new joints
            newJoints=[j.referenceToNew for j in i_loop.joints]

            # mapping from oldFrametems to new Frametems
            newCSList=[cs.referenceToNew for cs in i_loop.CSList]

            args=[i_loop.name]+newJoints+newCSList

            p_CrankSlider=pCrankSlider(*args)
            newLoopList.append(p_CrankSlider)

        elif isinstance(i_loop, iHexapod):

            # mapping from oldJoints to new joints
            newJoints=[j.referenceToNew for j in i_loop.joints]

            # mapping from oldFrametems to new Frametems
            newCSList=[cs.referenceToNew for cs in i_loop.CSList]

            args=[i_loop.name]+newJoints+newCSList

            p_Hexapod=pHexapod(*args)
            newLoopList.append(p_Hexapod)

        elif isinstance(i_loop, iHexapod_m_AV):

            # mapping from oldJoints to new joints
            newJoints=[j.referenceToNew for j in i_loop.joints]

            # mapping from oldFrametems to new Frametems
            newCSList=[cs.referenceToNew for cs in i_loop.CSList]

            args=[i_loop.name]+newJoints+newCSList

            p_Hexapod_m_AV=pHexapod_m_AV(*args)
            newLoopList.append(p_Hexapod_m_AV)



        elif isinstance(i_loop, iFourBarTrans):

            # mapping from oldJoints to new joints
            newJoints=[j.referenceToNew for j in i_loop.joints]

            # mapping from oldFrametems to new Frametems
            newCSList=[cs.referenceToNew for cs in i_loop.CSList]

            args=[i_loop.name]+newJoints+newCSList

            p_fourBarTrans=pFourBarTrans(*args)
            newLoopList.append(p_fourBarTrans)


        elif isinstance(i_loop, iTransmission):

            # mapping from oldJoints to new joints
            newJoints=[j.referenceToNew for j in i_loop.joints]
            args=[i_loop.name]+newJoints+[i_loop.ratio]

            p_transmission=pTransmission(*args)
            newLoopList.append(p_transmission)

        elif isinstance(i_loop, iExpJoint):

            # mapping from oldJoints to new joints
            newJoints=[j.referenceToNew for j in i_loop.joints]
            args=[i_loop.name]+newJoints+[i_loop.exp]

            p_expJoint=pExpJoint(*args)
            newLoopList.append(p_expJoint)

        else:
            if (i_loop is not None):
                print("Constraint '" + str(i_loop) + "' which is a '" + str(i_loop.__class__) + "' is unknown!")
            raise NotImplementedError
    # End of for i_loop in loopList:

    return newLoopList





def flattenCSStructure(iBody):
    """
    transform all nested CS so that they are defined w.r.t
    the body where they belong to
    """

    # create a copy
    csList=iBody.coordList[:]

    while len(csList) > 0:
        # take the first element
        toplevelCS=csList.pop(0)

        while len(toplevelCS.coordList) > 0:
            # there are some subLevel CS

            # we pop from the original list
            # because we want to erase the CS from there
            sublevelCS=toplevelCS.coordList.pop(0)

            redefineCS(sublevelCS, iBody, toplevelCS.p, toplevelCS.R)

            # the sublevelCS has now become toplevel
            # its children have to be treated as well:
            csList.append(sublevelCS)


def redefineCS(cs, body, p, R):
    """
    alter the definition of a CS
    so that it will be a toplevel CS of body
    by applying p and R
    """

    cs.p=(p+R*cs.p)
    cs.R=(R*cs.R)
    body._insertCS(cs)

    # CS 25.06.2011, Bugfix: Also change the parent of cs, otherwise it will
    # still point to the old coordinate system
    cs.parent = body

    # change cg if it is a body (which was connected to a fixed joint)
    # TODO: Check that
    if (cs.__class__ is iBody):
        cs.cg = R*cs.cg + cs.p
        cs.inertia = transpose(R)*cs.inertia*R


def updateChildCS(new_Body, p_bif, R_bif):
    """
    iterates over all child CS
    (which are still defined w.r.t. the old bif)
    and defines them with respect to jef (= the new body inertial frame)
    """
    for cs in new_Body.children:
        assert isinstance(cs, pFrame)

        cs.p = p_bif+ R_bif*cs.p
        cs.R = R_bif*cs.R


def applyFixedJoints(iBody, stage):
    """
    Merge all bodies, interconnected by fixed joints, into one.
    """

    # stage == 1
    # Convert Bodies into Frames
    if (stage == 1):
        # Run through all fixed Joints
        for jointName, joint in iBody.jointDict.items():
            if (joint.dof is None):
                # Now we have a fixed joint

                # 1. Connect Bodies by adding Child-Coordinate System to parent
                parent = joint.parent
                child = joint.child
                body = child.parent
                parent.children.append(child)
                parent.coordList.append(child)

                # 3. Update all Coordinate System Names of Child Body
                for cs in body.coordList:
                    cs.name = "%s%s"%(body.name,cs.name)


                # 4. Update Information of Child and Child's Body
                body.p = -child.p
                body.R = transpose(child.R)
                body.children.remove(child)
                body.coordList.remove(child)
                body.oldName = body.name
                body.name = "_body_%s"%body.name     # Put a '_body_' in front of the body's name in order to avoid double names
                child.p = Matrix([0,0,0])
                child.R = eye(3)
                child.parent = None
                child.children.append(body)
                child.coordList.append(body)
                joint.child = body

    # stage == 2
    # Calculate new Mass, Cg, Inertia
    if (stage == 2):
        # List of Joints to delete
        delList = []
        for jointName, joint in iBody.jointDict.items():
            if (joint.dof is None):
                # Now we have a fixed joint
                delList.append(jointName)

                # Get Parent and Child Body
                parent = joint.parent.parent
                child = joint.child

                # Remove Child Body from List
                del iBody.bodyDict[child.oldName]

                # Calculate new Properties of Combined Body
                m = parent.mass + child.mass;
                cg = (parent.mass*parent.cg + child.mass*child.cg) / m;
                # Inertia
                inertia = parent.inertia + child.inertia + child.mass*transpose(skew(child.cg))*skew(child.cg)

                # save new values
                parent.mass = m
                parent.cg = cg
                parent.inertia = inertia

        # delete all items from delList
        for item in delList:
            del iBody.jointDict[item]


# avoid importing * in MbsSystem nor every function by its own
# todo: is this needed any longer? Seems to be deprecated, see __init__.py here
class PublicMethods(object):
    """
    Collects all functions that are ought to be used by other modules

    These functions are declared as static methods hence this class in not
    intented to be instantiated.
    """

    def __init__(self):
        raise TypeError("This class is a collection of static methods."\
                         "It is not intented to be instantiated.")



    # calculate the equations of Motion
    @staticmethod
    def genEquations(iWorld, mode, diff=0, kinematicsOnly=False, \
                     graphOptimizations=False):
        assert isinstance(iWorld, _MbsSystemType)

        _Graph = iWorld.graph

        print("Transforming Model ...")
        _transformModel(iWorld)


        print("Obtaining Equations of Motion ...")

        # Select Generator
        if (mode == "explicit"):
            gen = Generator_Explicit('Generator_Explicit', _Graph, _pInertialBody,
                                     _gravity, sensors=_sensors,
                                     loops=_loops, diff=diff,
                                     kinematicsOnly=kinematicsOnly)
        elif (mode == "recursive"):
            gen = Generator_Recursive('Generator_Recursive', _Graph, _pInertialBody,
                                      _gravity, sensors=_sensors,
                                      loops =_loops, diff=diff,
                                      kinematicsOnly=kinematicsOnly)
        elif (mode == "ordern"):
            gen = Generator_OrderN('Generator_OrderN', _Graph, _pInertialBody,
                                      _gravity, sensors=_sensors,
                                      loops =_loops, diff=diff,
                                      kinematicsOnly=kinematicsOnly)
        else:
            raise ValueError("genEquations: Unknown Mode '%s'"%mode)

        # Do the calculations
        gen.getSystemEqns()
        iWorld.state = gen.state

        # Build the Graph
        print("Building Graph with optimization %s"%str(graphOptimizations))
        t = _Graph.buildGraph( graphOptimizations )

        # OK
        return True, t

    # generate code from the equations
    @staticmethod
    def genCode(iWorld, typestr, modelname, dirname='.', **kwargs):

        # create directory, if necessary
        dirname = os.path.realpath(dirname)
        if not os.path.exists(dirname):
            print('Directory "%s" does not exist. Creating it...'%dirname)
            os.makedirs(dirname)

        # get Graph
        _Graph = iWorld.graph

        # call Output
        return _Graph.writeCode(typestr, modelname, dirname, **kwargs)


    @staticmethod
    def show( model, modelname, options, **kwargs):
        from pymbs.input import MbsSystem
        assert isinstance(model, MbsSystem)

        _Graph = model.graph

        # Write Python Code
        print("Writing model code for GUI to temporary directory: '%s'"%tempfile.gettempdir())
        #graphics = CodeGenerator('py', modelname, _Graph, tempfile.gettempdir())
        #graphics.genVisualisationSensors()
        #graphics.genStateDerivative()

        _Graph.writeCode('py', modelname, tempfile.gettempdir())


        # this is the list of graphRep objects:
        grList = list(model.graphRepDict.values())

        launchGui(grList, _Graph, modelname, _gravity,
                  model.state, options, **kwargs)


    @staticmethod
    def genSarturisXml(model, modelname, dirname = '.'):

        grList = list(model.graphRepDict.values())

        save = CodeGenerator('xml', modelname, _Graph, dirname)
        save.genSarturisScenegraph(grList)

    @staticmethod
    def genMatlabAnimation(model, modelname, dirname = '.', **kwargs):

        # this is the list of graphRep objects:
        grList = list(model.graphRepDict.values())

        graphics = CodeGenerator('m', modelname, _Graph, dirname)
        graphics.genVisualisationSensors()
        graphics.genAnimation(grList, _gravity, **kwargs)

        # try to delete 'stl.mat', otherwise graphics never get updated
        try:
            os.remove(os.path.join(dirname, 'stl.mat'))
        #TODO: which exception??
        except:
            # Ignore file not found
            pass

    @staticmethod
    def exportGraphReps(model, fileName):

        #check whether _Graph was already created        
        try:
            _Graph = model.graph
        except NameError:
            raise PyMbsError('Cannot export graph reps. You have to call ' \
                             'the <genEquations()> method of your MbsSystem-'\
                             'instance before calling <exportGraphReps()>.')

        grList = list(model.graphRepDict.values())

        if not grList:
            raise PyMbsError('Cannot export graphics. There are no '\
                             'visualisation objects defined. Call '\
                             '<addVisualisation.xyz()> of your MbsSystem-'\
                             'instance to add 3d objects representing the '\
                             'bodies of your mbs.')

        exporter = GraphRepWriter(grList, _Graph, fileName)
        exporter.save()