#include <iostream>
#include "test_equal.h"

int test_jacobian( int &argc,  char *argv[])
{
    // ToDo

    BasicPtr a( new Symbol("a") );
    BasicPtr b( new Symbol("b") );


    // jacobian(a,[a]) = [1]
    BasicPtrVec veca;
    veca.push_back(a);

    BasicPtr symbols1(new Matrix(veca,Shape(1)));
    BasicPtr jaca(new Jacobian(a,symbols1));
    BasicPtr jacas = jaca->simplify();
    BasicPtrVec vecone;
    vecone.push_back(Int::getOne());
    BasicPtr matone(new Matrix(vecone,Shape(1)));
    if (jacas != matone) return -1;

    // jacobian(a,[b]) = [0]
    BasicPtrVec vecb;
    vecb.push_back(b);

    BasicPtr symbols2(new Matrix(vecb,Shape(1)));
    BasicPtr jaca1(new Jacobian(a,symbols2));
    BasicPtr jaca1s = jaca1->simplify();
    BasicPtrVec veczero;
    veczero.push_back(Zero::getZero());
    BasicPtr matzero(new Matrix(veczero,Shape(1)));
    if (jaca1s != matzero) return -2;

    // jacobian([a,b,c],[a,b,c]) = [[1,0,0],[0,1,0],[0,0,1]]
    BasicPtr c( new Symbol("c") );
    BasicPtrVec vecabc;
    vecabc.push_back(a);
    vecabc.push_back(b);
    vecabc.push_back(c);

    BasicPtr symbols3(new Matrix(vecabc,Shape(3)));
    BasicPtr jacabc(new Jacobian(symbols3,symbols3));
    BasicPtr jacabcs = jacabc->simplify();;
    if (jacabcs != Util::eye(Shape(3,3))) return -3;

    // jacobian([a^2,b^2,c^2],[a,b,c]) = [[1,0,0],[0,1,0],[0,0,1]]
    BasicPtrVec vecaabbcc;
    vecaabbcc.push_back(a*a);
    vecaabbcc.push_back(b*b);
    vecaabbcc.push_back(c*c);

    BasicPtr jacaabbcc(new Jacobian(new Matrix(vecaabbcc,Shape(3)),symbols3));
    BasicPtr jacaabbccs = jacaabbcc->simplify();
    BasicPtrVec mat3vec;
    mat3vec.push_back(Int::New(2)*a);
    mat3vec.push_back(Zero::getZero());
    mat3vec.push_back(Zero::getZero());
    mat3vec.push_back(Zero::getZero());
    mat3vec.push_back(Int::New(2)*b);
    mat3vec.push_back(Zero::getZero());
    mat3vec.push_back(Zero::getZero());
    mat3vec.push_back(Zero::getZero());
    mat3vec.push_back(Int::New(2)*c);
    BasicPtr mat(new Matrix(mat3vec,Shape(3,3)));
    if (jacaabbccs != mat) return -4;

    return 0;
}