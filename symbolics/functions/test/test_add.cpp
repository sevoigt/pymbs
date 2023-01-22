#include <iostream>
#include "test_add.h"

int test_add( int &argc,  char *argv[])
{
    // TODO
#pragma region add
    // add(1.0,1.0) = 2.0
    BasicPtr cdbl(new Real(1.0));
    BasicPtr exp = Add::New(cdbl,cdbl);
    BasicPtr cdbl_two(new Real(2.0));
    if (exp != cdbl_two) return -1;

    // add(1,1) = 2
    BasicPtr cint(new Int(1));
    BasicPtr exp1 = Add::New(cint,cint);
    BasicPtr ci(new Int(2));
    if (exp1 != ci) return -2;

    // add(1.0,1) = 2.0
    BasicPtr exp2 = Add::New(cdbl,cint);
    if (exp2 != cdbl_two) return -3;

    // add(1,1.0) = 2.0
    BasicPtr exp3 = Add::New(cint,cdbl);
    if (exp3 != cdbl_two) return -4;

    // add(c+d) = c+d 
    BasicPtr c(new Symbol("c"));
    BasicPtr d(new Symbol("d"));
    BasicPtr exp4 = Add::New(c,d);
    BasicPtr exp5(new Add(c,d));
    if (exp4 != exp5) return -5;

    // add([1,2,3],[1,2,3]) = [2,4,6]
    Matrix *cvint = new Matrix( Shape(3) );
    *cvint = 1,2,3;
    BasicPtr exp6 = Add::New(BasicPtr(cvint),BasicPtr(cvint));
    Matrix cvmint( Shape(3) );
    cvmint = 2,4,6;
    if (exp6 != cvmint) return -6;
#pragma endregion

    // der(a+b) = der(a)+der(b)
    BasicPtr a( new Symbol("a") );
    BasicPtr b( new Symbol("b") );
    if (Add::New(a,b)->der() != Add::New(a->der(),b->der())) return -10;

    // der(a+b,a) = 1
	BasicPtr abdera = Add::New(a,b)->der(a);
	abdera = abdera->simplify();
    if (abdera != Int::getOne()) return -11;

	// eye(3)-eye(3) == 0
	Matrix *eye = new Matrix( Shape(3,3) );
	eye->set(0,0, Int::getOne()); 	
	eye->set(1,1, Int::getOne());
	eye->set(2,2, Int::getOne());

	Matrix *eye2 = new Matrix( Shape(3,3) );
	eye2->set(0,0, Int::getOne()); 	
	eye2->set(1,1, Int::getOne());
	eye2->set(2,2, Int::getOne());

	BasicPtr left(eye);
	BasicPtr right(eye2);

	BasicPtr diff = left - right;
	std::cout << diff->toString() << std::endl;

	if (diff != Zero::getZero( Shape(3,3) )) return -20;


    return 0;
}