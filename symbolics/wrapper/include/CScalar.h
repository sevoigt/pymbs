#ifndef __CScalar_H_
#define __CScalar_H_

#include <Python.h>
#include "Symbolics.h"
#include "CBasic.h"

using namespace Symbolics;

namespace Symbolics
{
    namespace Python
    {
        struct _CScalar_Object_ : public CBasicObject  
        {
        };
        
        typedef struct _CScalar_Object_ CScalarObject;

        extern PyTypeObject CScalarObjectType;
    };
};

#endif // __CScalar_H_