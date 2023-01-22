#ifndef __CTan_H_
#define __CTan_H_

#include <Python.h>
#include "Symbolics.h"
#include "CBasic.h"

using namespace Symbolics;

namespace Symbolics
{
    namespace Python
    {
        struct _CTan_Object_ : public CBasicObject  
        {
        };
        
        typedef struct _CTan_Object_ CTanObject;

        extern PyTypeObject CTanObjectType;
    };
};

#endif // __CTan_H_