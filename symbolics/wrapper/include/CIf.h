
#ifndef __CIF_H_
#define __CIF_H_

#include <Python.h>
#include "Symbolics.h"
#include "CBasic.h"

using namespace Symbolics;

namespace Symbolics
{
    namespace Python
    {
        struct _CIf_Object_ : public CBasicObject  
        {
        };
        
        typedef struct _CIf_Object_ CIfObject;

        extern PyTypeObject CIfObjectType;
    };
};

#endif // __CIF_H_