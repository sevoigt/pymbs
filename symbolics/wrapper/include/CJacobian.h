#ifndef __CJACOBIAN_H_
#define __CJACOBIAN_H_

#include <Python.h>
#include "Symbolics.h"
#include "CBasic.h"

using namespace Symbolics;

namespace Symbolics
{
    namespace Python
    {
        struct _CJacobian_Object_ : public CBasicObject  
        {
        };
        
        typedef struct _CJacobian_Object_ CJacobianObject;

        extern PyTypeObject CJacobianObjectType;
    };
};

#endif // __CJACOBIAN_H_