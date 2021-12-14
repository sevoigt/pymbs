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
#include "test_transpose.h"

int test_transpose( int &argc,  char *argv[])
{
    // Symbole und Konstanten anlegen
    BasicPtr a( new Symbol("a") );
    BasicPtr b( new Symbol("b", Shape(3)) );
    BasicPtr A( new Symbol("A", Shape(3,2)) );

    // Symbole fuer 3x1 Vektor
    BasicPtr v0( new Symbol("v0") );
    BasicPtr v1( new Symbol("v1") );
    BasicPtr v2( new Symbol("v2") );
    // Zugehoeriger Vektor
    Matrix *vec = new Matrix(Shape(3));
    *vec = v0, v1, v2;
    BasicPtr v(vec);
    // Zuegehoerige Matrix
    Matrix *vecT = new Matrix(Shape(1,1,3));
    *vecT = v0, v1, v2;
    BasicPtr vT(vecT);

    // Na dann mal los
    BasicPtr transa( new Transpose(a) );
    if (transa->getType() != Type_Transpose) return -1;
    if (transa->getShape() != Shape()) return -2;

    BasicPtr transb( new Transpose(b) );
    if (transb->getType() != Type_Transpose) return -2;
    if (transb->getShape() != Shape(1,1,3)) return -3;

    BasicPtr transA( new Transpose(A) );
    if (transA->getType() != Type_Transpose) return -4;
    if (transA->getShape() != Shape(2,3)) return -5;

    // Klappt es auch?
    BasicPtr t1(transa->simplify());
    if (t1 != a) return -10;
    BasicPtr transv( new Transpose(v) );
    BasicPtr t2(transv->simplify());
    if (t2 != vT) return -11;

    // Neg nach vorn ziehen
    BasicPtr negA( new Neg(A) );
    BasicPtr transNegA( new Transpose(negA) );
    BasicPtr negTransA( new Neg(transA) );
    BasicPtr t3(transNegA->simplify());
    if (t3 != negTransA) return -20;
    // Transpose(Transpose(.)) == (.)
    BasicPtr transTransA(new Transpose(transA) );
    BasicPtr t4(transTransA->simplify());
    if (t4 != A) return -21;


    // der(transpose(A)) = transpose(der(A))
    BasicPtr derA = transA->der();
    if (derA != Transpose::New(Der::New(A))) return -30;
    return 0;
}