#include <iostream>
#include "test_atan.h"
#include <math.h>

int test_atan( int &argc,  char *argv[])
{
    // Symbol und Konstanten anlegen
    BasicPtr a( new Symbol("a") );
    BasicPtr A( new Symbol("A", Shape(3,2)) );
    BasicPtr two( new Int( 2 ) );
    BasicPtr pointTwo( new Real( 0.2 ) );

    // Na dann mal los (Skalar & Matrx)
    BasicPtr atana( new Atan(a) );
    if (atana->getType() != Type_Atan) return -1;
    if (atana->getShape() != Shape()) return -2;
    BasicPtr atanA( new Atan(A) );
    if (atanA->getType() != Type_Atan) return -3;
    if (atanA->getShape() != Shape(3,2)) return -4;

    // Vereinfachungen mit Konstanten
    // Atan(2) = atan(2)
    BasicPtr AtanTwo( new Atan(two) );
    BasicPtr atanTwo( new Real( atan(2.0) ) );
    BasicPtr t1(AtanTwo->simplify());
    if (t1 != atanTwo) return -10;
    if (Atan::New(two) != atanTwo) return -110;
    // Atan(.2) = atan(.2)
    BasicPtr AtanPointTwo( new Atan(pointTwo) );
    BasicPtr atanPointTwo( new Real( atan(0.2) ) );
    BasicPtr t2(AtanPointTwo->simplify());
    if (t2 != atanPointTwo) return -11; 
    if (Atan::New(pointTwo) != atanPointTwo) return -111;

    // Komplexe Vereinfachungen
    // Atan(Tan(.)) == (.)
    BasicPtr Tana( new Tan(a) );
    BasicPtr AtanTana( new Atan( Tana ) );
    BasicPtr t3(AtanTana->simplify());
    if (t3 != a) return -20;
    if (Atan::New(Tana) != a) return -120;

    // atan(0) = 0
    BasicPtr atanzero(new Atan(Int::getZero()));
    BasicPtr atanzerosim = atanzero->simplify();
    BasicPtr atan0(new Atan(BasicPtr( new Int(0))));
    BasicPtr atan0im = atan0->simplify();
    if (atanzerosim != Real::New( atan(0.0))) return -21;
    if (atanzerosim != atan0im) return -22;

    // atan[[a,a],[a,a]] = [[atan(a),atan(a)],[atan(a),atan(a)]]
    BasicPtrVec matvals;
    matvals.push_back(a);
    matvals.push_back(a);
    matvals.push_back(a);
    matvals.push_back(a);
    BasicPtr mata(new Matrix(matvals,Shape(2,2)));
    BasicPtr atanmat(new Atan(mata));
    BasicPtr atansmatsim = atanmat->simplify();
    BasicPtrVec atanmatvals;
    atanmatvals.push_back(atana);
    atanmatvals.push_back(atana);
    atanmatvals.push_back(atana);
    atanmatvals.push_back(atana);
    BasicPtr matatana(new Matrix(atanmatvals,Shape(2,2)));
    if (atansmatsim != matatana) return -23;
    if (Atan::New(mata) != matatana) return -24;

    // der(atan(a)) = der(a)/(1+a^2)
    BasicPtr deratana = atana->der();
    if (deratana != Util::div(a->der(),1+Util::pow(a,2))) return -30;

    return 0;
}