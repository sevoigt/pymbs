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
#include "test_solve.h"

int test_solve( int &argc,  char *argv[])
{
    // solve(1.0,1.0) = 1.0
    BasicPtr cdbl(new Real(1.0));
    BasicPtr exp1(new Solve(cdbl,cdbl));
    BasicPtr exp2 = exp1->simplify();
    if (exp2 != cdbl) return -1;

    // solve(1,1) = 1
    BasicPtr cint(new Int(1));
    BasicPtr exp3(new Solve(cint,cint));
    BasicPtr exp3a = exp3->simplify();
    if (exp3a != cint) return -2;

    // solve(1.0,1) = 1.0
    BasicPtr exp4(new Solve(cdbl,cint));
    BasicPtr exp5 = exp4->simplify();
    if (exp5 != cdbl) return -4;

    // solve(1,1.0) = 1.0
    BasicPtr exp6(new Solve(cint,cdbl));
    BasicPtr exp7 = exp6->simplify();
    if (exp7 != cdbl) return -5;

    // solve(A(3,2),1.0) = InternalError
    BasicPtr A( new Symbol("A", Shape(3,2) ) );
    try
    {
        BasicPtr exp8(new Solve(A,cdbl));
        return -6;
    }
    catch( ShapeError ) {}
    catch ( ... ) { return -7; };

    // solve(A(3,1),1.0) = InternalError
    Matrix *matB = new Matrix( Shape(3,2) );
    BasicPtr B( matB );
    try
    {
        BasicPtr exp9(new Solve(B,cdbl));
        return -8;
    }
    catch( ShapeError ) {}
    catch ( ... ) { return -9; };

    // solve(A(3,2),B(3) = InternalError
    Matrix *vecB = new Matrix( Shape(3) );
    BasicPtr vB( vecB );
    try
    {
        BasicPtr exp9(new Solve(B,vB));
        return -10;
    }
    catch( ShapeError ) {}
    catch ( ... ) { return -11; };

    // solve(I(3,3),B(3)) = B(3)
    *vecB = 1.0,1.0,1.0;
    Matrix *matI = new Matrix( Shape(3,3) );
    *matI = 1.0,0.0,0.0,
            0.0,1.0,0.0,
            0.0,0.0,1.0;
    BasicPtr I( matI );
    BasicPtr exp10(new Solve(matI,vB));
    BasicPtr exp11 = exp10->simplify();
    if (exp11 != vB) return -12;

    // der(solve(A,b)) = solve(A,der(b)-der(A)*solve(A,b))
    BasicPtr D( new Symbol("D", Shape(3,3) ) );
    BasicPtr b( new Symbol("b", Shape(3) ) );
    BasicPtr solveAb = Solve::New(D,b);
    BasicPtr dersolveAb = solveAb->der();
    if (dersolveAb != Solve::New(D,b->der()-D->der()*solveAb)) return -30;

    return 0;
}