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
#include "test_acos.h"
#include "math.h"

int test_acos( int &argc,  char *argv[])
{
    // Symbol und Konstanten anlegen
    BasicPtr a( new Symbol("a") );
    BasicPtr A( new Symbol("A", Shape(3,2)) );
    BasicPtr one( new Int( 1 ) );
    BasicPtr pointTwo( new Real( 0.2 ) );

    // Na dann mal los (Skalar & Matrx)
    BasicPtr acosa( new Acos(a) );
    if (acosa->getType() != Type_Acos) return -1;
    if (acosa->getShape() != Shape()) return -2;
    BasicPtr acosA( new Acos(A) );
    if (acosA->getType() != Type_Acos) return -3;
    if (acosA->getShape() != Shape(3,2)) return -4;

    // Vereinfachungen mit Konstanten
    // Acos(1) = acos(1)
    BasicPtr AcosOne( new Acos(one) );
    BasicPtr acosOne( new Real( acos(1.0) ) );
    BasicPtr t1(AcosOne->simplify());
    if (t1 != acosOne) return -10; 
    if (Acos::New(one) != acosOne) return -110; 
    // Acos(.2) = acos(.2)
    BasicPtr AcosPointTwo( new Acos(pointTwo) );
    BasicPtr acosPointTwo( new Real( acos(0.2) ) );
    BasicPtr t2(AcosPointTwo->simplify());
    if (t2 != acosPointTwo) return -11; 
    if (Acos::New(pointTwo) != acosPointTwo) return -111; 

    // Komplexe Vereinfachungen
    // Acos(Cos(.)) == (.)
    BasicPtr Cosa( new Cos(a) );
    BasicPtr AcosCosa( new Acos( Cosa ) );
    BasicPtr t3(AcosCosa->simplify());
    if (t3 != a) return -20;
    if (Acos::New(Cosa) != a) return -120;

    // acos(0) = 1.570796327
    BasicPtr acoszero(new Acos(Int::getZero()));
    BasicPtr acoszerosim = acoszero->simplify();
    BasicPtr acos0(new Acos(BasicPtr( new Int(0))));
    BasicPtr acos0im = acos0->simplify();
    if (acoszerosim != Real::New( acos(0.0))) return -21;
    if (acoszerosim != acos0im) return -22;

    // acos[[a,a],[a,a]] = [[acos(a),acos(a)],[acos(a),acos(a)]]
    BasicPtrVec matvals;
    matvals.push_back(a);
    matvals.push_back(a);
    matvals.push_back(a);
    matvals.push_back(a);
    BasicPtr mata(new Matrix(matvals,Shape(2,2)));
    BasicPtr acosmat(new Acos(mata));
    BasicPtr acossmatsim = acosmat->simplify();
    BasicPtrVec acosmatvals;
    acosmatvals.push_back(BasicPtr(new Acos(a)));
    acosmatvals.push_back(BasicPtr(new Acos(a)));
    acosmatvals.push_back(BasicPtr(new Acos(a)));
    acosmatvals.push_back(BasicPtr(new Acos(a)));
    BasicPtr matacosa(new Matrix(acosmatvals,Shape(2,2)));
    if (acossmatsim != matacosa) return -23;
    if (Acos::New(mata) != matacosa) return -24;


    // der(acos(a)) = -der(a)/sqrt(1-a^2)
    BasicPtr deracosa = acosa->der();
    if (deracosa != Util::div(-a->der(),Util::sqrt(1-Util::pow(a,2)))) return -30;

    return 0;
}