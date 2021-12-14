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
#include "test_scalar.h"

int test_scalar( int &argc,  char *argv[])
{
    // Symbol und Konstanten anlegen
    BasicPtr a( new Symbol("a") );
    BasicPtr v( new Symbol("v", Shape(1)) );
    BasicPtr A( new Symbol("A", Shape(1,1)) );
    BasicPtr B( new Symbol("B", Shape(2,1)) );

    // Na dann mal los (Skalar & Matrx)
    BasicPtr s_a( new Scalar(a) );
    if (s_a->getType() != Type_Scalar) return -1;
    if (s_a->getShape() != Shape()) return -2;
    BasicPtr s_v( new Scalar(v) );
    if (s_v->getType() != Type_Scalar) return -3;
    if (s_v->getShape() != Shape()) return -4;
    BasicPtr s_A( new Scalar(A) );
    if (s_A->getType() != Type_Scalar) return -5;
    if (s_A->getShape() != Shape()) return -6;
    try {
        BasicPtr s_B( new Scalar(B) );
        return -7;
    }
    catch (ShapeError) {}
    catch (...) { return -8; };

    // Wenn es schon ein Skalar ist, dann kann das Scalar auch weg
    BasicPtr t1(s_a->simplify());
    if (t1 != a) return -10;
    // Bei allen anderen darf das nicht passieren
    BasicPtr t2(s_v->simplify());
    if (t2 == v) return -11;
    BasicPtr t3(s_A->simplify());
    if (t3 == A) return -12;

    // Negs sollten vorgeholt werden, damit sie sich ggf. in einem Add darueber aufheben koennen
    BasicPtr negA( new Neg(A) );
    BasicPtr scalarNegA( new Scalar(negA) );
    BasicPtr negScalarA( new Neg(s_A) );
    BasicPtr t4(scalarNegA->simplify());
    if (t4 != negScalarA) return -20;

    // der(scalar(a)) = scalar(der(a))
    if (s_a->der() != Scalar::New(a->der())) return -30;

    return 0;
}