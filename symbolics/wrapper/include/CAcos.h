#ifndef __CACOS_H_
#define __CACOS_H_

#include <Python.h>
#include "Symbolics.h"
#include "CBasic.h"

using namespace Symbolics;

namespace Symbolics
{
    namespace Python
    {
        struct _CAcos_Object_ : public CBasicObject  
        {
        };
        
        typedef struct _CAcos_Object_ CAcosObject;

        extern PyTypeObject CAcosObjectType;
    };
};

#endif // __CACOS_H_