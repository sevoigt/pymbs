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
#include "test_skew.h"

int test_skew( int &argc,  char *argv[])
{
    // Symbole und Konstanten anlegen
    BasicPtr a( new Symbol("a", Shape(3)) );
    BasicPtr b( new Symbol("b") );
    BasicPtr c( new Symbol("c", Shape(4)) );
    BasicPtr A( new Symbol("A", Shape(3,2)) );

    // Symbole fuer 3x1 Vektor
    BasicPtr v0( new Symbol("v0") );
    BasicPtr v1( new Symbol("v1") );
    BasicPtr v2( new Symbol("v2") );
    // Zugehoeriger Vektor
    Matrix *vec = new Matrix(Shape(3));
    *vec = v0, v1, v2;
    BasicPtr v(vec);
    // Zueghoerige Matrix
    Matrix *m = new Matrix(Shape(3,3));
    BasicPtr Zero( new Int(0) );
    *m = Zero, -v2, v1,
         v2, Zero, -v0,
         -v1, v0, Zero;
    BasicPtr V(m);

    // Na dann mal los (Vektor)
    BasicPtr skewa( new Skew(a) );
    if (skewa->getType() != Type_Skew) return -1;
    if (skewa->getShape() != Shape(3,3)) return -2;

    // ShapeErrors
    try {
        BasicPtr skewb( new Skew(b) );
        return -3;
    }
    catch (ShapeError) { }
    catch (...) { return -4; };
    try {
        BasicPtr skewc( new Skew(c) );
        return -5;
    } 
    catch (ShapeError) { }
    catch (...) { return -6; };
    try {
        BasicPtr skewA( new Skew(A) );
        return -7;
    }
    catch (ShapeError) { }
    catch (...) { return -8; };

    // Klappt es auch?
    BasicPtr skewv( new Skew(v) );
    BasicPtr t1(skewv->simplify());
    if (t1 != V) return -10;

    // Neg nach vorn ziehen
    BasicPtr nega( new Neg(a) );
    BasicPtr skewNega( new Skew(nega) );
    BasicPtr negSkewa( new Neg(skewa) );
    BasicPtr t2(skewNega->simplify());
    if (t2 != negSkewa) return -20;

    // skew(Zeros(3)) = Zeros(3,3);
    BasicPtr skewzero3(new Skew(Zero::getZero(Shape(3))));
    BasicPtr skewzero3s = skewzero3->simplify();
    if (skewzero3s != Zero::getZero(Shape(3,3))) return -21;
    if (Skew::New(Zero::getZero(Shape(3))) != Zero::getZero(Shape(3,3))) return -21;


    // der(skew(a)) = skew(der(a))
    if (skewa->der() != Skew::New(a->der())) return -30;

    return 0;
}