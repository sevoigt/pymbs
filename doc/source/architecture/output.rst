Output
======

The output module is used to create code from a given set of equations for a
specific target language. PyMbs is capable of generating code for

* Python
* Matlab
* Modelica
* C/C++
* Fortran.

This section will explain the mechanism of code generation and enable
you to create custom writers for languages not listed above. Therefore we
strongly advise you to take a look at the source code of the involved classes
to get a better understanding of how things work.


The Code Generation Process
---------------------------

Code generation is now handled in the C++ backend of PyMbs and unfortunately
has not been documented yet.
