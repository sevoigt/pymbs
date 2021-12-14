/*
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
*/

#ifndef __CASSIGNMENT_H_
#define __CASSIGNMENT_H_

#include <Python.h>
#include <stdexcept>
#include "Symbolics.h"
#include "Graph.h"

using namespace Symbolics;

namespace Symbolics
{
    namespace Python
    {
        // CAssignmentObject definieren
        typedef struct _CAssignment_Object_: PyObject {
            PyObject_HEAD
            Symbolics::Graph::Assignment m_Assignment;
			_CAssignment_Object_(Symbolics::Graph::Assignment ass);
        } CAssignmentObject;

        extern PyTypeObject CAssignmentObjectType;
    };
};


#endif // __CASSIGNMENT_H_