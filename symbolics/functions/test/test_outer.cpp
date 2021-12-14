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
#include "test_equal.h"

int test_outer( int &argc,  char *argv[])
{

    BasicPtr a( new Symbol("a") );
    BasicPtr b( new Symbol("b") );
    BasicPtr c( new Symbol("c") );
    BasicPtr d( new Symbol("d") );
    BasicPtr e( new Symbol("e") );
    BasicPtr f( new Symbol("f") );

    // outer ([a,b,c],[d,e,f]) = [[a*d,a*e,a*f],[b*d,b*e,b*f],[c*d,c*e,c*f]]
    BasicPtrVec mat1vec;
    mat1vec.push_back(a);
    mat1vec.push_back(b);
    mat1vec.push_back(c);
    BasicPtr vec1(new Matrix(mat1vec,Shape(3)));

    BasicPtrVec mat2vec;
    mat2vec.push_back(d);
    mat2vec.push_back(e);
    mat2vec.push_back(f);
    BasicPtr vec2(new Matrix(mat2vec,Shape(3)));

    BasicPtr o1(new Outer(vec1,vec2));
    BasicPtr o1s = o1->simplify();

    BasicPtrVec mat3vec;
    mat3vec.push_back(a*d);
    mat3vec.push_back(a*e);
    mat3vec.push_back(a*f);
    mat3vec.push_back(b*d);
    mat3vec.push_back(b*e);
    mat3vec.push_back(b*f);
    mat3vec.push_back(c*d);
    mat3vec.push_back(c*e);
    mat3vec.push_back(c*f);
    BasicPtr mat(new Matrix(mat3vec,Shape(3,3)));
    if (o1s != mat) return -1;
    // ToDo


    return 0;
}