#ifndef __CELEMENT_H_
#define __CELEMENT_H_

#include <Python.h>
#include "Symbolics.h"
#include "CBasic.h"

using namespace Symbolics;

namespace Symbolics
{
    namespace Python
    {
        struct _CElement_Object_ : public CBasicObject  
        {
        };
        
        typedef struct _CElement_Object_ CElementObject;

        extern PyTypeObject CElementObjectType;
    };
};

#endif // __CELEMENT_H_