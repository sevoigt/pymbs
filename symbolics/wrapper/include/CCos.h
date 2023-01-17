#ifndef __CCos_H_
#define __CCos_H_

#include <Python.h>
#include "Symbolics.h"
#include "CBasic.h"

using namespace Symbolics;

namespace Symbolics
{
    namespace Python
    {
        struct _CCos_Object_ : public CBasicObject  
        {
        };
        
        typedef struct _CCos_Object_ CCosObject;

        extern PyTypeObject CCosObjectType;
    };
};

#endif // __CCos_H_