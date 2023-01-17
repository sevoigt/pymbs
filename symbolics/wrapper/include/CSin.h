#ifndef __CSIN_H_
#define __CSIN_H_

#include <Python.h>
#include "Symbolics.h"
#include "CBasic.h"

using namespace Symbolics;

namespace Symbolics
{
    namespace Python
    {
        struct _CSin_Object_ : public CBasicObject  
        {
        };
        
        typedef struct _CSin_Object_ CSinObject;

        extern PyTypeObject CSinObjectType;
    };
};

#endif // __CSIN_H_