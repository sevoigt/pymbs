#ifndef __CSkew_H_
#define __CSkew_H_

#include <Python.h>
#include "Symbolics.h"
#include "CBasic.h"

using namespace Symbolics;

namespace Symbolics
{
    namespace Python
    {
        struct _CSkew_Object_ : public CBasicObject  
        {
        };
        
        typedef struct _CSkew_Object_ CSkewObject;

        extern PyTypeObject CSkewObjectType;
    };
};

#endif // __CSkew_H_