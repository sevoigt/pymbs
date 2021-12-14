/*
This file is part of PyMbs.

PyMbs is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as 
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

PyMbs is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public 
License along with PyMbs.
If not, see <http://www.gnu.org/licenses/>.

Copyright 2011, 2012 Carsten Knoll, Christian Schubert, 
                     Jens Frenkel, Sebastian Voigt
*/

#include <iostream>
#include <math.h>
#include "Symbolics.h"

using namespace Symbolics;

int neg( int &argc,  char *argv[])
{
    // -(1.0) == -1.0
    BasicPtr cdbl(new Real(1.0));
    BasicPtr exp(new Neg(cdbl));
    BasicPtr simple_exp = exp->simplify();
    BasicPtr scdbl(new Real(-1.0));
    if (simple_exp != scdbl) return -1;

    // (-1) == -1
    BasicPtr cint(new Int(1));
    BasicPtr iexp(new Neg(cint));
    BasicPtr simple_iexp = iexp->simplify();
    BasicPtr scint(new Int(-1));
    if (simple_iexp != scint) return -2;

    // -(-(1.0)) = 1.0
    BasicPtr exp1(new Neg(exp));
    BasicPtr simple_exp1 = exp1->simplify();
    if (simple_exp1 != cdbl) return -3;

    return 0;
}

int add( int &argc,  char *argv[])
{
    // Add(1.0) == 1.0
    BasicPtr cdbl(new Real(1.0));
	BasicPtrVec arg; arg.push_back(cdbl);
    BasicPtr exp(new Add(arg));
    BasicPtr simple_exp = exp->simplify();
    if (simple_exp != cdbl) return -4;

    // Add(Add(1.0)) == 1.0
	BasicPtrVec arg1; arg1.push_back(exp);
    BasicPtr exp1(new Add(arg1));
    BasicPtr simple_exp1 = exp1->simplify();
    std::cout << exp1->toString() << std::endl;
    std::cout << simple_exp1->toString() << std::endl;
    if (simple_exp1 != cdbl) return -5;

    // Add{Add[Add(1.0,1.0),Add(1.0+1.0)], Add[Add(1.0+1.0),Add(1.0+1.0)]} == 8.0
    // {[(1.0+1.0)+(1.0+1.0)]+[(1.0+1.0)+(1.0+1.0)]} == 8.0
    BasicPtr exp2(new Add(cdbl,cdbl));
    BasicPtr exp3(new Add(cdbl,cdbl));
    BasicPtr exp4(new Add(exp2,exp3));
    BasicPtr exp5(new Add(exp2,exp3));
    BasicPtr exp6(new Add(exp4,exp5));
    BasicPtr dbl(new Real(8.0));
    BasicPtr simple_exp2 = exp6->simplify();
    if (simple_exp2 != dbl) return -6;


    // Add{Add[Add(1,1),Add(1+1)], Add[Add(1+1),Add(1+1)]} == 8
    // {[(1+1)+(1+1)]+[(1+1)+(1+1)]} == 8
    BasicPtr cint(new Int(1));
    BasicPtr exp7(new Add(cint,cint));
    BasicPtr exp8(new Add(cint,cint));
    BasicPtr exp9(new Add(exp7,exp8));
    BasicPtr exp10(new Add(exp7,exp8));
    BasicPtr exp11(new Add(exp9,exp10));
    BasicPtr ci(new Int(8));
    BasicPtr simple_exp3 = exp11->simplify();
    if (simple_exp3 != ci) return -7;


    // Add:
    //   Add{Add[Add(1.0,1.0),Add(1.0+1.0)], Add[Add(1.0+1.0),Add(1.0+1.0)]}
    //   Add{Add[Add(1,1),Add(1+1)], Add[Add(1+1),Add(1+1)]}
    // == 16.0
    // vereinfacht: 8.0 + 8 == 16.0
    BasicPtr exp12(new Add(exp6,exp11));
    BasicPtr simple_exp4 = exp12->simplify();
    // Wäre das nicht sinnvoller? Int + Dbl = Dbl
    BasicPtr exp13(new Real(16.0));
    if (simple_exp4 != exp13) return -8;

    // (-(1.0+1.0))+(1.0+1.0) == 0.0
    BasicPtr exp14(new Neg(exp2));
    BasicPtr exp15(new Add(exp14,exp2));
    BasicPtr simple_exp5 = exp15->simplify();
    BasicPtr ndbl(new Real(0.0));
    if (simple_exp5 != ndbl) return -9;

    // (a+b)+(a+b)-> 2*a+2*b 
    BasicPtr a(new Symbol("a"));
    BasicPtr b(new Symbol("b"));
    BasicPtr exp16(new Add(a,b));
    BasicPtr exp17(new Add(a,b));
    BasicPtr exp18(new Add(exp16,exp17));
    BasicPtr simple_exp6 = exp18->simplify();
    BasicPtr int_two(new Int(2));
    BasicPtr exp19(new Mul(int_two,a));
    BasicPtr exp20(new Mul(int_two,b));
    BasicPtr exp21(new Add(exp19,exp20));
    if (simple_exp6 != exp21) return -10;

    // a+(-a) = 0
    BasicPtr exp22(new Add(a,BasicPtr(new Neg(a))));
    BasicPtr simple_exp22 = exp22->simplify();
    if (simple_exp22 != Zero::getZero()) return -19;

    // (0.5*a)-(0.5*a) = 0
    BasicPtr exp23 = Mul::New(Real::New(0.5),a);
    BasicPtr exp24 = Mul::New(Real::New(0.5),a);
    BasicPtr exp25(new Add(exp23,Neg::New(exp24)));
    std::cout << exp25->toString() << std::endl;
    BasicPtr simple_exp25 = exp25->simplify();
    std::cout << simple_exp25->toString() << std::endl;
    if (simple_exp25 != Zero::getZero()) return -20;


    // a+[[0,0,0],[0,0,0],[0,0,0]] = [[a,a,a],[a,a,a],[a,a,a]]
    BasicPtr exp31(new Add(Zero::getZero(Shape(3,3)),a));
    BasicPtr simple_exp31 = exp31->simplify();
    std::cout << simple_exp31->toString() << std::endl;
    const Matrix *simple_exp31mat = Util::getAsConstPtr<Matrix>(simple_exp31);
    for (size_t i=0;i<3;i++)
      for (size_t j=0;j<3;j++)
          if (simple_exp31mat->get(i,j) != a) return -25;

    // -sin(a)*cos(b) + cos(a)*-sin(b)
    BasicPtr cosa( new Cos(a) );
    BasicPtr sina( new Sin(a) );
    BasicPtr nsina( new Neg(sina) );
    BasicPtr cosb( new Cos(b) );
    BasicPtr sinb( new Sin(b) );
    BasicPtr nsinb( new Neg(sinb) );
    BasicPtr cosansinb( new Mul(cosa,nsinb) );
    BasicPtr nsinacosb( new Mul(nsina,cosb ) );
    BasicPtr tmp( new Add(cosansinb,nsinacosb) );
    BasicPtr cosasinb( new Mul(cosa,sinb) );
    BasicPtr sinacosb( new Mul(sina,cosb ) );
    BasicPtr ref( new Add(BasicPtr(new Neg(cosasinb)),BasicPtr(new Neg(sinacosb))) );
    BasicPtr tmps = tmp->simplify();
    std::cout << tmps->toString() << std::endl;
    std::cout << ref->toString() << std::endl;
    if (tmps != ref) return -30;

    return 0;
}

int mul( int &argc,  char *argv[])
{
    // Mul(1.0) == 1.0
    BasicPtr cdbl(new Real(1.0));
	  BasicPtrVec arg; arg.push_back(cdbl);
    BasicPtr exp(new Mul(arg));
    BasicPtr simple_exp = exp->simplify();
    if (simple_exp != cdbl) return -11;

    // {[(1.0*1.0)*(1.0*1.0)]*[(1.0*1.0)*(1.0*1.0)]} == 1.0
    BasicPtr exp2(new Mul(cdbl,cdbl));
    BasicPtr exp3(new Mul(cdbl,cdbl));
    BasicPtr exp4(new Mul(exp2,exp3));
    BasicPtr exp5(new Mul(exp2,exp3));
    BasicPtr exp6(new Mul(exp4,exp5));
    BasicPtr simple_exp2 = exp6->simplify();
    if (simple_exp2 != cdbl) return -12;

    // {[(1*1)*(1*1)]*[(1*1)*(1*1)]} == 1
    BasicPtr cint(new Int(1));
    BasicPtr exp7(new Mul(cint,cint));
    BasicPtr exp8(new Mul(cint,cint));
    BasicPtr exp9(new Mul(exp7,exp8));
    BasicPtr exp10(new Mul(exp7,exp8));
    BasicPtr exp11(new Mul(exp9,exp10));
    BasicPtr simple_exp3 = exp11->simplify();
    if (simple_exp3 != cint) return -13;

    // Mul:
    //   {[(1.0*1.0)*(1.0*1.0)]*[(1.0*1.0)*(1.0*1.0)]}
    //   {[(1*1)*(1*1)]*[(1*1)*(1*1)]}
    // == 1.0
    BasicPtr exp12(new Mul(exp6,exp11));
    BasicPtr simple_exp4 = exp12->simplify();
    if (simple_exp4 != cdbl) return -14;

    // (-(1.0*1.0))*(1.0*1.0) == -1.0
    BasicPtr exp14(new Neg(exp2));
    BasicPtr exp15(new Mul(exp14,exp2));
    BasicPtr simple_exp5 = exp15->simplify();
    BasicPtr nint(new Int(-1));
    if (simple_exp5 != nint) return -15; 

    // (c*d)*(-(c*d)) == -(c*d*c*d)
    BasicPtr c(new Symbol("c"));
    BasicPtr d(new Symbol("d"));
    BasicPtr exp16(new Mul(c,d));
    BasicPtr exp17(new Neg(exp16));
    BasicPtr exp18(new Mul(exp16,exp17));
    BasicPtr simple_exp6 = exp18->simplify();
  	BasicPtrVec args19;
  	args19.push_back(Pow::New(c,Int::New(2))); args19.push_back(Pow::New(d,Int::New(2)));
    BasicPtr exp19( new Mul(args19) );
    BasicPtr exp20( new Neg( exp19 ));
    if (simple_exp6 != exp20) return -16;

    // a*{-[(-a)*(-b)*c]} == -(a*a*b*c)
    BasicPtr e(new Symbol("e"));
    BasicPtr nc(new Neg(c));
    BasicPtr nd(new Neg(d));
	BasicPtrVec args21;
	args21.push_back(nc); args21.push_back(nd); args21.push_back(e);
	BasicPtr exp21( new Mul(args21) );
    BasicPtr exp22a( new Neg(exp21) );
    BasicPtr exp22( new Mul(c, exp22a) );
    BasicPtr simple_exp22 = exp22->simplify();
	BasicPtrVec args23;
	args23.push_back(Pow::New(c,Int::New(2))); args23.push_back(d); args23.push_back(e);
    BasicPtr exp23( new Mul(args23) );
    BasicPtr exp24( new Neg( exp23 ) );
    if (simple_exp22 != exp24) return -17;

    // (-a)*{-[(-a)*(-b)*c]}
    BasicPtr exp25( new Mul(nc, exp22a ) );
    BasicPtr simple_exp25 = exp25->simplify();
	BasicPtrVec args26;
	args26.push_back(Pow::New(c,Int::New(2))); args26.push_back(d); args26.push_back(e);
    BasicPtr exp26( new Mul(args26) );
    if (simple_exp25 != exp26) return -18;


    BasicPtr null(new Real(0.0));
    Matrix *mat = new Matrix(Shape(3));
    *mat = null,null,e;
    BasicPtr exp27(mat);
    Matrix *dblmat = new Matrix(Shape(3,3));
    *dblmat = 1.0,0.0,0.0,
              0.0,1.0,0.0,
              0.0,0.0,2.0;
    BasicPtr exp28(dblmat);
    BasicPtr exp29 = exp28*exp27;
    BasicPtr simple_exp29 = exp29->simplify();
    Matrix *resmat = new Matrix(Shape(3));
    *resmat = null,null,2.0*e;
    BasicPtr exp30(resmat);
    if (simple_exp29 != exp30) return -19;


    BasicPtr a(new Symbol("a"));
    BasicPtr exp31(new Mul(Util::eye(Shape(3,3)),a));
    BasicPtr simple_exp31 = exp31->simplify();
    std::cout << simple_exp31->toString() << std::endl;
    const Matrix *simple_exp31mat = Util::getAsConstPtr<Matrix>(simple_exp31);
    for (size_t i=0;i<3;i++)
      for (size_t j=0;j<3;j++)
        if (i!= j)
        {
          if (simple_exp31mat->get(i,j) != Int::getZero()) return -20;
        }
        else
          if (simple_exp31mat->get(i,i) != a) return -21;

    // a*a*a*b*b*c = (a^3)*(b^2)*c
    BasicPtr b(new Symbol("b"));
 	  BasicPtrVec args32;
	  args32.push_back(a); args32.push_back(b); args32.push_back(c); args32.push_back(b); args32.push_back(a); args32.push_back(a);
    BasicPtr exp32( new Mul(args32) );
    BasicPtr exp32s = exp32->simplify();
 	  BasicPtrVec args33;
	  args33.push_back(Pow::New(a,Int::New(3))); args33.push_back(Pow::New(b,Int::New(2))); args33.push_back(c);
    BasicPtr exp33( new Mul(args33) );
    if (exp32s != exp33) return -22;

    return 0;
}

int pow( int &argc,  char *argv[])
{
/*
    Real cdbl(1.0);
    BasicPtr cdplp(new Real(cdbl));
    Real cdbl_two(2.0);
    BasicPtr cdbl_twop(new Real(cdbl_two));
    Real cdbl_four(4.0);
    BasicPtr cdbl_fourp(new Real(cdbl_four));
    Int cint(1);
    BasicPtr cintp(new Int(cint));
    Int cint_two(2);
    BasicPtr cint_twop(new Int(cint_two));
    Int cint_four(4);
    BasicPtr cint_fourp(new Int(cint_four));

    // pow(f,1) = f 
    BasicPtr f(new Symbol("f"));
    BasicPtr exp1(new Pow(f,cint));
    BasicPtr exp2 = exp1->simplify();
    if (exp2 != f) return -105;

    // pow(f,1.0) = f 
    BasicPtr exp3(new Pow(f,cdbl));
    BasicPtr exp4 = exp3->simplify();
    if (exp4 != f) return -106;

    // pow(pow(f,2),2) = pow(f,4)
    BasicPtr exp5(new Pow(f,cint_two));
    BasicPtr exp6(new Pow(exp5,cint_two));
    BasicPtr exp7 = exp6->simplify();
    BasicPtr exp8 = Util::pow(f,cint_fourp);
    if (exp7 != exp8) return -107;

    // pow(pow(f,2.0),2.0) = pow(f,4.0)
    BasicPtr exp9(new Pow(f,cdbl_two));
    BasicPtr exp10(new Pow(exp9,cdbl_two));
    BasicPtr exp11 = exp10->simplify();
    BasicPtr exp12 = Util::pow(f,cdbl_fourp);
    if (exp11 != exp12) return -108;

    // pow(pow(f,2.0),2) = pow(f,4)
    BasicPtr exp13(new Pow(exp9,cint_two));
    BasicPtr exp14 = exp13->simplify();
    if (exp14 != exp12) return -109;

    // pow(pow(f,2),2.0) = pow(f,4)
    BasicPtr exp15(new Pow(exp5,cdbl_two));
    BasicPtr exp16 = exp15->simplify();
    if (exp16 != exp12) return -110;

    // pow(-f,2) = pow(f,2)
    BasicPtr exp17(new Neg(f));
    BasicPtr exp18(new Pow(exp17,cint_two));
    BasicPtr exp19 = exp18->simplify();
    if (exp19 != exp5) return -111;

    // pow(-f,3) = -pow(f,3)
    BasicPtr exp20(new Pow(exp17,Int(3)));
    BasicPtr exp21 = exp20->simplify();
    BasicPtr exp22 = Neg::New(new Pow(f,Int(3)));
    if (exp21 != exp22) return -112;

    // pow(-f,0) = 1
    BasicPtr exp23(new Pow(exp17,Int(0)));
    BasicPtr exp24 = exp23->simplify();
    BasicPtr exp25(new Int(1));
    if (exp24 != exp25) return -113;
*/
    return 0;
}

int sin( int &argc,  char *argv[])
{

    // sin(1.0) = 0.84147098480789650
    BasicPtr cdbl(new Real(1.0));
    BasicPtr cdbl_sin(new Real(0.84147098480789650));
    BasicPtr exp(new Sin(cdbl));
    BasicPtr exp1 = exp->simplify();
    if (exp1 != cdbl_sin) return -114;

    // sin(1) = 0.84147098480789650
    BasicPtr cint(new Int(1));
    BasicPtr exp2(new Sin(cint));
    BasicPtr exp3 = exp2->simplify();
    if (exp3 != cdbl_sin) return -115;

    // sin(-f) = -sin(f)
    BasicPtr f(new Symbol("f"));
    BasicPtr exp4 = Neg::New(f);
    BasicPtr exp5(new Sin(exp4));
    BasicPtr exp6 = exp5->simplify();
    BasicPtr exp6a = Neg::New(Sin::New(f));
    if (exp6 != exp6a) return -116;

    // sin([1,1,1]) = [0.84147098480789650,0.84147098480789650,0.84147098480789650]
    Matrix cvint( Shape(3) );
    cvint = 1,1,1;
    BasicPtr cvintp(new Matrix(cvint));
    BasicPtr exp7(new Sin(cvintp));
    Matrix cvdbl_sin( Shape(3) );
    cvdbl_sin = 0.8414709848078965040,0.8414709848078965040,0.8414709848078965040;
    BasicPtr cvdbl_sinp(new Matrix(cvdbl_sin));
    BasicPtr exp7a= exp7->simplify();
    if (exp7a != cvdbl_sinp) return -117;

    // sin([1.0,1.0,1.0]) = [0.84147098480789650,0.84147098480789650,0.84147098480789650]
    Matrix cvdbl( Shape(3) );
    cvdbl = 1.0,1.0,1.0;
    BasicPtr cvdblp(new Matrix(cvdbl));
    BasicPtr exp8(new Sin(cvdblp));
    BasicPtr exp8a= exp8->simplify();
    if (exp8a != cvdbl_sinp) return -118;

    // sin([1,1;1,1]) = [0.84147098480789650,0.84147098480789650;0.84147098480789650,0.84147098480789650]
    Matrix cmint( Shape(2,2) );
    cmint = 1,1,
            1,1;
    BasicPtr cmintp(new Matrix(cmint));
    BasicPtr exp10(new Sin(cmintp));
    Matrix cmdbl_sin( Shape(2,2) );
    cmdbl_sin = 0.8414709848078965040,0.8414709848078965040,0.8414709848078965040,0.8414709848078965040;
    BasicPtr cmdbl_sinp(new Matrix(cmdbl_sin));
    BasicPtr exp10a= exp10->simplify();
    if (exp10a != cmdbl_sinp) return -120;

    // sin([1.0,1.0;1.0,1.0]) = [0.84147098480789650,0.84147098480789650;0.84147098480789650,0.84147098480789650]
    Matrix cmdbl( Shape(2,2) );
    cmdbl = 1.0,1.0,1.0,1.0;
    BasicPtr cmdblp(new Matrix(cmdbl));
    BasicPtr exp11(new Sin(cmdblp));
    BasicPtr exp11a= exp11->simplify();
    if (exp11a != cmdbl_sinp) return -121;


    return 0;
}

int cos( int &argc,  char *argv[])
{

    // cos(1.0) = 0.54030230586813977
    BasicPtr cdbl(new Real(1.0));
    BasicPtr cdbl_cos(new Real(0.54030230586813977));
    BasicPtr exp(new Cos(cdbl));
    BasicPtr exp1 = exp->simplify();
    if (exp1 != cdbl_cos) return -124;

    // cos(1) = 0.54030230586813977
    BasicPtr cint(new Int(1));
    BasicPtr exp2(new Cos(cint));
    BasicPtr exp3 = exp2->simplify();
    if (exp3 != cdbl_cos) return -125;

    // cos(-f) = -cos(f)
    BasicPtr f(new Symbol("f"));
    BasicPtr exp4 = Neg::New(f);
    BasicPtr exp5(new Cos(exp4));
    BasicPtr exp6 = exp5->simplify();
    BasicPtr exp6a = Cos::New(f);
    if (exp6 != exp6a) return -126;

    // cos([1,1,1]) = [0.54030230586813977,0.54030230586813977,0.54030230586813977]
    Matrix cvint( Shape(3) );
    cvint = 1,1,1;
    BasicPtr cvintp(new Matrix(cvint));
    BasicPtr exp7(new Cos(cvintp));
    Matrix cvdbl_cos( Shape(3) );
    cvdbl_cos = 0.5403023058681397740,0.5403023058681397740,0.5403023058681397740;
    BasicPtr cvdbl_cosp(new Matrix(cvdbl_cos));
    BasicPtr exp7a= exp7->simplify();
    if (exp7a != cvdbl_cosp) return -127;

    // cos([1.0,1.0,1.0]) = [0.54030230586813977,0.54030230586813977,0.54030230586813977]
    Matrix cvdbl( Shape(3) );
    cvdbl = 1.0,1.0,1.0;
    BasicPtr cvdblp(new Matrix(cvdbl));
    BasicPtr exp8(new Cos(cvdblp));
    BasicPtr exp8a= exp8->simplify();
    if (exp8a != cvdbl_cosp) return -128;

    // cos([1,1;1,1]) = [0.54030230586813977,0.54030230586813977;0.54030230586813977,0.54030230586813977]
    Matrix cmint( Shape(2,2) );
    cmint = 1,1,
            1,1;
    BasicPtr cmintp(new Matrix(cmint));
    BasicPtr exp10(new Cos(cmintp));
    Matrix cmdbl_cos( Shape(2,2) );
    cmdbl_cos = 0.5403023058681397740,0.5403023058681397740,0.5403023058681397740,0.5403023058681397740;
    BasicPtr cmdbl_cosp(new Matrix(cmdbl_cos));
    BasicPtr exp10a= exp10->simplify();
    if (exp10a != cmdbl_cosp) return -130;

    // cos([1.0,1.0;1.0,1.0]) = [0.54030230586813977,0.54030230586813977;0.54030230586813977,0.54030230586813977]
    Matrix cmdbl( Shape(2,2) );
    cmdbl = 1.0,1.0,1.0,1.0;
    BasicPtr cmdblp(new Matrix(cmdbl));
    BasicPtr exp11(new Cos(cmdblp));
    BasicPtr exp11a= exp11->simplify();
    if (exp11a != cmdbl_cosp) return -131;

    return 0;
}


int common( int &argc,  char *argv[])
{
	// Testen, ob das simplify sich nicht mit anderen Expressions verhakt
	std::cout << "--------- Simplify Untereinander -----------" << std::endl;
	Matrix *v1 = new Matrix(Shape(3));
	*v1 = 1,2,3;

	Matrix *A = new Matrix(Shape(3,3));
	*A = 1,2,3,
		 4,5,6,
		 7,8,9;

	Matrix *v2 = new Matrix(Shape(3));
	*v2 = 2,3,4;

	Matrix *ref1 = new Matrix(Shape(3));
	*ref1 = sin(21.0),sin(49.0),sin(77.0);
	Matrix *ref2 = new Matrix(Shape(3));
	*ref2 = cos(21.0),cos(49.0),cos(77.0);

	BasicPtr mulAv2( new Mul( BasicPtr(A), BasicPtr(v2) ));
	BasicPtr addv1Av2( new Add( BasicPtr(v1), mulAv2 ) );
	BasicPtr E1( new Sin(addv1Av2) );
	BasicPtr E2( new Cos(addv1Av2) );

	// E1 und E2 simplifizieren
	E1 = E1->simplify();
	E2 = E2->simplify();

	// Ausgabe
	std::cout << E1->toString() << std::endl;
	std::cout << E2->toString() << std::endl;

	// Auswertung
	if (E1 != BasicPtr(ref1) ) return -1;
	if (E2 != BasicPtr(ref2) ) return -2;


	return 0;
}


int main( int argc,  char *argv[])
{
    int res = 0;
    res = neg(argc,argv);
    if (res !=0) return -1000 + res;
    res = add(argc,argv);
    if (res !=0) return -2000 + res;
    res = mul(argc,argv);
    if (res !=0) return -3000 + res;
    res = pow(argc,argv);
    if (res !=0) return -4000 + res;
    res = sin(argc,argv);
    if (res !=0) return -5000 + res;
    res = cos(argc,argv);
    if (res !=0) return -6000 + res;
	res = common(argc,argv);
	if (res !=0) return -7000 + res;

    return 0;
}