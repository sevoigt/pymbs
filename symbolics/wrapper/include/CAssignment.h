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