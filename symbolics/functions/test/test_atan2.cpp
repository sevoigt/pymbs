#include <iostream>
#include "test_atan2.h"
#include "math.h"

int test_atan2( int &argc,  char *argv[])
{
    // Symbol und Konstanten anlegen
    BasicPtr a( new Symbol("a") );
    BasicPtr b( new Symbol("b") );
    BasicPtr A( new Symbol("A", Shape(3,2)) );
    BasicPtr two( new Int( 2 ) );
    BasicPtr pointTwo( new Real( 0.2 ) );

    // Na dann mal los (Skalar & Matrx)
    BasicPtr atan2a( new Atan2(a,b) );
    if (atan2a->getType() != Type_Atan2) return -1;
    if (atan2a->getShape() != Shape()) return -2;
    try
    {
        BasicPtr atan2A( new Atan2(A,b) );
        return -3;
    }
    catch (ShapeError) { }
    catch (...) { return -4; };
    try
    {
        BasicPtr atan2A( new Atan2(b,A) );
        return -5;
    }
    catch (ShapeError) { }
    catch (...) { return -6; };
    try
    {
        BasicPtr atan2A( new Atan2(A,A) );
        return -7;
    }
    catch (ShapeError) { }
    catch (...) { return -8; };

    // Vereinfachungen mit Konstanten
    // Atan2(2,0.2) = atan2(2,0.2)
    BasicPtr Atan2TwoPointTwo( new Atan2(two, pointTwo) );
    BasicPtr atan2TwoPointTwo( new Real( atan2(2.0,0.2) ) );
    BasicPtr t1(Atan2TwoPointTwo->simplify());
    if (t1 != atan2TwoPointTwo) return -10; 
    if (Atan2::New(two,pointTwo) != atan2TwoPointTwo) return -110;

    // der(atan2(a,b)) = der(b/a) / (1+(b/a)^2 )
    BasicPtr deratan2a = atan2a->der();
    //if (deratan2a != Util::div(Util::div(b,a)->der(),1+Util::pow(Util::div(b,a),2))) return -30;

    return 0;
}