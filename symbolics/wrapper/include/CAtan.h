#ifndef __CAtan_H_
#define __CAtan_H_

#include <Python.h>
#include "Symbolics.h"
#include "CBasic.h"

using namespace Symbolics;

namespace Symbolics
{
    namespace Python
    {
        struct _CAtan_Object_ : public CBasicObject  
        {
        };
        
        typedef struct _CAtan_Object_ CAtanObject;

        extern PyTypeObject CAtanObjectType;
    };
};

#endif // __CAtan_H_