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

#! /usr/bin/python
from distutils.core import setup

## == Possible Arguments
# python setup.py sdist
# python setup.py install
# python setup.py bdist_wininst
# python setup.py bdist_rpm
# python setup.py bdist --help-formats

## Build binary module

'''
# This setup.py only ships the precompiled *.pyd for Windows
try:
    from .builder import Builder
except Exception: #ImportError
    from builder import Builder

builder = Builder('symbolics', 'Symbolics', 'Symbolics/SympyWrapper')
bin_mod = builder.all(buildType = 'Release')
'''

## == Build package
setup(name = 'PyMbs',
      version = '0.2.2',
      description = 'A Python library for modelling multibody systems.',
      long_description = '''\
PyMbs is a Python library for modelling holonomic multibody systems.\
It is able to generate simulation code for Python, Modelica and Matlab.
''',
      author = 'Carsten Knoll, Christian Schubert, Jens Frenkel, Sebastian Voigt',
      author_email = 'christian.schubert@tu-dresden.de',
      url = 'http://sourceforge.net/projects/pymbs/',
      license = 'LGPL',
      packages = ['PyMbs',
                  'PyMbs.Common',
                  'PyMbs.Graphics', \
                  'PyMbs.Input',
                  'PyMbs.Symbolics',
                  'PyMbs.Symbolics.SympyWrapper',
                  'PyMbs.Test', \
                  'PyMbs.Processing',
                  'PyMbs.Processing.LoadElements', \
                  'PyMbs.Processing.Loops', \
                  'PyMbs.Processing.Sensors', \
                  'PyMbs.Examples'],
      package_dir = {'PyMbs': ''},
      package_data = {'PyMbs': ['COPYING', 'COPYING.LESSER',
                                #bin_mod,
                                'utils/FMI/include/*',
                                'utils/FMI/template/*',
                                'Symbolics/symbolics.pyd',
                                'doc/PyMbs.pdf'
                                ]},
      provides = 'PyMbs',
      requires = ['PyMbs.Symbolics (>=0.6.7)'],
     )

