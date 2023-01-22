#include <iostream>
#include "test_asin.h"
#include "math.h"

int test_asin( int &argc,  char *argv[])
{
    // Symbol und Konstanten anlegen
    BasicPtr a( new Symbol("a") );
    BasicPtr A( new Symbol("A", Shape(3,2)) );
    BasicPtr one( new Int( 1 ) );
    BasicPtr pointTwo( new Real( 0.2 ) );

    // Na dann mal los (Skalar & Matrx)
    BasicPtr asina( new Asin(a) );
    if (asina->getType() != Type_Asin) return -1;
    if (asina->getShape() != Shape()) return -2;
    BasicPtr asinA( new Asin(A) );
    if (asinA->getType() != Type_Asin) return -3;
    if (asinA->getShape() != Shape(3,2)) return -4;

    // Vereinfachungen mit Konstanten
    // Asin(1) = asin(1)
    BasicPtr AsinOne( new Asin(one) );
    BasicPtr asinOne( new Real( asin(1.0) ) );
    BasicPtr t1(AsinOne->simplify());
    if (t1 != asinOne) return -10; 
    if (Asin::New(one) != asinOne) return -110;
    // Asin(.2) = asin(.2)
    BasicPtr AsinPointTwo( new Asin(pointTwo) );
    BasicPtr asinPointTwo( new Real( asin(0.2) ) );
    BasicPtr t2(AsinPointTwo->simplify());
    if (t2 != asinPointTwo) return -11; 
    if (Asin::New(pointTwo) != asinPointTwo) return -111;

    // Komplexe Vereinfachungen
    // Asin(Sin(.)) == (.)
    BasicPtr Sina( new Sin(a) );
    BasicPtr AsinSina( new Asin( Sina ) );
    BasicPtr t3(AsinSina->simplify());
    if (t3 != a) return -20;
    if (Asin::New(Sina) != a) return -120;

    // asin(0) = 0
    BasicPtr asinzero(new Asin(Int::getZero()));
    BasicPtr asinzerosim = asinzero->simplify();
    BasicPtr asin0(new Asin(BasicPtr( new Int(0))));
    BasicPtr asin0im = asin0->simplify();
    if (asinzerosim != Real::New( asin(0.0))) return -21;
    if (asinzerosim != asin0im) return -22;

    // asin[[a,a],[a,a]] = [[asin(a),asin(a)],[asin(a),asin(a)]]
    BasicPtrVec matvals;
    matvals.push_back(a);
    matvals.push_back(a);
    matvals.push_back(a);
    matvals.push_back(a);
    BasicPtr mata(new Matrix(matvals,Shape(2,2)));
    BasicPtr asinmat(new Asin(mata));
    BasicPtr asinsmatsim = asinmat->simplify();
    BasicPtrVec asinmatvals;
    asinmatvals.push_back(asina);
    asinmatvals.push_back(asina);
    asinmatvals.push_back(asina);
    asinmatvals.push_back(asina);
    BasicPtr matasina(new Matrix(asinmatvals,Shape(2,2)));
    if (asinsmatsim != matasina) return -23;
    if (Asin::New(mata) != matasina) return -24;

    // der(asin(a)) = der(a)/sqrt(1-a^2)
    BasicPtr derasina = asina->der();
    if (derasina != Util::div(a->der(),Util::sqrt(1-Util::pow(a,2)))) return -30;

    return 0;
}