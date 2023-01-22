#ifndef __CSIGN_H_
#define __CSIGN_H_

#include <Python.h>
#include "Symbolics.h"
#include "CBasic.h"

using namespace Symbolics;

namespace Symbolics
{
    namespace Python
    {
        struct _CSign_Object_ : public CBasicObject  
        {
        };
        
        typedef struct _CSign_Object_ CSignObject;

        extern PyTypeObject CSignObjectType;
    };
};

#endif // __CSIGN_H_