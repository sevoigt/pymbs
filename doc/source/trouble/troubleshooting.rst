Troubleshooting
===============

In this chapter we rty to gather all problems which might occur during the
usage of PyMbs and how they can be solved.

Error: PyScripter throws strange errors in Gui.py
-------------------------------------------------

Unfortunately, PyScripter does not reinitialise its Python engine properly
which can cause very strange errors. You have the following options to resolve
this:

* Restart PyScripter
* Use remote Python Engine (Run -> Python Engine -> Remote) which can be
  reinitialised
* Use a different editor

These strange errors are most commonly caused by a module that has been already
imported and have changed (automatically or manually) since as is the case with
some parts of the GUI.  The Python engine does not recognise this and does not
reload the module. Hence you are still working with the old version which can
lead to unexpected behaviour.

Anything Else
-------------

Please contact Christian (christian.schubert @ <tud>). He will be happy to 
assist you.

<tud> = tu-dresden.de
