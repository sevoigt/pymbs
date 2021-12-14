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
Created on 30.08.2009

@author: Carsten Knoll


This module contains some usefull decorators
(for debugging)

'''

from sys import stdout,stderr

# copied from
# http://wiki.python.org/moin/PythonDecoratorLibrary#DifferentDecoratorForms
class DecoTraceWithArgs(object):
    '''decorator class with ARGUMENTS

       This can be used for unbounded functions and methods.  If this wraps a
       class instance, then extract it and pass to the wrapped method as the
       first arg.
    '''

    def __init__(self, *dec_args, **dec_kw):
        '''The decorator arguments are passed here.  Save them for runtime.'''
        self.dec_args = dec_args
        self.dec_kw = dec_kw

        self.label = dec_kw.get('label', 'T')
        self.fid = dec_kw.get('stream', stderr)

    def _showargs(self, *fargs, **kw):

        print('%s: enter %s with args=%s, kw=%s' % (self.label, self.f.__name__, str(fargs), str(kw)), file=self.fid)
        print('%s:   passing decorator args=%s, kw=%s' % (self.label, str(self.dec_args), str(self.dec_kw)), file=self.fid)

    def _aftercall(self, status):
        print('%s: exit %s with status=%s' % (self.label, self.f.__name__, str(status)), file=self.fid)
    def _showinstance(self, instance):
        print('%s: instance=%s' % (self.label, instance), file=self.fid)

    def __call__(self, f):
        def wrapper(*fargs, **kw):
            '''
              Combine decorator arguments and function arguments and pass to wrapped
              class instance-aware function/method.

              Note: the first argument cannot be "self" because we get a parse error
              "takes at least 1 argument" unless the instance is actually included in
              the argument list, which is redundant.  If this wraps a class instance,
              the "self" will be the first argument.
            '''

            self._showargs(*fargs, **kw)

            # merge decorator keywords into the kw argument list
            kw.update(self.dec_kw)

            # Does this wrap a class instance?
            if (fargs and getattr(fargs[0], '__class__', None)):

                # pull out the instance and combine function and
                # decorator args
                instance, fargs = fargs[0], fargs[1:]+self.dec_args
                self._showinstance(instance)

                # call the method
                ret=f(instance, *fargs, **kw)
            else:
                # just send in the give args and kw
                ret=f(*(fargs + self.dec_args), **kw)

            self._aftercall(ret)
            return ret

        # Save wrapped function reference
        self.f = f
        wrapper.__name__ = f.__name__
        wrapper.__dict__.update(f.__dict__)
        wrapper.__doc__ = f.__doc__
        return wrapper


# adapt the decorator for our purposes:

class DecoAssertNotNan(DecoTraceWithArgs):
    def __init__(self, *dec_args, **dec_kw):
        self.instanceList = []
        DecoTraceWithArgs.__init__(self, *dec_args, **dec_kw)

    # avoid spamming
    def _showargs(self, *fargs, **kw):
        pass

    def _showinstance(self, instance):
        # save the instance name
        # too show it in case some trouble occurs

        self.instanceList.append(instance)
        pass


    def _aftercall(self, status):
        """this is called after finishing the call to the wrapped method

        status:  its return value which is examined here
        """
        if str(status) == 'nan':
            print("problematic instanceList:", self.instanceList)

        self.instanceList.pop()
        assert  not str(status) == 'nan'

