#include <iostream>
#include "test_pow.h"
#include "math.h"

int test_pow( int &argc,  char *argv[])
{
    // Symbol und Konstanten anlegen
    BasicPtr a( new Symbol("a") );
    BasicPtr A( new Symbol("A", Shape(2,2)) );
    BasicPtr B( new Symbol("B", Shape(3,2)) );
    BasicPtr two( new Int( 2 ) );
    BasicPtr minusTwo( new Int( -2 ) );
    BasicPtr pointTwo( new Real( 0.2 ) );
    BasicPtr minusPointTwo( new Real( -0.2 ) );

    // Na dann mal los (Skalar & Matrx)
    BasicPtr cubea( new Pow(a,3) );
    if (cubea->getType() != Type_Pow) return -1;
    if (cubea->getShape() != Shape()) return -2;
    BasicPtr squareA( new Pow(A,2) );
    if (squareA->getType() != Type_Pow) return -3;
    if (squareA->getShape() != Shape(2,2)) return -4;
    try    {
        BasicPtr powB( new Pow(B,1) );
        return -5;
    } 
    catch (ShapeError) { }
    catch (...) { return -6; };

    // Vereinfachungen testen (Exponent == 1)
    BasicPtr powOnea( new Pow(a,1) );
    BasicPtr t1(powOnea->simplify());
    if (t1 != a) return -10;
    BasicPtr powOneA( new Pow(A,1) );
    BasicPtr t2(powOneA->simplify());
    if (t2 != A) return -11;

    // Vereinfachungen testen (Exponent == 0)
    BasicPtr powZeroa( new Pow(a,0) );
    BasicPtr One( new Int(1) );
    BasicPtr t3(powZeroa->simplify());
    if (t3 != One) return -12;
    BasicPtr powZeroA( new Pow(A,0) );
    BasicPtr t4(powZeroA->simplify());
    if (t4 != One) return -13;

    // Vereinfachungen mit Konstanten
    // Pow(2,3) == 8
    BasicPtr cubeTwo( new Pow(two, 3) );
    BasicPtr Eight( new Int(8) );
    BasicPtr t5(cubeTwo->simplify());
    if (t5 != Eight) return -20;
    // Pow(2,0.5) == sqrt(2)
    BasicPtr sqrtTwo( new Pow(two, 0.5) );
    BasicPtr SqrtTwo( new Real( sqrt(2.0) ) );
    BasicPtr t6(sqrtTwo->simplify());
    if (t6 != SqrtTwo) return -21;

    // Pow(0,1.234) == 0
    BasicPtr Zero( new Int(0) );
    BasicPtr powZero( new Pow(Zero, 1.234) );
    BasicPtr t7(powZero->simplify());
    if (t7 != Zero) return -22;
    // Pow(1,1.234) == 1
    BasicPtr powOne( new Pow(One, 1.234) );
    BasicPtr t8(powOne->simplify());
    if (t8 != One) return -23;

    // Komplexe Vereinfachungen
    // Pow(Pow(a,2),3) = pow(f,6)
    BasicPtr squarea( new Pow(a,2) );
    BasicPtr cubeSquarea( new Pow(squarea,3) );
    BasicPtr powSixa( new Pow(a,6) );
    BasicPtr t9(cubeSquarea->simplify());
    if (t9 != powSixa) return -30;

    // Pow(-a,2) = Pow(a,2)
    BasicPtr nega( new Neg(a) );
    BasicPtr squareNega( new Pow(nega,2) );
    BasicPtr t10(squareNega->simplify());
    if (t10 != squarea) return -31;

    // Pow(-a,3) = -Pow(a,3)
    BasicPtr cubeNega(new Pow(nega,3) );
    BasicPtr negCubea( new Neg(cubea) );
    BasicPtr t11(cubeNega->simplify());
    if (t11 != negCubea) return -32;

    // Pow(-a,4) = Pow(a,4)
    BasicPtr powFourNega(new Pow(nega,4) );
    BasicPtr powFoura( new Pow(a,4) );
    BasicPtr t12(powFourNega->simplify());
    if (t12 != powFoura) return -33;

    // der(a^2) = 2*der(a)*a
    if (squarea->der() != Mul::New(Int::New(2),Mul::New(a->der(),a))) return -34;

    // matrix([0,0,0];[0,0,-0.5];[0,0.5,0])^2 
    Matrix *mat = new Matrix(Shape(3,3));
    mat->set(1,2,Real::New(0.5));
    mat->set(2,1,Real::New(-0.5));
    BasicPtr pmat(mat);
    BasicPtr pow(new Pow(pmat,Int::New(2)));
    BasicPtr pows = pow->simplify();
    Matrix *rmat = new Matrix(Shape(3,3));
    rmat->set(1,1,Real::New(-0.25));
    rmat->set(2,2,Real::New(-0.25));
    BasicPtr prmat(rmat);
    if (pows != prmat) return -35;



    return 0;
}