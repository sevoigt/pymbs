#include <iostream>
#include "test_cos.h"
#include "math.h"

int test_cos( int &argc,  char *argv[])
{
    // Symbol und Konstanten anlegen
    BasicPtr a( new Symbol("a") );
    BasicPtr A( new Symbol("A", Shape(3,2)) );
    BasicPtr two( new Int( 2 ) );
    BasicPtr pointTwo( new Real( 0.2 ) );

    // Na dann mal los (Skalar & Matrx)
    BasicPtr cosa( new Cos(a) );
    if (cosa->getType() != Type_Cos) return -1;
    if (cosa->getShape() != Shape()) return -2;
    BasicPtr cosA( new Cos(A) );
    if (cosA->getType() != Type_Cos) return -3;
    if (cosA->getShape() != Shape(3,2)) return -4;

    // Vereinfachungen mit Konstanten
    // Cos(2) = cos(2)
    BasicPtr CosTwo( new Cos(two) );
    BasicPtr cosTwo( new Real( cos(2.0) ) );
    BasicPtr t1(CosTwo->simplify());
    if (t1 != cosTwo) return -10; 
    // Cos(.2) = cos(.2)
    BasicPtr CosPointTwo( new Cos(pointTwo) );
    BasicPtr cosPointTwo( new Real( cos(0.2) ) );
    BasicPtr t2(CosPointTwo->simplify());
    if (t2 != cosPointTwo) return -11; 

    // Komplexe Vereinfachungen
    // Cos(Acos(.)) == (.)
    BasicPtr Acosa( new Acos(a) );
    BasicPtr CosAcosa( new Cos( Acosa ) );
    BasicPtr t3(CosAcosa->simplify());
    if (t3 != a) return -20;
    // Cos(Neg(.)) == Cos(.)
    BasicPtr nega( new Neg(a) );
    BasicPtr cosNega( new Cos(nega) );
    BasicPtr t4(cosNega->simplify());
    if (t4 != cosa) return -21;

    // cos(1.0) = 0.54030230586813977
    BasicPtr cdbl(new Real(1.0));
    BasicPtr cdbl_cos(new Real(0.54030230586813977));
    BasicPtr exp = Cos::New(cdbl);
    if (exp != cdbl_cos) return -133;

    // cos(1) = 0.54030230586813977
    BasicPtr cint(new Int(1));
    BasicPtr exp1 = Cos::New(cint);
    if (exp1 != cdbl_cos) return -134;

    // cos(f) = cos(f) 
    BasicPtr f(new Symbol("f"));
    BasicPtr exp2 = Cos::New(f);
    BasicPtr exp3(new Cos(f));
    if (exp2 != exp3) return -135;

    // cos(-f) = cos(f) 
    BasicPtr exp4 = Neg::New(f);
    BasicPtr exp5 = Cos::New(exp4);
    if (exp5 != exp2) return -136;

    // cos([1,1,1]) = [0.54030230586813977,0.54030230586813977,0.54030230586813977]
    Matrix cvint( Shape(3) );
    cvint = 1,1,1;
    BasicPtr cvintp(new Matrix(cvint));
    BasicPtr exp7 = Cos::New(cvintp);
    Matrix cvdbl_cos( Shape(3) );
    cvdbl_cos = 0.5403023058681397740,0.5403023058681397740,0.5403023058681397740;
    BasicPtr cvdbl_cosp(new Matrix(cvdbl_cos));
    if (exp7 != cvdbl_cosp) return -137;

    // cos([1.0,1.0,1.0]) = [0.54030230586813977,0.54030230586813977,0.54030230586813977]
    Matrix cvdbl( Shape(3) );
    cvdbl = 1.0,1.0,1.0;
    BasicPtr cvdblp(new Matrix(cvdbl));
    BasicPtr exp8 = Cos::New(cvdblp);
    if (exp8 != cvdbl_cosp) return -138;

    // cos([f,f,f]) = [cos(f),cos(f),cos(f)]
    Matrix mat( Shape(3) );
    mat = f,f,f;
    BasicPtr matp(new Matrix(mat));
    BasicPtr exp9 = Cos::New(matp);
    Matrix mat_cos( Shape(3) );
    mat_cos = exp2,exp2,exp2;
    BasicPtr mat_cosp(new Matrix(mat_cos));
    if (exp9 != mat_cosp) return -139;

    // cos([1,1;1,1]) = [0.54030230586813977,0.54030230586813977;0.54030230586813977,0.54030230586813977]
    Matrix cmint( Shape(2,2) );
    cmint = 1,1,
            1,1;
    BasicPtr cmintp(new Matrix(cmint));
    BasicPtr exp10 = Cos::New(cmintp);
    Matrix cmdbl_cos( Shape(2,2) );
    cmdbl_cos = 0.5403023058681397740,0.5403023058681397740,0.5403023058681397740,0.5403023058681397740;
    BasicPtr cmdbl_cosp(new Matrix(cmdbl_cos));
    if (exp10 != cmdbl_cosp) return -140;

    // cos([1.0,1.0;1.0,1.0]) = [0.54030230586813977,0.54030230586813977;0.54030230586813977,0.54030230586813977]
    Matrix cmdbl( Shape(2,2) );
    cmdbl = 1.0,1.0,1.0,1.0;
    BasicPtr cmdblp(new Matrix(cmdbl));
    BasicPtr exp11 = Cos::New(cmdblp);
    if (exp11 != cmdbl_cosp) return -141;

    // cos([f,f;f,f]) = [cos(f),cos(f);cos(f),cos(f)]
    Matrix mat1( Shape(2,2) );
    mat1 = f,f,f,f;
    BasicPtr mat1p(new Matrix(mat1));
    BasicPtr exp12 = Cos::New(mat1p);
    Matrix mat1_cos( Shape(2,2) );
    mat1_cos = exp2,exp2,exp2,exp2;
    BasicPtr mat1_cosp(new Matrix(mat1_cos));
    if (exp12 != mat1_cosp) return -142;


    // der(cos(a)) = -der(a)*sin(a) 
    BasicPtr dercosa = cosa->der();
    if (dercosa !=-(a->der()*Sin::New(a))) return -30;

    return 0;
}