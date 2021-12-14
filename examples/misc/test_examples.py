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

# call testExamples.py without filename: test all automatically, suppress gui
# call testExamples.py with filename: check specific example with visualization
#
# save dictionary to file


import sys
import glob
import json
from PyMbs.Input import MbsSystem

try:
    from colorama import Fore
except ImportError:
    without_colorama = True
else:
    without_colorama = False

try:
    with open('testResults.json', 'r') as f:
        results = json.loads(f.read())
except IOError:
    print('No previous test results found; will generate new file')
    results = {}
except ValueError:
    print('Previous results could not be read; will generate new file')
    results = {}

files = glob.glob('*.py')
# Files in directory that aren't examples
files.remove('test.py')
files.remove('testExamples.py')
files.remove('profile.py')
files.remove('__init__.py')

# called without filename
if len(sys.argv) == 1:
    # Mock GUI call, to iterate over all examples without the need to close
    # each window
    MbsSystem.show = lambda *args, **kwargs: True

    passed = 0
    for filename in files:
        if without_colorama:
            print(filename)
        else:
            print(Fore.GREEN + filename + Fore.RESET)
        try:
            exec(compile(open(filename).read(), filename, 'exec'))
        except:
            print(sys.exc_info()[0])
            # if result key for file exists, don't overwrite second entry
            if results.get(filename) == None:
                results[filename] = ["FAILED", "NOT VALIDATED"]
            else:
                results[filename][0] = "FAILED"
        else:
            passed += 1
            if results.get(filename) == None:
                results[filename] = ["PASSED", "NOT VALIDATED"]
            else:
                results[filename][0] = "PASSED"

    print(str((passed/len(files))*100) + "% have passed.")
    with open('testResults.json', 'w') as f:
        f.write(json.dumps(results, indent=1))

# called with filename
elif len(sys.argv) == 2:
    filename = sys.argv[1]
    exec(compile(open(filename).read(), filename, 'exec'))
    status = eval(input("Did the example run succesfully (1) or not (2)?"))
    if status == 1:
        results[filename] = ["PASSED", "WORKS"]
    elif status == 2:
        results[filename] = ["PASSED", "DOESN'T WORK"]
    else:
        print("Incorrect input")
    with open('testResults.json', 'w') as f:
        f.write(json.dumps(results, indent=1))

else:
    print("Incorrect command line arguments")
