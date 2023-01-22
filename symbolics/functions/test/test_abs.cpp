#include <iostream>
#include "test_abs.h"

int test_abs( int &argc,  char *argv[])
{
    // Symbol und Konstanten anlegen
    BasicPtr a( new Symbol("a") );
    BasicPtr A( new Symbol("A", Shape(3,2)) );
    BasicPtr two( new Int( 2 ) );
    BasicPtr minusTwo( new Int( -2 ) );
    BasicPtr pointTwo( new Real( 0.2 ) );
    BasicPtr minusPointTwo( new Real( -0.2 ) );

    // Na dann mal los (Skalar & Matrx)
    BasicPtr absa( new Abs(a) );
    if (absa->getType() != Type_Abs) return -1;
    if (absa->getShape() != Shape()) return -2;
    BasicPtr absA( new Abs(A) );
    if (absA->getType() != Type_Abs) return -3;
    if (absA->getShape() != Shape(3,2)) return -4;

    // Vereinfachungen mit Konstanten
    // |2| = 2
    BasicPtr abstwo( new Abs(two) );
    BasicPtr t1(abstwo->simplify());
    if (t1 != two) return -10; 
    if (Abs::New(two) != two) return -110;
    // |-2| = 2
    BasicPtr absMinusTwo( new Abs(minusTwo) );
    BasicPtr t2(absMinusTwo->simplify());
    if (t2 != two) return -11; 
    if (Abs::New(minusTwo) != two) return -111;
    // |0.2| = 0.2
    BasicPtr absPointTwo( new Abs(pointTwo) );
    BasicPtr t3(absPointTwo->simplify());
    if (t3 != pointTwo) return -12; 
    if (Abs::New(pointTwo) != pointTwo) return -112;
    // |-0.2| = 0.2
    BasicPtr absMinusPointTwo( new Abs(minusPointTwo) );
    BasicPtr t4(absMinusPointTwo->simplify());
    if (t4 != pointTwo) return -13; 
    if (Abs::New(minusPointTwo) != pointTwo) return -113;
    // |0| = 0
    BasicPtr absZero( new Abs(Int::getZero()) );
    BasicPtr t41(absZero->simplify());
    if (t41 != Int::getZero()) return -14; 
    if (Abs::New(Int::getZero()) != Int::getZero()) return -114;
    

    // Komplexe Vereinfachungen
    // Neg rausschmeissen
    BasicPtr nega( new Neg(a) );
    BasicPtr absNega( new Abs(nega) );
    BasicPtr t5(absNega->simplify());
    if (t5 != absa) return -20;
    if (Abs::New(nega) != absa) return -120;
    // Abs(Pow(.,2*N)) = Pow(.,2*N)
    // Abs(Pow(.,2)) == Pow(.,2)
    BasicPtr squarea( new Pow(a,2) );
    BasicPtr absSquarea( new Abs( squarea ) );
    BasicPtr t6(absSquarea->simplify());
    if (t6 != squarea) return -21;
    if (Abs::New(squarea) != squarea) return -121;
    // Abs(Pow(.,3)) == Abs(Pow(.,3))
    BasicPtr cubea( new Pow(a,3) );
    BasicPtr absCubea( new Abs( cubea ) );
    BasicPtr t7(absCubea->simplify());
    if (t7 != absCubea) return -22;
    if (Abs::New(cubea) != t7) return -122;
    // Abs(Pow(.,8)) == Pow(.,8)
    BasicPtr powa( new Pow(a,8) );
    BasicPtr absPowa( new Abs( powa ) );
    BasicPtr t8(absPowa->simplify());
    if (t8 != powa) return -23;
    if (Abs::New(powa) != powa) return -122;

    // abs[[a,a],[a,a]] = [[abs(a),abs(a)],[abs(a),abs(a)]]
    BasicPtrVec matvals;
    matvals.push_back(a);
    matvals.push_back(a);
    matvals.push_back(a);
    matvals.push_back(a);
    BasicPtr mata(new Matrix(matvals,Shape(2,2)));
    BasicPtr absmat(new Abs(mata));
    BasicPtr abssmatsim = absmat->simplify();
    BasicPtrVec absmatvals;
    absmatvals.push_back(BasicPtr(new Abs(a)));
    absmatvals.push_back(BasicPtr(new Abs(a)));
    absmatvals.push_back(BasicPtr(new Abs(a)));
    absmatvals.push_back(BasicPtr(new Abs(a)));
    BasicPtr matabsa(new Matrix(absmatvals,Shape(2,2)));
    if (abssmatsim != matabsa) return -24;
    if (Abs::New(mata) != matabsa) return -124;

    // der(abs(a)) = sign(a)*der(a)
    BasicPtr derabsa = absa->der();
    if (derabsa != Mul::New(Sign::New(a),Der::New(a))) return -30;

    return 0;
}