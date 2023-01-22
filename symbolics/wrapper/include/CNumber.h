#ifndef __CNumber_H_
#define __CNumber_H_

#include <Python.h>
#include "Symbolics.h"
#include "CBasic.h"

using namespace Symbolics;

namespace Symbolics
{
    namespace Python
    {
        struct _CNumber_Object_ : public CBasicObject  
        {
        };
        
        typedef struct _CNumber_Object_ CNumberObject;

        extern PyTypeObject CNumberObjectType;
    };
};

#endif // __CNumber_H_