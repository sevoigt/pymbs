# -*- coding: utf-8 -*-
"""
Created on 30.07.2009

@author: Christian Schubert

Use Matlab for simplification of expressions
"""

from pymbs.symbolics import Matrix
from sympy import sympify

DO_NOT_SIMPLIFY = False
USE_MATLAB = True

try:

    if (not DO_NOT_SIMPLIFY):

        if (USE_MATLAB):
            import pymat

            # If pymat is installed
            class Simplification(object):
                """
                Simplification using MATLAB
                """

                def __init__(self):
                    """
                    Open connection to MATLAB
                    """
                    print('PyMat is installed. Trying to start MATLAB ...')
                    self.handle = pymat.open()
                    print('done')
                    print('Using MATLAB for Simplifications')


                def __del__(self):
                    """
                    Close connection to MATLAB
                    """
                    import pymat
                    pymat.close(self.handle)


                def simple(self, exp):
                    """
                    Simplify a given expression
                    """
                    if (isinstance(exp, Matrix)):
                        return exp.applyfunc(self.simple)

                    # Convert term to string
                    strExp = str(exp).replace('**', '^')

                    # Save locals for sympify, to guarantee that variables are the same
                    # TODO: And functions?
                    atoms = list( strExp.atoms )
                    local_dict = list(zip( [str(a) for a in atoms], atoms ))

                    # Communicate with MATLAB
                    pymat.put(self.handle, 'temp', strExp)          # Save expression as string
                    pymat.eval(self.handle, 's = sym(temp);')       # Convert it into a symbolic expression
                    pymat.eval(self.handle, 's = simple(s);')       # Simplification
                    pymat.eval(self.handle, 'temp = char(s);')      # Convert it back into a string
                    strNewExp = pymat.get(self.handle, 'temp')      # Get the string variable                    

                    strNewExp = strNewExp.replace('^', '**')        # convert ^ back into **
                    strNewExp = strNewExp.replace('csgn(', 'sign(') # sometimes there is a funny csgn - function
                    newExp = sympify(strNewExp, locals=local_dict)  # Sympify the string variable
                    return newExp
        else:   # USE_MATLAB == FALSE
            raise ImportError

    else: # DO_NOT_SIMPLIFY

        class Simplification(object):
            def __init__(self):
                """
                Constructor: do nothing
                """
                print('Simplification disabled.')

            def simple(self, exp):
                return exp

except ImportError:

    # if it is not installed
    class Simplification(object):
        """
        Simplification using pymbs.symbolics
        """

        def __init__(self):
            """
            Constructor: do nothing
            """
            print('PyMat and MATLAB are not available. Using pymbs.symbolics for simplifications')

        def simple(self, exp):
            """
            Simplify a given expression
            """
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
        print('Simplification failed')
        print(exp)
        print('----------------- TRACEBACK -----------------')
        traceback.print_exc()
        print('---------------------------------------------')
        pass

    return exp
