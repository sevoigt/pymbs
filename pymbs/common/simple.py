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
Created on 30.07.2009

@author: Christian Schubert
'''

#from PyMbs.Symbolics import Matrix, sympify, trigsimp

DO_NOT_SIMPLIFY = False
USE_MATLAB = True

try:

    if (not DO_NOT_SIMPLIFY):

        if (USE_MATLAB):
            import pymat

            # if pymat is installed
            class Simplification(object):
                '''
                Simplification using MATLAB
                '''


                def __init__(self):
                    '''
                    Open Connection to MATLAB
                    '''
                    print("PyMat is installed. Trying to start MATLAB ...")
                    self.handle = pymat.open()
                    print("done")
                    print("Using MATLAB for Simplifications")


                def __del__(self):
                    '''
                    Close Connection to MATLAB
                    '''
                    import pymat
                    pymat.close(self.handle)


                def simple(self, exp):
                    '''
                    Simplify a Given Expression
                    '''
                    if (isinstance(exp, Matrix)):
                        return exp.applyfunc(self.simple)

                    # Convert term to string
                    strExp = str(exp).replace("**", "^")

                    # Save locals for sympify, to guarantee that variables are the same
                    # TODO: And functions?
                    atoms = list( strExp.atoms )
                    local_dict = list(zip( [str(a) for a in atoms], atoms ))

                    # Communicate with MATLAB
                    pymat.put(self.handle, 'temp', strExp)          # Save Expression as String
                    pymat.eval(self.handle, 's = sym(temp);')       # Convert it into a Symbolic Expression
                    pymat.eval(self.handle, 's = simple(s);')       # Simplification
                    pymat.eval(self.handle, 'temp = char(s);')      # Convert it back into a String
                    strNewExp = pymat.get(self.handle, 'temp')      # Get the String Variable                    

                    strNewExp = strNewExp.replace("^", "**")        # convert ^ back into **
                    strNewExp = strNewExp.replace("csgn(", "sign(") # sometimes there is a funny csgn - function
                    newExp = sympify(strNewExp, locals=local_dict)  # Sympify the String Variable
                    return newExp
        else:   # USE_MATLAB == FALSE
            raise ImportError

    else: # DO_NOT_SIMPLIFY

        class Simplification(object):
            def __init__(self):
                '''
                Constructor: do nothing
                '''
                print("Simplification disabled.")

            def simple(self, exp):
                return exp

except ImportError:

    # if it is not installed
    class Simplification(object):
        '''
        Simplification using PyMbs.Symbolics
        '''

        def __init__(self):
            '''
            Constructor: do nothing
            '''
            print("PyMat and MATLAB are not available. Using PyMbs.Symbolics for Simplifications")

        def simple(self, exp):
            '''
            Simplify a Given Expression
            '''
            if (DO_NOT_SIMPLIFY):
                return exp
            
            exp.simplify()
            return exp

# UnboundLocalError: local variable '_SimpleObject' referenced before assignment
#_SimpleObject = None
#def simple(exp):
#    if _SimpleObject is None:
#        _SimpleObject = Simplification()
#
#    return _SimpleObject.simple(exp)

def simple(exp):
    global _SimpleObject

    if ('_SimpleObject' not in globals()):
        _SimpleObject = Simplification()

    try:
        exp = _SimpleObject.simple(exp)
    except:
        import traceback
        print("Simplification failed")
        print(exp)
        print("----------------- TRACEBACK -----------------")
        traceback.print_exc()
        print("---------------------------------------------")
        pass

    return exp
