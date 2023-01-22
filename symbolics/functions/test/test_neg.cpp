#include <iostream>
#include "test_neg.h"

int test_neg( int &argc,  char *argv[])
{

#pragma region neg
    // neg(1.0) = -1.0
    BasicPtr cdbl(new Real(1.0));
    BasicPtr exp = Neg::New(cdbl);
    BasicPtr scdbl(new Real(-1.0));
    if (exp != scdbl) return -1;

    // neg(1) = -1
    BasicPtr cint(new Int(1));
    BasicPtr iexp = Neg::New(cint);
    BasicPtr scint(new Int(-1));
    if (iexp != scint) return -2;

    // neg(neg(1.0)) = 1.0
    BasicPtr exp1 = Neg::New(exp);
    if (exp1 != cdbl) return -3;

    // neg(a) = -a
    BasicPtr a(new Symbol("a"));
    BasicPtr exp2 = Neg::New(a);
    BasicPtr exp3(new Neg(a));
    if (exp2 != exp3) return -4;

    // neg([1,2,3]) = [-1,-2,-3]
    Matrix *cvint = new Matrix( Shape(3) );
    *cvint = 1,2,3;
    BasicPtr exp4 = Neg::New(BasicPtr(cvint));
    Matrix cvmint( Shape(3) );
    cvmint = -1,-2,-3;
    if (exp4 != cvmint) return -5;

#pragma endregion

    // der(-a) = -der(a)
    if (Neg::New(a)->der() != Neg::New(a->der())) return -10;

    // TODO
    return 0;
}