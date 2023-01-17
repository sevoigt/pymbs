#ifndef __CTranspose_H_
#define __CTranspose_H_

#include <Python.h>
#include "Symbolics.h"
#include "CBasic.h"

using namespace Symbolics;

namespace Symbolics
{
    namespace Python
    {
        struct _CTranspose_Object_ : public CBasicObject  
        {
        };
        
        typedef struct _CTranspose_Object_ CTransposeObject;

        extern PyTypeObject CTransposeObjectType;
    };
};

#endif // __CTranspose_H_