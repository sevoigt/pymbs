#ifndef __CLESS_H_
#define __CLESS_H_

#include <Python.h>
#include "Symbolics.h"
#include "CBasic.h"

using namespace Symbolics;

namespace Symbolics
{
    namespace Python
    {
        struct _CLess_Object_ : public CBasicObject  
        {
        };
        
        typedef struct _CLess_Object_ CLessObject;

        extern PyTypeObject CLessObjectType;
    };
};

#endif // __CLESS_H_