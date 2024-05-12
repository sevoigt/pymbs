Development Ressources
======================

This section provides some entry points for further development of PyMbs.

Adding new code-export targets
------------------------------

1. Implement the xxxPrinter.h and xxxPrinter.cpp for the export target.
The printer converts the symbolic expressions to strings for the target.
Usually the target languages don't have native linear algebra support. Thus
the printer must incorporate the library that is used in the final code
such as numpy arrays in Python.

2. Implement the xxxWriter.h and the xxxWriter.cpp for the export target.
The writer creates the actual code files with functions for the state
derivative, sensors, visuals etc. The code created here will be the actual
running model in the target language.

3. Add the new writer to CGraph_writeOutput() in CGraph.cpp and provide a
string identifier for the target. Usually the file extension of the target's
source code files is used, such as "py" for Python.

4. Add a function to the class GenCode in mbs_system.py to access the target
from the PyMbs model description file. Make sure to use the same string
identifier as in step 3.

5. Test, test, test. The export shall work with as many examples as possible
but at least for the basic ones in the examples/ root folder.