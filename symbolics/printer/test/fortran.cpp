#include <iostream>
#include "Symbolics.h"
#include "FortranPrinter.h"

using namespace Symbolics;

FortranPrinter fp;

int fortranTestIntReal()
{
    std::string out = "";
    BasicPtr cint(new Int(-22));
    if ( fp.print(cint).compare("(-22d0)") )
        out += "TEST_ERROR: int fortranTest 1: "+ fp.print(cint) + "\n";

    BasicPtr cdbl(new Real(3.33));
    if ( fp.print(cdbl).compare("3.33d0") )
        out += "TEST_ERROR: Real test 1: "+ fp.print(cdbl) + "\n";

    BasicPtr cdbl2(new Real(0.0033));
    if ( fp.print(cdbl2).compare("0.0033d0") )
        out += "TEST_ERROR: Real test 2: "+ fp.print(cdbl2) + "\n";

    BasicPtr cdbl3(new Real(4.25e-10));
    if ( fp.print(cdbl3).compare("4.25d-010") )
        out += "TEST_ERROR: Real test 3: "+ fp.print(cdbl3) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -1;
    return 0;
}

int fortranTestSymbol()
{
    std::string out = "";
    BasicPtr sym1(new Symbol("abcd"));
    if ( fp.print(sym1).compare("abcd") )
        out += "TEST_ERROR: Symbol test 1: "+ fp.print(sym1) + "\n";

    BasicPtr sym2(new Symbol("EFG"));
    if ( fp.print(sym2).compare("EFG") )
        out += "TEST_ERROR: Symbol test 2: "+ fp.print(sym2) + "\n";

    BasicPtr sym3(new Symbol("_bcd"));
    if ( fp.print(sym3).compare("F_bcd") )
        out += "TEST_ERROR: Symbol test 3: "+ fp.print(sym3) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -2;
    return 0;
}

int fortranTestMatrix()
{
    std::string out = "";
    BasicPtr ma1(new Matrix(Shape(3,3)));
    if ( fp.print(ma1).compare("reshape((/0d0, 0d0, 0d0, &\n        0d0, 0d0, 0d0, &\n        0d0, 0d0, 0d0/), (/3,3/))") )
        out += "TEST_ERROR: Matrix test 1: "+ fp.print(ma1) + "\n";

    BasicPtrVec v;
    v.push_back(new Real(1.33));
    v.push_back(new Real(2.33));
    v.push_back(new Int(3));
    v.push_back(new Real(4.44));
    BasicPtr ma2(new Matrix(v,Shape(2,2)));
    if ( fp.print(ma2).compare("reshape((/1.33d0, 3d0, &\n        2.33d0, 4.44d0/), (/2,2/))") )
        out += "TEST_ERROR: Matrix test 2: "+ fp.print(ma2) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -4;
    return 0;
}

int fortranTestElement()
{
    std::string out = "";
    Shape s = Shape(3,4);
    BasicPtr el1(new Element(new Symbol("testSymbol",s),2,1));
    if ( fp.print(el1).compare("testSymbol(3,2)") )
        out += "TEST_ERROR: Element test 1: "+ fp.print(el1) + "\n";

    Shape s2= Shape(5);
    BasicPtr el2(new Element(new Symbol("testSymbol2",s2),3,0));
    if ( fp.print(el2).compare("testSymbol2(4,1)") )
        out += "TEST_ERROR: Element test 2: "+ fp.print(el2) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -8;
    return 0;
}

int fortranTestPow()
{
    std::string out = "";
    Shape s = Shape(3,3);

    BasicPtr pow1(new Pow(new Symbol("testSymbol"),3));
    if ( fp.print(pow1).compare("(testSymbol**3d0)") )
        out += "TEST_ERROR: Pow test 1: "+ fp.print(pow1) + "\n";
    BasicPtr pow2(new Pow(new Symbol("testSymbol"),3.5));
    if ( fp.print(pow2).compare("(testSymbol**3.5d0)") )
        out += "TEST_ERROR: Pow test 2: "+ fp.print(pow2) + "\n";

    BasicPtr pow3(new Pow(new Symbol("testMatrix",s),3));
    if ( fp.print(pow3).compare("matpow(testMatrix,3)") )
        out += "TEST_ERROR: Pow test 3: "+ fp.print(pow3) + "\n";

    //Folgendes schl�gt fehl, und muss noch gefixt werden (kein Printer Problem!)
    BasicPtr pow4(new Pow(new Symbol("testMatrix",s),3.0));
    if ( fp.print(pow4->simplify()).compare("matpow(testMatrix,3)") )
        out += "TEST_ERROR: Pow test 4: "+ fp.print(pow4) + "\n";

    //Folgendes muss einen Fehler ergeben, da Matrizen mit gebrochenen Exponenten nicht implementiert sind.
    BasicPtr pow5(new Pow(new Symbol("testMatrix",s),3.5));
    if ( fp.print(pow5).compare("(Error: FortranPrinter: Pow: Base is matrix but exponent is no Integer, it is of type '5' and expands to '3.5d0')") )
        out += "TEST_ERROR: Pow test 5: "+ fp.print(pow5) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -16;
    return 0;
}

int fortranTestSkew()
{
    std::string out = "";
    Shape s = Shape(3);
    BasicPtr sk1(new Skew(new Symbol("testSymbol",s)));
    if ( fp.print(sk1).compare("skew(testSymbol)") )
        out += "TEST_ERROR: Skew test 1: "+ fp.print(sk1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -32;
    return 0;
}

int fortranTestZero()
{
    std::string out = "";
    Shape s = Shape(3,4);
    BasicPtr z1(new Zero(s));
    if ( fp.print(z1).compare("reshape((/0d0, 0d0, 0d0, 0d0, 0d0, 0d0, 0d0, 0d0, 0d0, 0d0, 0d0, 0d0/),(/3,4/))") )
        out += "TEST_ERROR: Zero test 1: "+ fp.print(z1) + "\n";

    Shape s2= Shape(5);
    BasicPtr z2(new Zero(s2));
    if ( fp.print(z2).compare("reshape((/0d0, 0d0, 0d0, 0d0, 0d0/),(/5,1/))") )
        out += "TEST_ERROR: Zero test 2: "+ fp.print(z2) + "\n";

    BasicPtr z3(new Zero());
    if ( fp.print(z3).compare("0d0") )
        out += "TEST_ERROR: Zero test 3: "+ fp.print(z3) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -64;
    return 0;
}

int fortranTestInverse()
{
    std::string out = "";
    Shape s = Shape(3,4);
    BasicPtr in1(new Inverse(new Symbol("testSymbol",s)));
    if ( fp.print(in1).compare("MIGS(testSymbol)") )
        out += "TEST_ERROR: Inverse test 1: "+ fp.print(in1) + "\n";

    BasicPtr ma1(new Matrix(Shape(3,3)));
    BasicPtr in2(new Inverse(ma1));
    if ( fp.print(in2).compare("MIGS(reshape((/0d0, 0d0, 0d0, &\n        0d0, 0d0, 0d0, &\n        0d0, 0d0, 0d0/), (/3,3/)))") )
        out += "TEST_ERROR: Inverse test 2: "+ fp.print(in2) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -128;
    return 0;
}

int fortranTestSolve()
{
    std::string out = "";
    Shape s = Shape(4,4);
    Shape s2= Shape(4);
    BasicPtr so1(new Solve(new Symbol("testSymbolA",s),new Symbol("testSymbolB",s2)));
    if ( fp.print(so1).compare("LEGS(testSymbolA, testSymbolB)") )
        out += "TEST_ERROR: Solve test 1: "+ fp.print(so1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -256;
    return 0;
}

int fortranTestAbs()
{
    std::string out = "";
    Shape s = Shape(3,4);
    BasicPtr b1(new Abs(new Symbol("testSymbol",s)));
    if ( fp.print(b1).compare("abs(testSymbol)") )
        out += "TEST_ERROR: Abs test 1: "+ fp.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -512;
    return 0;
}

int fortranTestAdd()
{
    std::string out = "";
    BasicPtr b1(new Add(new Symbol("testSymbol"),new Symbol("testSymbol2")));
    if ( fp.print(b1).compare("(testSymbol + testSymbol2)") )
        out += "TEST_ERROR: Add test 1: "+ fp.print(b1) + "\n";

    BasicPtrVec bv;
    bv.push_back(new Symbol("testSymbol"));
    bv.push_back(new Symbol("testSymbol2"));
    bv.push_back(new Neg(new Symbol("testSymbol3")));
    bv.push_back(new Symbol("testSymbol4"));
    BasicPtr b2(new Add(bv));
    if ( fp.print(b2).compare("(testSymbol + testSymbol2 - testSymbol3 + testSymbol4)") )
        out += "TEST_ERROR: Add test 2: "+ fp.print(b2) + "\n";

    bv.clear();
    bv.push_back(new Symbol("testSymbol"));
    bv.push_back(new Int(2));
    bv.push_back(new Neg(new Int(3)));
    bv.push_back(new Int(-4));
    bv.push_back(new Neg(new Int(-5)));
    BasicPtr b3(new Add(bv));
    if ( fp.print(b3).compare("(testSymbol + 2d0 - 3d0 - 4d0 - (-5d0))") )
        out += "TEST_ERROR: Add test 3: "+ fp.print(b3) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -1024;
    return 0;
}

int fortranTestDer()
{
    std::string out = "";
    Shape s = Shape(3,4);
    BasicPtr b1(new Der(new Symbol("testSymbol",s)));
    if ( fp.print(b1).compare("der_testSymbol") )
        out += "TEST_ERROR: Der test 1: "+ fp.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -2048;
    return 0;
}

int fortranTestMul()
{
    std::string out = "";
    BasicPtr b1(new Mul(new Symbol("testSymbol"),new Symbol("testSymbol2")));
    if ( fp.print(b1).compare("(testSymbol*testSymbol2)") )
        out += "TEST_ERROR: Mul test 1: "+ fp.print(b1) + "\n";

    BasicPtrVec bv;
    bv.push_back(new Symbol("testSymbol"));
    bv.push_back(new Symbol("testSymbol2"));
    bv.push_back(new Neg(new Symbol("testSymbol3")));
    bv.push_back(new Symbol("testSymbol4"));
    BasicPtr b2(new Mul(bv));
    if ( fp.print(b2).compare("(testSymbol*testSymbol2*(-testSymbol3)*testSymbol4)") )
        out += "TEST_ERROR: Mul test 2: "+ fp.print(b2) + "\n";

    Shape s1 = Shape(1,1,4); //Spaltenvektoren m�ssen so erstellt werden, Shape(1,4) erzeugt eine Matrix (ndim=2)
    Shape s2 = Shape(4);
    Shape s3 = Shape(3,3);
    bv.clear();
    bv.push_back(new Symbol("testSymbol"));
    bv.push_back(new Symbol("testSymbol2",s1));
    bv.push_back(new Symbol("testSymbol3",s2));
    BasicPtr b3(new Mul(bv));
    if ( fp.print(b3).compare("scalar(testSymbol*matmul(testSymbol2,testSymbol3))") )
        out += "TEST_ERROR: Mul test 3: "+ fp.print(b3) + "\n";

    bv.clear();
    bv.push_back(new Symbol("testSymbol2",s3));
    bv.push_back(new Symbol("testSymbol3",s3));
    BasicPtr b4(new Mul(bv));
    if ( fp.print(b4).compare("(matmul(testSymbol2,testSymbol3))") )
        out += "TEST_ERROR: Mul test 4: "+ fp.print(b4) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -4096;
    return 0;
}

int fortranTestNeg()
{
    std::string out = "";
    BasicPtr b1(new Neg(new Symbol("testSymbol")));
    if ( fp.print(b1).compare("(-testSymbol)") )
        out += "TEST_ERROR: Neg test 1: "+ fp.print(b1) + "\n";

    BasicPtr b2(new Neg(new Real(-2.5)));
    if ( fp.print(b2).compare("(-(-2.5d0))") )
        out += "TEST_ERROR: Neg test 2: "+ fp.print(b2) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -8192;
    return 0;
}

int fortranTestSign()
{
    std::string out = "";
    BasicPtr b1(new Sign(new Symbol("testSymbol")));
    if ( fp.print(b1).compare("sign(1d0, testSymbol)") )
        out += "TEST_ERROR: Sign test 1: "+ fp.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -16384;
    return 0;
}

int fortranTestTranspose()
{
    std::string out = "";
    BasicPtr b1(new Transpose(new Symbol("testSymbol")));
    if ( fp.print(b1).compare("transpose(testSymbol)") )
        out += "TEST_ERROR: Transpose test 1: "+ fp.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -32768;
    return 0;
}

int fortranTestUnknown()
{
    std::string out = "";
    BasicPtrVec bv;
    bv.push_back(new Symbol("testSymbol"));
    bv.push_back(new Int(2));
    bv.push_back(new Symbol("testSymbol3"));
    BasicPtr b1(new Unknown("testUnknownFunc",bv));
    if ( fp.print(b1).compare("testUnknownFunc(testSymbol,2d0,testSymbol3)") )
        out += "TEST_ERROR: Unknown test 1: "+ fp.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -65536;
    return 0;
}

int fortranTestTrig()
{
    std::string out = "";
    BasicPtr b1(new Acos(new Symbol("testSymbol")));
    if ( fp.print(b1).compare("acos(testSymbol)") )
        out += "TEST_ERROR: Acos test: "+ fp.print(b1) + "\n";

    BasicPtr b2(new Asin(new Symbol("testSymbol")));
    if ( fp.print(b2).compare("asin(testSymbol)") )
        out += "TEST_ERROR: Asin test: "+ fp.print(b2) + "\n";

    BasicPtr b3(new Atan(new Symbol("testSymbol")));
    if ( fp.print(b3).compare("atan(testSymbol)") )
        out += "TEST_ERROR: Atan test: "+ fp.print(b3) + "\n";

    BasicPtr b4(new Atan2(new Symbol("testSymbol"), new Symbol("test2")));
    if ( fp.print(b4).compare("atan2(testSymbol,test2)") )
        out += "TEST_ERROR: Atan2 test: "+ fp.print(b4) + "\n";

    BasicPtr b5(new Cos(new Symbol("testSymbol")));
    if ( fp.print(b5).compare("cos(testSymbol)") )
        out += "TEST_ERROR: Cos test: "+ fp.print(b5) + "\n";

    BasicPtr b6(new Sin(new Symbol("testSymbol")));
    if ( fp.print(b6).compare("sin(testSymbol)") )
        out += "TEST_ERROR: Sin test: "+ fp.print(b6) + "\n";

    BasicPtr b7(new Tan(new Symbol("testSymbol")));
    if ( fp.print(b7).compare("tan(testSymbol)") )
        out += "TEST_ERROR: Tan test: "+ fp.print(b7) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -131072;
    return 0;
}

int fortranTestVergl()
{
    std::string out = "";
    BasicPtr b1(new Equal(new Symbol("testSymbol"),new Symbol("test")));
    if ( fp.print(b1).compare("(testSymbol == test)") )
        out += "TEST_ERROR: Equal test: "+ fp.print(b1) + "\n";

    BasicPtr b2(new Greater(new Symbol("testSymbol2"),new Symbol("test2")));
    if ( fp.print(b2).compare("(testSymbol2>test2)") )
        out += "TEST_ERROR: Greater test: "+ fp.print(b2) + "\n";

    BasicPtr b3(new Less(new Symbol("testSymbol3"),new Symbol("test3")));
    if ( fp.print(b3).compare("(testSymbol3<test3)") )
        out += "TEST_ERROR: Less test: "+ fp.print(b3) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -262144;
    return 0;
}

int fortranTestComment()
{
    std::string out = "";
    if ( fp.comment("Testkommentar").compare(" ! Testkommentar") )
        out += "TEST_ERROR: Comment test 1: "+ fp.comment("Testkommentar") + "\n";

    std::cout << out;
    if (out.compare(""))
        return -524288;
    return 0;
}

int fortranTestDimension()
{
    std::string out = "";
    Shape s1 = Shape();
    Shape s2 = Shape(6);
    Shape s3 = Shape(7,5);

    if ( fp.dimension(new Symbol("testsymbol",s1)).compare("") )
        out += "TEST_ERROR: Dimension test 1: "+ fp.dimension(new Symbol("testsymbol",s1)) + "\n";

    if ( fp.dimension(new Symbol("testsymbol",s2)).compare(", dimension(6,1)") )
        out += "TEST_ERROR: Dimension test 2: "+ fp.dimension(new Symbol("testsymbol",s2)) + "\n";

    if ( fp.dimension(new Symbol("testsymbol",s3)).compare(", dimension(7,5)") )
        out += "TEST_ERROR: Dimension test 3: "+ fp.dimension(new Symbol("testsymbol",s3)) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -1048576;
    return 0;
}

int fortranTestBool()
{
    std::string out = "";
    BasicPtr b1(new Bool(true));
    if ( fp.print(b1).compare(".TRUE.") )
        out += "TEST_ERROR: Bool test 1: "+ fp.print(b1) + "\n";

    BasicPtr b2(new Bool(false));
    if ( fp.print(b2).compare(".FALSE.") )
        out += "TEST_ERROR: Bool test 2: "+ fp.print(b2) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -8388608;
    return 0;
}


int fortranMain()
{
    std::cout << "started FortranPrinter tests..." << std::endl;
    int res=0;
    res += fortranTestIntReal();
    res += fortranTestSymbol();
    res += fortranTestMatrix();
    res += fortranTestElement();
    res += fortranTestPow();
    res += fortranTestSkew();
    res += fortranTestZero();
    res += fortranTestInverse();
    res += fortranTestSolve();
    res += fortranTestAbs();
    res += fortranTestNeg();
    res += fortranTestAdd();
    res += fortranTestMul();
    res += fortranTestDer();
    res += fortranTestSign();
    res += fortranTestTranspose();
    res += fortranTestUnknown();
    res += fortranTestTrig();
    res += fortranTestVergl();
    res += fortranTestBool();

    res += fortranTestComment();
    res += fortranTestDimension();

    if (res)
        std::cout << "completed FortranPrinter tests with errors! Errorcode: " << res << std::endl;
    else
        std::cout << "FortranPrinter tests successfull." << std::endl;

    return res;
}