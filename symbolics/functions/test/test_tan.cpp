#include <iostream>
#include "test_tan.h"
#include "math.h"

int test_tan( int &argc,  char *argv[])
{
    // Symbol und Konstanten anlegen
    BasicPtr a( new Symbol("a") );
    BasicPtr A( new Symbol("A", Shape(3,2)) );
    BasicPtr two( new Int( 2 ) );
    BasicPtr pointTwo( new Real( 0.2 ) );

    // Na dann mal los (Skalar & Matrx)
    BasicPtr tana( new Tan(a) );
    if (tana->getType() != Type_Tan) return -1;
    if (tana->getShape() != Shape()) return -2;
    BasicPtr tanA( new Tan(A) );
    if (tanA->getType() != Type_Tan) return -3;
    if (tanA->getShape() != Shape(3,2)) return -4;

    // Vereinfachungen mit Konstanten
    // Tan(2) = tan(2)
    BasicPtr TanTwo( new Tan(two) );
    BasicPtr tanTwo( new Real( tan(2.0) ) );
    BasicPtr t1(TanTwo->simplify());
    if (t1 != tanTwo) return -10; 
    if (Tan::New(two) != tanTwo) return -110;
    // Tan(.2) = tan(.2)
    BasicPtr TanPointTwo( new Tan(pointTwo) );
    BasicPtr tanPointTwo( new Real( tan(0.2) ) );
    BasicPtr t2(TanPointTwo->simplify());
    if (t2 != tanPointTwo) return -11; 
    if (Tan::New(pointTwo) != tanPointTwo) return -111; 

    // Komplexe Vereinfachungen
    // Tan(Atan(.)) == (.)
    BasicPtr Atana( new Atan(a) );
    BasicPtr TanAtana( new Tan( Atana ) );
    BasicPtr t3(TanAtana->simplify());
    if (t3 != a) return -20;
    if (Tan::New(Atana) != a) return -120;
    // Tan(Neg(.)) == Neg(Tan(.))
    BasicPtr nega( new Neg(a) );
    BasicPtr tanNega( new Tan(nega) );
    BasicPtr t4(tanNega->simplify());
    if (t4 != Neg::New(tana)) return -21;
    if (Tan::New(nega) != Neg::New(tana)) return -121;

    // tan(0) = 0
    BasicPtr tanzero(new Tan(Int::getZero()));
    BasicPtr tanzerosim = tanzero->simplify();
    BasicPtr tan0(new Tan(BasicPtr( new Int(0))));
    BasicPtr tan0im = tan0->simplify();
    if (tanzerosim != Real::New( tan(0.0))) return -22;
    if (tanzerosim != tan0im) return -23;

    // tan[[a,a],[a,a]] = [[tan(a),tan(a)],[tan(a),tan(a)]]
    BasicPtrVec matvals;
    matvals.push_back(a);
    matvals.push_back(a);
    matvals.push_back(a);
    matvals.push_back(a);
    BasicPtr mata(new Matrix(matvals,Shape(2,2)));
    BasicPtr tanmat(new Tan(mata));
    BasicPtr tansmatsim = tanmat->simplify();
    BasicPtrVec tanmatvals;
    tanmatvals.push_back(tana);
    tanmatvals.push_back(tana);
    tanmatvals.push_back(tana);
    tanmatvals.push_back(tana);
    BasicPtr mattana(new Matrix(tanmatvals,Shape(2,2)));
    if (tansmatsim != mattana) return -24;
    if (Tan::New(mata) != mattana) return -25;

    // der(tan(a)) = der(a)/cos(a)^2
    BasicPtr dertana = tana->der();
    if (dertana != Util::div(a->der(),Util::pow(Cos::New(a),Int::New(2)))) return -30;

    return 0;
}