#ifndef __CINVERSE_H_
#define __CINVERSE_H_

#include <Python.h>
#include "Symbolics.h"
#include "CBasic.h"

using namespace Symbolics;

namespace Symbolics
{
    namespace Python
    {
        struct _CInverse_Object_ : public CBasicObject  
        {
        };
        
        typedef struct _CInverse_Object_ CInverseObject;

        extern PyTypeObject CInverseObjectType;
    };
};

#endif // __CINVERSE_H_