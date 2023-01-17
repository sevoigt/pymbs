#ifndef __CDer_H_
#define __CDer_H_

#include <Python.h>
#include "Symbolics.h"
#include "CBasic.h"

using namespace Symbolics;

namespace Symbolics
{
    namespace Python
    {
        struct _CDer_Object_ : public CBasicObject  
        {
        };
        
        typedef struct _CDer_Object_ CDerObject;

        extern PyTypeObject CDerObjectType;
    };
};

#endif // __CDer_H_