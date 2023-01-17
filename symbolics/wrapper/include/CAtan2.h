#ifndef __CAtan2_H_
#define __CAtan2_H_

#include <Python.h>
#include "Symbolics.h"
#include "CBasic.h"

using namespace Symbolics;

namespace Symbolics
{
    namespace Python
    {
        struct _CAtan2_Object_ : public CBasicObject  
        {
        };
        
        typedef struct _CAtan2_Object_ CAtan2Object;

        extern PyTypeObject CAtan2ObjectType;
    };
};

#endif // __CAtan2_H_