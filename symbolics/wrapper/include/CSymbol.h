#ifndef __CSYMBOL_H_
#define __CSYMBOL_H_

#include <Python.h>
#include "Symbolics.h"
#include "CBasic.h"

using namespace Symbolics;

namespace Symbolics
{
    namespace Python
    {
        struct _CSymbol_Object_ : public CBasicObject  
        {
        };
        
        typedef struct _CSymbol_Object_ CSymbolObject;

        extern PyTypeObject CSymbolObjectType;
    };
};

#endif // __CSYMBOL_H_