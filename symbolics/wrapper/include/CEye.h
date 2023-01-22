#ifndef __CEYE_H_
#define __CEYE_H_

#include <Python.h>
#include "Symbolics.h"
#include "CBasic.h"

using namespace Symbolics;

namespace Symbolics
{
    namespace Python
    {
        struct _CEye_Object_ : public CBasicObject  
        {
        };
        
        typedef struct _CEye_Object_ CEyeObject;

        extern PyTypeObject CEyeObjectType;
    };
};

#endif // __CEYE_H_