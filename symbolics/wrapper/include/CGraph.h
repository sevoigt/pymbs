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