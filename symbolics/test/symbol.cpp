/*
This file is part of PyMbs.

PyMbs is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as 
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

PyMbs is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public 
License along with PyMbs.
If not, see <http://www.gnu.org/licenses/>.

Copyright 2011, 2012 Carsten Knoll, Christian Schubert, 
                     Jens Frenkel, Sebastian Voigt
*/

#include <iostream>
#include "Symbolics.h"

using namespace Symbolics;

int main( int argc,  char *argv[])
{
    // Beispiel aufbauen
    BasicPtr a(new Symbol("a"));
#ifdef _DEBUG
    if (a->toString() != "Symbol(a)") return -1;
#else
    if (a->toString() != "a") return -1;
#endif
    if (a->getType() != Type_Symbol) return -2;
    if (!a->is_Scalar()) return -3; 
    try
    {
        if (a->getShape().getDimension(1) != 1) return -4; 
    }
    catch(ShapeError)
    {
    }
    catch(...)
    {
        return -5;
    }
    try
    {
      if (a->getShape().getDimension(2) != 1) return -6; 
    }
    catch(ShapeError)
    {
    }
    catch(...)
    {
        return -7;
    }


    BasicPtr b(new Symbol("b",Shape(1,1)));
#ifdef _DEBUG
    if (b->toString() != "Symbol(b)") return -8;
#else
    if (b->toString() != "b") return -8;
#endif
    if (b->getType() != Type_Symbol) return -9;
    if (!b->is_Matrix()) return -10; 
    if (b->getShape().getDimension(1) != 1) return -11; 
    if (b->getShape().getDimension(2) != 1) return -12; 


    BasicPtr c(new Symbol("c",Shape(1)));
#ifdef _DEBUG
    if (c->toString() != "Symbol(c)") return -13;
#else
    if (c->toString() != "c") return -13;
#endif
    if (c->getType() != Type_Symbol) return -14;
    if (!c->is_Vector()) return -15; 
    if (c->getShape().getDimension(1) != 1) return -16; 
    try
    {
      if (c->getShape().getDimension(2) != 1) return -17; 
    }
    catch(ShapeError)
    {
    }
    catch(...)
    {
        return -18;
    }

    BasicPtr d(new Symbol("d",Shape(1,1)));
    if (d->simplify() != d) return -19;

    Symbol *sym = Util::getAsPtr<Symbol>(d);
    if (sym->getKind() != VARIABLE) return -20;

#ifdef _DEBUG
    try
    {
        BasicPtr e(new Symbol("d"));
    }
    catch(InternalError)
    {
    }
    catch( ... )
    {
        return -21;
    }
#endif

    // vergleich
#ifndef _DEBUG
    BasicPtr a1(new Symbol("a"));
#endif
    BasicPtr e(new Symbol("e"));
#ifndef _DEBUG
    if (a1 != a) return -50;
    if (a1 < a) return -51;
    if (a < a1) return -52;
#endif
    if (b < a) return -53;
    if (d < e) return -54;

#pragma region Derivative

    BasicPtr dera = a->der();
    if (dera != Der::New(a)) return -100;

    BasicPtr deraa  =a->der(a);
    if (deraa != Int::getOne() ) return -101;

#ifndef _DEBUG
    BasicPtr derab  =a1->der(b);
    if (derab != Zero::getZero() ) return -102;
#endif

#pragma endregion

    return 0;
}