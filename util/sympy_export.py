# -*- coding: utf-8 -*-
'''
'''


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
Created on 09.07.2014

@author: kunze
'''

import os
from sympy import *


def sympymbs(world):
    '''
    Returns a callable function to be used with PyTrajectory out of 
    exported motion equations from PyMbs.
    
    Parameters
    ----------
    
    world : PyMbs.Input.MbsSystem.MbsSystem
		Reference to a MbsSystem object
    
    Returns
    -------
    
    Callable function for the vectorfield of the multibody system.
    '''
    
    # check type
    #assert( isinstance(world, PyMbs.Input.MbsSystem.MbsSystem) )
    
    # get motion equations, parameters and controllers
    eqns_mo = world.getMotionEquations()
    parameters = world.getParameters()
    controller = world.getController()
    
    # at first we create a buffer for the string that we complete and execute 
    # to dynamically define a function and return it
    fnc_str_buffer ='''
def f(x, u):
    
    # System variables
    %s  # x_str
    %s  # q_str
    %s  # u_str
    %s  # control_str
    
    # Parameters
    %s  # par_str
    
    # Vectorfield
    %s  # ff_str
    
    return ff
'''
    
    
    # the given motion equations (which are basically symbolics.CAssignments from PyMbs)
    # should represent equations that look something like this:
    #
    #   der_q = qd
    #   matrix([[q_joint_1_Tx],[q_joint_3_Ry]]) = q
    #   cosq_joint_3_Ry = cos(q_joint_3_Ry)
    #   sinq_joint_3_Ry = sin(q_joint_3_Ry)
    #   T_Last = matrix([[cosq_joint_3_Ry,0,sinq_joint_3_Ry],
    #                    [0,1,0],
	#                    [(-sinq_joint_3_Ry),0,cosq_joint_3_Ry]])
    # ...
    #   M_ = ...
    # ...
    #   matrix([[qd_joint_1_Tx],[qd_joint_3_Ry]]) = qd
    # ...
    #   C_ = ...
    #   qdd = matrix(linalg.solve(M_,(-C_)))
    #   der_qd = qdd
    
    # so there are different kinds of equations that require different handling
    
    # first, those containing information about the final vectorfield
    #
    # in the example above: 'der_q = qd' and 'der_qd = qdd'
    # they will be stored in
    ff = []
    
    # then, those containing information about the system state variables (dimension) 
    # and how to unpack them
    #
    # in the above example: 'matrix([[q_joint_1_Tx],[q_joint_3_Ry]]) = q'
    #                   and 'matrix([[qd_joint_1_Tx],[qd_joint_3_Ry]]) = qd'
    x_eqns = []
    
    # and last but not least, those defining the variables/parameters used in the various equations
    # like:  'cosq_joint_3_Ry = cos(q_joint_3_Ry)'
    #     or 'T_Last = matrix([[cosq_joint_3_Ry,0,sinq_joint_3_Ry],
    #                         [0,1,0],
	#                         [(-sinq_joint_3_Ry),0,cosq_joint_3_Ry]])'
    par_eqns = []
    
    for eqn in eqns_mo:
        # get left hand site of the motion equation
        lhs = str(eqn.lhs)
        # and right hand site, removing possible newline character 
        # that come from the string representation
        rhs = str(eqn.rhs).replace(os.linesep, '')
        
        if lhs.startswith('der'):
            # equation looks like:  'der_q = qd'
            # and therefor contains information about vectorfield
            ff.append(rhs)
        elif rhs.startswith('q') and not lhs.startswith('der'):
            # equation looks like:  'matrix([[q_joint_1_Tx],[q_joint_3_Ry]]) = q'
            # and therefor contains information about the system state variables 
            # such as q, qd and so on
            x_eqns.append([lhs, rhs])
        else:
            # all other equations that contain information about other parameters and variables
            par_eqns.append([lhs, rhs])
    
    #################################
    # handle system state variables #
    #################################
    # --> leads to x_str which shows how to unpack the state variables
    #     and q_str which is just a vice versa string of the equations in x_eqns
    #     (necessary for par_eqns)
    x_str = ''
    q_str = ''
    for s in x_eqns:
        # s[0] is the left hand site of the equation
        # and s[1] its right hand site
        if s[0].startswith('matrix'):
            # if the left hand site starts with 'matrix' the state variable on rhs 
            # has a dimension > 1 and the equation looks something like:
            # 
            #   'matrix([[q_joint_1_Tx],[q_joint_3_Ry]]) = q'
            # 
            # to append the variables on lhs (here: q_joint_1_Tx and q_joint_3_Ry)
            # we create a sympy matrix of symbols and than iterate through its elements
            # and append their string representations to x_str
            Ml = S(s[0], locals={"matrix":Matrix})
            for x in Ml:
                x_str += '%s, '%str(x)
        else:
            # if left hand site is a scalar, we can just append its string representation
            x_str += '%s, '%str(s[0])
        
        # here we reverse the equation and append it to q_str
        # we have to replace 'matrix' with 'Matrix' because this is the
        # name of the sympy matrix class
        q_str += '%s = %s; '%(s[1], s[0].replace('matrix', 'Matrix'))
    
    # as a last we remove the trailing '; ' to avoid syntax erros
    x_str = x_str[:-2] + ' = x' # x_str now should look like:   'q_joint_1_Tx, q_joint_3_Ry, ... = x'
    q_str = q_str[:-2]          # q_str is just a concatenation of reversed x_eqns
    
    ##########################
    # handle input variables #
    ##########################
    # --> leads to u_str and control_str
    
    # u_str will show how to unpack the inputs of the control system
    u_str = ''
    # control_str will show how to create a matrix for every input
    # depending on its dimension which is necessary for the later evaluation
    # of the par_eqns
    control_str = ''
    j = 0 # used for indexing
    for k, v in list(controller.items()):
        # 'k' is the name or string representation of the input
        # 'v' is its dimension tuple (e.g. v = (3,1) )
        control_str += str(k) + ' = Matrix(['
        for i in range(v[0]*v[1]):
            # add an 'u_i' to u_str
            # --> they will be the unpacked inputs in the later evaluation
            #     of the function returned by this method (that means sympymbs)
            u_str += 'u%d, '%(j+i)
            # and append this 'u_i' as an element of the current input 'k'
            control_str += 'u%d, '%(j+i)
        
        # remember number of inputs so far so that u_str will 
        # not look like: 'u1, u2, u3, u1, u2, u3 = u'
        # but like:      'u1, u2, u3, u4, u5, u6 = u'
        j += i+1
        
        # in case we have on input 'F' of dimension 3 ( 'v = (3,1)' ), then
        # control_str will look like:
        #   'F = Matrix([u1, u2, u3]).reshape(1,3).T'
        control_str = control_str[:-2] + ']).reshape(%d, %d).T; '%(v[1], v[0])
    
    # after we remove the trailing '; ' to avoid syntax errors x_str will look like:
    #   'u1, u2, ... , um = u'
    u_str = u_str[:-2] + ' = u'
    
    ############################
    # handle system parameters #
    ############################
    # --> leads to par_str
    par_str = ''
    for k,v in list(parameters.items()):
        # 'k' is the name of a system parameter such as mass or gravitational acceleration
        # 'v' is its value in SI units
        par_str += '%s = %s; '%(str(k), str(v))
    
    for pe in par_eqns:
        # par_eqns contains equations that define additional variables such as:
        #
        #   'cosq_joint_3_Ry = cos(q_joint_3_Ry)'
        #
        # where 'cosq_joint_3_Ry' will be used in other equations
        if pe[1].startswith('matrix(linalg.solve('):
            # if the variable is the solution of a equation system we need additional handling
            #
            # the equation looks like
            #
            #   'qdd = matrix(linalg.solve(M_,(-C_)))'
            # 
            # to get a symbolic solution of this system that can be written back into the
            # par_str in string form we first have to extract the two matrices 
            # ('M_' and '(-C_)' in the example above)
            tmp = pe[1][20:-2]
            tmp1, tmp2 = tmp.split(',')
            # tmp1 is now: 'M_'
            # and tmp1 is: '(-C_)'
            # 
            # then we have to create a sympy.Symbol for every variable that could be
            # part of the matrices assignments in par_str
            # remember: C_ would be assigned to something like
            # 
            #   C_ = matrix([[(matrix([[1,0,0]]) * (int_G_C_body_1 + int_G_C_body_3))[0,0]],
			#	             [(matrix([[0,1,0]]) * int_L_C_body_3)[0,0]]])
            # 
            # so we would need symbols for 'int_G_C_body_1', 'int_G_C_body_3' and 'int_L_C_body_3'
            #
            # therefor we first execute x_str with replaced right hand site
            # ('... = x' becomes '... = symbols(...)')
            # to get symbols for the system variables (e.g. q_joint_1_Tx' and 'q_joint_3_Ry')
            exec(x_str.split('=')[0] + '= symbols("%s")'%x_str.split('=')[0])
            # then we execute our q_str to get symbols for the variables 'q', 'qd' and such which
            # are used in the equations of par_str
            exec(q_str)
            # next we have to create a sympy.MatrixSymbol for every input of the system
            # because they cannot be scalar symbols 
            for c,s in list(controller.items()):
                # 'c' is the name of the input and 's' its shape
                exec(c + ' = MatrixSymbol("%s",%d,%d)'%(c, s[0], s[1]))
            # now we can execute the par_str we built so far and have all symbols we need to get
            # a symbolic solution of the equation system
            exec(par_str)
            # this is done by simply calling for example
            # 'M_.solve(-C_)'
            exec('TMP = %s.solve(%s)'%(tmp1, tmp2))
            # the solution was stored in TMP and is added to par_str in string form
            par_str += '%s = %s; '%(pe[0], str(TMP))
        else:
            # if the variable is not solution of a equation system
            # we simply can add the lhs and rhs in 'pe' as an equaion to par_str 
            # (again replacing 'matrix' with 'Matrix' because that is the sympy method's name)
            par_str += '%s = %s; '%(pe[0], pe[1].replace('matrix', 'Matrix'))
    
    # as a last we remove the trailing '; ' from par_str to avoid syntax errors
    par_str = par_str[:-2]
    
    ######################
    # create vectorfield #
    ######################
    # --> leads to ff_str
    ff_str = 'ff = Matrix.vstack('
    
    for w in ff:
        ff_str += w + ', '
    
    ff_str = ff_str[:-2] + ')'
    
    ############################
    # Create callable function #
    ############################
    # now we can replace all placeholders in the function string buffer
    fnc_str = fnc_str_buffer%(x_str, q_str, u_str, control_str, par_str, ff_str)
    # and finally execute it which will create a python function 'f'
    exec(fnc_str)
    
    # now we have defined a callable function that can be used within PyTrajectory
    return f
