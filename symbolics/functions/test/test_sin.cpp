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
#include "test_sin.h"
#include "math.h"

int test_sin( int &argc,  char *argv[])
{
    // Symbol und Konstanten anlegen
    BasicPtr a( new Symbol("a") );
    BasicPtr A( new Symbol("A", Shape(3,2)) );
    BasicPtr two( new Int( 2 ) );
    BasicPtr pointTwo( new Real( 0.2 ) );

    // Na dann mal los (Skalar & Matrx)
    BasicPtr sina( new Sin(a) );
    if (sina->getType() != Type_Sin) return -1;
    if (sina->getShape() != Shape()) return -2;
    BasicPtr sinA( new Sin(A) );
    if (sinA->getType() != Type_Sin) return -3;
    if (sinA->getShape() != Shape(3,2)) return -4;

    // Vereinfachungen mit Konstanten
    // Sin(2) = sin(2)
    BasicPtr SinTwo( new Sin(two) );
    BasicPtr sinTwo( new Real( sin(2.0) ) );
    BasicPtr t1(SinTwo->simplify());
    if (t1 != sinTwo) return -10; 
    if (Sin::New(two) != sinTwo) return -110; 
    // Sin(.2) = sin(.2)
    BasicPtr SinPointTwo( new Sin(pointTwo) );
    BasicPtr sinPointTwo( new Real( sin(0.2) ) );
    BasicPtr t2(SinPointTwo->simplify());
    if (t2 != sinPointTwo) return -11; 
    if (Sin::New(pointTwo) != sinPointTwo) return -111; 

    // Komplexe Vereinfachungen
    // Sin(Asin(.)) == (.)
    BasicPtr Asina( new Asin(a) );
    BasicPtr SinAsina( new Sin( Asina ) );
    BasicPtr t3(SinAsina->simplify());
    if (t3 != a) return -20;
    if (Sin::New(Asina) != a) return -120;
    // Sin(Neg(.)) == Neg(Sin(.))
    BasicPtr nega( new Neg(a) );
    BasicPtr sinNega( new Sin(nega) );
    BasicPtr negSina( new Neg(sina) );
    BasicPtr t4(sinNega->simplify());
    if (t4 != negSina) return -21;
    if (Sin::New(nega) != negSina) return -121;

    // sin(1.0) = 0.84147098480789650
    BasicPtr cdbl(new Real(1.0));
    BasicPtr cdbl_sin(new Real(0.84147098480789650));
    BasicPtr exp = Sin::New(cdbl);
    if (exp != cdbl_sin) return -123;

    // sin(1) = 0.84147098480789650
    BasicPtr cint(new Int(1));
    BasicPtr exp1 = Sin::New(cint);
    if (exp1 != cdbl_sin) return -124;
    if (Sin::New(cint) != cdbl_sin) return -224;

    // sin(f) = sin(f) 
    BasicPtr f(new Symbol("f"));
    BasicPtr exp2 = Sin::New(f);
    BasicPtr exp3(new Sin(f));
    if (exp2 != exp3) return -125;

    // sin(-f) = -sin(f) 
    BasicPtr exp4 = Neg::New(f);
    BasicPtr exp5 = Sin::New(exp4);
    BasicPtr exp6 = Neg::New(exp2);
    if (exp5 != exp6) return -126;

    // sin([1,1,1]) = [0.84147098480789650,0.84147098480789650,0.84147098480789650]
    Matrix cvint( Shape(3) );
    cvint = 1,1,1;
    BasicPtr cvintp(new Matrix(cvint));
    BasicPtr exp7 = Sin::New(cvintp);
    Matrix cvdbl_sin( Shape(3) );
    cvdbl_sin = 0.8414709848078965040,0.8414709848078965040,0.8414709848078965040;
    BasicPtr cvdbl_sinp(new Matrix(cvdbl_sin));
    if (exp7 != cvdbl_sinp) return -127;

    // sin([1.0,1.0,1.0]) = [0.84147098480789650,0.84147098480789650,0.84147098480789650]
    Matrix cvdbl( Shape(3) );
    cvdbl = 1.0,1.0,1.0;
    BasicPtr cvdblp(new Matrix(cvdbl));
    BasicPtr exp8 = Sin::New(cvdblp);
    if (exp8 != cvdbl_sinp) return -128;

    // sin([f,f,f]) = [sin(f),sin(f),sin(f)]
    Matrix mat( Shape(3) );
    mat = f,f,f;
    BasicPtr matp(new Matrix(mat));
    BasicPtr exp9 = Sin::New(matp);
    Matrix mat_sin( Shape(3) );
    mat_sin = exp2,exp2,exp2;
    BasicPtr mat_sinp(new Matrix(mat_sin));
    if (exp9 != mat_sinp) return -129;

    // sin([1,1;1,1]) = [0.84147098480789650,0.84147098480789650;0.84147098480789650,0.84147098480789650]
    Matrix cmint( Shape(2,2) );
    cmint = 1,1,
            1,1;
    BasicPtr cmintp(new Matrix(cmint));
    BasicPtr exp10 = Sin::New(cmintp);
    Matrix cmdbl_sin( Shape(2,2) );
    cmdbl_sin = 0.8414709848078965040,0.8414709848078965040,0.8414709848078965040,0.8414709848078965040;
    BasicPtr cmdbl_sinp(new Matrix(cmdbl_sin));
    if (exp10 != cmdbl_sinp) return -130;

    // sin([1.0,1.0;1.0,1.0]) = [0.84147098480789650,0.84147098480789650;0.84147098480789650,0.84147098480789650]
    Matrix cmdbl( Shape(2,2) );
    cmdbl = 1.0,1.0,1.0,1.0;
    BasicPtr cmdblp(new Matrix(cmdbl));
    BasicPtr exp11 = Sin::New(cmdblp);
    if (exp11 != cmdbl_sinp) return -131;

    // sin([f,f;f,f]) = [sin(f),sin(f);sin(f),sin(f)]
    Matrix mat1( Shape(2,2) );
    mat1 = f,f,f,f;
    BasicPtr mat1p(new Matrix(mat1));
    BasicPtr exp12 = Sin::New(mat1p);
    Matrix mat1_sin( Shape(2,2) );
    mat1_sin = exp2,exp2,exp2,exp2;
    BasicPtr mat1_sinp(new Matrix(mat1_sin));
    if (exp12 != mat1_sinp) return -132;


    // Scope
    const Sin* sin = Util::getAsConstPtr<Sin>(sina);
    ScopePtr  scope = sin->getScope();
    List *list = scope->first;
    while (list != NULL)
    {
        Sin *listsin = List::getAsPtr<Sin>(list);
        list = list->getRigth();
    }


    // der(sin(a)) = der(a)*cos(a)
    BasicPtr dersina = sina->der();
    if (dersina != a->der()*Cos::New(a)) return -30;

    return 0;
}