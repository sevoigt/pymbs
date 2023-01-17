#include <iostream>
#include "test_mul.h"

int test_mul( int &argc,  char *argv[])
{
    // TODO
#pragma region mul
    // mul(1.0,1.0) = 1.0
    BasicPtr cdbl(new Real(1.0));
    BasicPtr exp = Mul::New(cdbl,cdbl);
    if (exp != cdbl) return -1;

    // mul(1,1) = 1
    BasicPtr cint(new Int(1));
    BasicPtr exp1 = Mul::New(cint,cint);
    if (exp1 != cint) return -2;

    // mul(1.0,1) = 1.0
    BasicPtr exp2 = Mul::New(cdbl,cint);
    if (exp2 != cdbl) return -3;

    // mul(1,1.0) = 1.0
    BasicPtr exp3 = Mul::New(cint,cdbl);
    if (exp3 != cdbl) return -4;

    // mul(f+g) = f+g 
    BasicPtr f(new Symbol("f"));
    BasicPtr g(new Symbol("g"));
    BasicPtr exp4 = Mul::New(f,g);
    BasicPtr exp5(new Mul(f,g));
    if (exp4 != exp5) return -5;

    // add([1,2,3],[1,2,3]) = [2,4,6]
    Matrix *cvint = new Matrix( Shape(3) );
    *cvint = 1,2,3;
    BasicPtr cvintp(cvint);
    BasicPtr exp6 = Mul::New(BasicPtr(new Matrix(cvint->transpose())),cvintp);
    Matrix cvmint( cvint->transpose()*(*cvint));
    if (exp6 != cvmint) return -6;


    // der(a*b) = der(a)*b+a*der(b)
    BasicPtr a( new Symbol("a") );
    BasicPtr b( new Symbol("b") );
    if (Mul::New(a,b)->der() != Add::New(Mul::New(a->der(),b),Mul::New(a,b->der()))) return -10;

    // der(a*b,a) = b
    if (Mul::New(a,b)->der(a)->simplify() != b) return -11;

    // transpose(vector[a,b,c])*vector[a,b,c] = a*a+b*b+c*c
    BasicPtr c( new Symbol("c") );
    Matrix *abc = new Matrix( Shape(3));
    *abc = a,b,c;
    BasicPtr abcp(abc);
    BasicPtr tabc = Transpose::New(BasicPtr(abc));
    BasicPtr tabcabcp(new Mul(tabc,abcp));
    BasicPtr tabcabcps = tabcabcp->simplify();
    BasicPtr tabcabcp1 =  Mul::New(tabc,abcp);
    if (!tabcabcps->is_Scalar()) return -15;
    if (!tabcabcp1->is_Scalar()) return -16;
    if (tabcabcps->getType() == Type_Matrix) return -17;
    if (tabcabcp1->getType() == Type_Matrix) return -18;

    return 0;
}