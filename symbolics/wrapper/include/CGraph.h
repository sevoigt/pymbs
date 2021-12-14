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

#ifndef __CGRAPH_H_
#define __CGRAPH_H_

#include <Python.h>
#include "Symbolics.h"
#include "Graph.h"
#include <structmember.h>

using namespace Symbolics;

namespace Symbolics
{
    namespace Python
    {
        // CGraphObject definieren
        typedef struct _CGraph_Object_: PyObject {
            Graph::Graph *m_graph;
        } CGraphObject;

        extern PyTypeObject CGraphObjectType;
    };
};

#endif // __CGRAPH_H_