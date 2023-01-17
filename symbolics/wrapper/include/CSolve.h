#ifndef __CSolve_H_
#define __CSolve_H_

#include <Python.h>
#include "Symbolics.h"
#include "CBasic.h"

using namespace Symbolics;

namespace Symbolics
{
    namespace Python
    {
        struct _CSolve_Object_ : public CBasicObject  
        {
        };
        
        typedef struct _CSolve_Object_ CSolveObject;

        extern PyTypeObject CSolveObjectType;
    };
};

#endif // __CSolve_H_