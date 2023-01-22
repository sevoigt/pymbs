#include <iostream>
#include "Symbolics.h"

using namespace Symbolics;

int intDbl( int &argc,  char *argv[])
{

    // intDbl(1.0) = 1.0
    BasicPtr cdbl(new Real(1.0));
    BasicPtr exp = Util::intDbl(cdbl);
    BasicPtr scdbl(new Real(1.0));
    if (exp != scdbl) return -5;

    // intDbl(1) = 1.0
    BasicPtr cint(new Int(1));
    BasicPtr exp1 = Util::intDbl(cint);
    if (exp1 != cdbl) return -6;

    // intDbl(b) = Error
    BasicPtr b(new Symbol("b"));
    try
    {
        BasicPtr exp2 = Util::intDbl(b);
       return -7;
    }
    catch(InternalError)
    {
    }
    catch(...)
    {
        return -8;
    }
    return 0;
}

int getas( int &argc,  char *argv[])
{
    // Symbol anlegen
    BasicPtr basic( new Symbol("e") );
    // Versuchen richtig zu casten
    try
    {
         Symbol* symbol = Util::getAsPtr<Symbol>(basic);
    }
    catch (...) { return -1; };

    // Versuchen falsch zu casten
    try
    {
         Add* add = Util::getAsPtr<Add>(basic);
        return -2;
    }
    catch (InternalError) {  }
    catch (...) { return -3; };
    // Alles ok.
    return 0;
}

int is_Zero( int &argc,  char *argv[])
{
    // is_Zero(0.0) = true
    BasicPtr cdbl(new Real(0.0));
    if (!Util::is_Zero(cdbl)) return -19;

    // is_Zero(0) = true
    BasicPtr cint(new Int(0));
    if (!Util::is_Zero(cint)) return -20;

    // is_Zero(1.0) = false
    BasicPtr cdbl_one(new Real(1.0));
    if (Util::is_Zero(cdbl_one)) return -21;

    // is_Zero(1) = false
    BasicPtr cint_one(new Int(1));
    if (Util::is_Zero(cint_one)) return -22;

    // is_Zero(a) = false
    BasicPtr h(new Symbol("h"));
    if (Util::is_Zero(h)) return -24;

    // is_Zero([0.0,0.0,0.0]) = true
    Matrix cvdbl( Shape(3) );
    cvdbl = 0.0,0.0,0.0;
    BasicPtr cvdblp(new Matrix(cvdbl));
    if (!Util::is_Zero(cvdblp)) return -25;

    // is_Zero([0,0,0]) = true
    Matrix cvint( Shape(3) );
    cvint = 0,0,0;
    BasicPtr cvintp(new Matrix(cvint));
    if (!Util::is_Zero(cvintp)) return -26;

    // is_Zero([0.0,0.0,1.0]) = false
    cvdbl = 0.0,0.0,1.0;
    BasicPtr cvdblp1(new Matrix(cvdbl));
    if (Util::is_Zero(cvdblp1)) return -27;

    // is_Zero([0,0,1]) = false
    cvint = 0,0,1;
    BasicPtr cvintp1(new Matrix(cvint));
    if (Util::is_Zero(cvintp1)) return -28;

    // is_Zero([0.0,0.0,0.0;0.0,0.0,0.0;0.0,0.0,0.0]) = true
    Matrix cmdbl( Shape(3,3) );
    cmdbl = 0.0,0.0,0.0,
            0.0,0.0,0.0,
            0.0,0.0,0.0;
    BasicPtr cmdblp(new Matrix(cmdbl));
    if (!Util::is_Zero(cmdblp)) return -29;

    // is_Zero([0,0,0;0,0,0;0,0,0]) = true
    Matrix cmint( Shape(3,3) );
    cmint = 0,0,0,
            0,0,0,
            0,0,0;
    BasicPtr cmintp(new Matrix(cmint));
    if (!Util::is_Zero(cmintp)) return -30;

    // is_Zero([0.0,0.0,0.0;0.0,0.0,0.0;0.0,0.0,1.0]) = false
    cmdbl = 0.0,0.0,0.0,
            0.0,0.0,0.0,
            0.0,0.0,1.0;
    BasicPtr cmdblp1(new Matrix(cmdbl));
    if (Util::is_Zero(cmdblp1)) return -31;

    // is_Zero([0,0,0;0,0,0;0,0,1]) = false
    cmint = 0,0,0,
            0,0,0,
            0,0,1;
    BasicPtr cmintp1(new Matrix(cmint));
    if (Util::is_Zero(cmintp1)) return -32;

    return 0;
}

int is_One( int &argc,  char *argv[])
{
    // is_One(1.0) = true
    BasicPtr cdbl(new Real(1.0));
    if (!Util::is_One(cdbl)) return -33;

    // is_One(1) = true
    BasicPtr cint(new Int(1));
    if (!Util::is_One(cint)) return -34;

    // is_One(0.0) = false
    BasicPtr cdbl_zero(new Real(0.0));
    if (Util::is_One(cdbl_zero)) return -35;

    // is_One(0) = false
    BasicPtr cint_zero(new Int(0));
    if (Util::is_One(cint_zero)) return -36;

    // is_One(i) = false
    BasicPtr i(new Symbol("i"));
    if (Util::is_One(i)) return -37;

    // is_One([1.0,1.0,1.0]) = false
    Matrix cvdbl( Shape(3) );
    cvdbl = 1.0,1.0,1.0;
    BasicPtr cvdblp(new Matrix(cvdbl));
    if (!Util::is_One(cvdblp)) return -38;

    // is_One([1,1,1]) = false
    Matrix cvint( Shape(3) );
    cvint = 1,1,1;
    BasicPtr cvintp(new Matrix(cvint));
    if (!Util::is_One(cvintp)) return -39;

    // is_One([0.0,0.0,1.0]) = false
    cvdbl = 0.0,0.0,1.0;
    BasicPtr cvdblp1(new Matrix(cvdbl));
    if (Util::is_One(cvdblp1)) return -40;

    // is_One([0,0,1]) = false
    cvint = 0,0,1;
    BasicPtr cvintp1(new Matrix(cvint));
    if (Util::is_One(cvintp1)) return -41;

    // is_One([1.0,0.0,0.0;0.0,1.0,0.0;0.0,0.0,1.0]) = true
    Matrix cmdbl( Shape(3,3) );
    cmdbl = 1.0,0.0,0.0,
            0.0,1.0,0.0,
            0.0,0.0,1.0;
    BasicPtr cmdblp(new Matrix(cmdbl));
    if (!Util::is_One(cmdblp)) return -42;

    // is_One([1,0,0;0,1,0;0,0,1]) = true
    Matrix cmint( Shape(3,3) );
    cmint = 1,0,0,
            0,1,0,
            0,0,1;
    BasicPtr cmintp(new Matrix(cmint));
    if (!Util::is_One(cmintp)) return -43;

    // is_One([0.0,0.0,0.0;0.0,0.0,0.0;0.0,0.0,1.0]) = false
    cmdbl = 0.0,0.0,0.0,
            0.0,0.0,0.0,
            0.0,0.0,1.0;
    BasicPtr cmdblp1(new Matrix(cmdbl));
    if (Util::is_One(cmdblp1)) return -44;

    // is_One([0,0,0;0,0,0;0,0,1]) = false
    cmint = 0,0,0,
            0,0,0,
            0,0,1;
    BasicPtr cmintp1(new Matrix(cmint));
    if (Util::is_One(cmintp1)) return -45;

    return 0;
}

// Konstruktoren testen
int construct( int &argc,  char *argv[])
{
    // 3 Symbole anlegen
    BasicPtr j(new Symbol("j"));
    BasicPtr k(new Symbol("k"));
    BasicPtr l(new Symbol("l"));

    // Mul mit BasicPtr anlegen
	BasicPtrVec args1;
	args1.push_back(j);	args1.push_back(k);	args1.push_back(l);
    BasicPtr exp1( new Mul(args1) );

    return 0;
}

int has_Function( int &argc,  char *argv[])
{
    BasicPtr m(new Symbol("m"));
    BasicPtr n(new Symbol("n"));

    BasicPtr derexp = new Der(m);
    BasicPtr exp = Mul::New(m,derexp);

    if (!Util::has_Function(exp,Symbolics::Type_Mul))
        return -46;

    BasicPtr exp1 = Add::New(exp,n);
    BasicPtr exp2 = Add::New(m,exp);

    if (!Util::has_Function(exp2,Symbolics::Type_Mul))
      return -47;

    if (!Util::has_Function(exp2,Symbolics::Type_Der))
      return -48;

    return 0;
}

int eye( int &argc,  char *argv[])
{
    BasicPtr eye = Util::eye(Shape(3,3));
    if (!Util::is_One(eye)) return -100;

    BasicPtr eye1 = Util::eye(Shape());
    if (!Util::is_One(eye1)) return -101;

    return 0;
}

int is_Const( int &argc,  char *argv[])
{
    BasicPtr m(new Symbol("m"));
    if (Util::is_Const(m)) return -150;

    BasicPtr i(new Int(1));
    if (!Util::is_Const(i)) return -151;

       BasicPtr r(new Real(1.0));
    if (!Util::is_Const(r)) return -152;

    Matrix *cvint = new Matrix( Shape(3) );
    *cvint = 1,2,3;
    BasicPtr cvintp(cvint);
    if (!Util::is_Const(cvintp)) return -153;


    BasicPtr a(new Symbol("a"));
 
    Shape s;
    Matrix *c = new Matrix( s );
    *c = a;
    BasicPtr cp(c);
    if (Util::is_Const(cp)) return -154;

    BasicPtr j(new Symbol("j"));
    BasicPtr k(new Symbol("k"));
    BasicPtr l(new Symbol("l"));
	
	BasicPtrVec args1;
	args1.push_back(j); args1.push_back(k); args1.push_back(l);
    BasicPtr exp1( new Mul(args1) );
    if (Util::is_Const(exp1)) return -155;
	BasicPtrVec arg2; 
	arg2.push_back(j); arg2.push_back(k); arg2.push_back(l);
    BasicPtr exp2( new Add(arg2) );
    if (Util::is_Const(exp2)) return -156;


    return 0;
}

int main( int argc,  char *argv[])
{
  int res = 0;
  res = intDbl(argc,argv);
  if (res !=0) return res;
  res = getas(argc,argv);
  if (res !=0) return res;
  res = is_Zero(argc,argv);
  if (res !=0) return res;
  res = is_One(argc,argv);
  if (res !=0) return res;
  res = construct(argc,argv);
  if (res !=0) return res;
  res = has_Function(argc,argv);
  if (res !=0) return res;
  res = eye(argc,argv);
  if (res !=0) return res;
  res = is_Const(argc,argv);
  if (res !=0) return res;

    return 0;
}