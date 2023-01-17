#ifndef __CZEROS_H_
#define __CZEROS_H_

#include <Python.h>
#include "Symbolics.h"
#include "CBasic.h"

using namespace Symbolics;

namespace Symbolics
{
    namespace Python
    {
        struct _CZeros_Object_ : public CBasicObject  
        {
        };
        
        typedef struct _CZeros_Object_ CZerosObject;

        extern PyTypeObject CZerosObjectType;
    };
};

#endif // __CZEROS_H_