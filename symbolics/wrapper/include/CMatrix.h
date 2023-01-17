#ifndef __CMATRIX_H_
#define __CMATRIX_H_

#include <Python.h>
#include "Symbolics.h"
#include "CBasic.h"

using namespace Symbolics;

namespace Symbolics
{
    namespace Python
    {
        struct _CMatrix_Object_ : public CBasicObject  
        {
        };
        
        typedef struct _CMatrix_Object_ CMatrixObject;

        extern PyTypeObject CMatrixObjectType;
    };
};

#endif // __CMATRIX_H_