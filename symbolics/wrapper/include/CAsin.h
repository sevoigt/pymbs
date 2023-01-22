#ifndef __CAsin_H_
#define __CAsin_H_

#include <Python.h>
#include "Symbolics.h"
#include "CBasic.h"

using namespace Symbolics;

namespace Symbolics
{
    namespace Python
    {
        struct _CAsin_Object_ : public CBasicObject  
        {
        };
        
        typedef struct _CAsin_Object_ CAsinObject;

        extern PyTypeObject CAsinObjectType;
    };
};

#endif // __CAsin_H_