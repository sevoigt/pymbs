#ifndef __COuter_H_
#define __COuter_H_

#include <Python.h>
#include "Symbolics.h"
#include "CBasic.h"

using namespace Symbolics;

namespace Symbolics
{
    namespace Python
    {
        struct _COuter_Object_ : public CBasicObject  
        {
        };
        
        typedef struct _COuter_Object_ COuterObject;

        extern PyTypeObject COuterObjectType;
    };
};

#endif // __COuter_H_