#include <iostream>
#include <map>
#include "Symbolics.h"
#include "FMUPrinter.h"

using namespace Symbolics;

std::map<std::string, int> vR;
FMUPrinter fmuP(&vR);

int fmuTestIntReal()
{
    std::string out = "";
    BasicPtr cint(new Int(-22));
    if ( fmuP.print(cint).compare("(-22)") )
        out += "TEST_ERROR: int fmuTest 1: "+ fmuP.print(cint) + "\n";

    BasicPtr cdbl(new Real(3.33));
    if ( fmuP.print(cdbl).compare("3.33") )
        out += "TEST_ERROR: Real test 1: "+ fmuP.print(cdbl) + "\n";

    BasicPtr cdbl2(new Real(0.0033));
    if ( fmuP.print(cdbl2).compare("0.0033") )
        out += "TEST_ERROR: Real test 2: "+ fmuP.print(cdbl2) + "\n";

    BasicPtr cdbl3(new Real(4.25e-10));
    if ( fmuP.print(cdbl3).compare("4.25e-010") )
        out += "TEST_ERROR: Real test 3: "+ fmuP.print(cdbl3) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -1;
    return 0;
}

int fmuTestSymbol()
{
    std::string out = "";
    BasicPtr sym1(new Symbol("abcd"));
    if ( fmuP.print(sym1).compare("abcd") )
        out += "TEST_ERROR: Symbol test 1: "+ fmuP.print(sym1) + "\n";

    BasicPtr sym2(new Symbol("EFG"));
    if ( fmuP.print(sym2).compare("EFG") )
        out += "TEST_ERROR: Symbol test 2: "+ fmuP.print(sym2) + "\n";

    BasicPtr sym3(new Symbol("_bcd"));
    if ( fmuP.print(sym3).compare("_bcd") )
        out += "TEST_ERROR: Symbol test 3: "+ fmuP.print(sym3) + "\n";

    BasicPtr sym4(new Symbol("testRefSymbol"));
    if ( fmuP.print(sym4).compare("r(testRefSymbol_)") )
        out += "TEST_ERROR: Symbol test 4: "+ fmuP.print(sym4) + "\n";

    BasicPtr sym5(new Symbol("time"));
    if ( fmuP.print(sym5).compare("comp->time") )
        out += "TEST_ERROR: Symbol test 5: "+ fmuP.print(sym5) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -2;
    return 0;
}

int fmuTestMatrix()
{
    std::string out = "";
    BasicPtr ma1(new Matrix(Shape(3,3)));
    if ( fmuP.print(ma1).compare("{{0,0,0},\n        {0,0,0},\n        {0,0,0}}") )
        out += "TEST_ERROR: Matrix test 1: "+ fmuP.print(ma1) + "\n";

    BasicPtrVec v;
    v.push_back(new Real(1.33));
    v.push_back(new Real(2.33));
    v.push_back(new Int(3));
    v.push_back(new Real(4.44));
    BasicPtr ma2(new Matrix(v,Shape(2,2)));
    if ( fmuP.print(ma2).compare("{{1.33,2.33},\n        {3,4.44}}") )
        out += "TEST_ERROR: Matrix test 2: "+ fmuP.print(ma2) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -4;
    return 0;
}

int fmuTestElement()
{
    std::string out = "";
    Shape s = Shape(3,4);
    BasicPtr el1(new Element(new Symbol("testSymbol",s),2,1));
    if ( fmuP.print(el1).compare("testSymbol[2][1]") )
        out += "TEST_ERROR: Element test 1: "+ fmuP.print(el1) + "\n";

    Shape s2= Shape(5);
    BasicPtr el2(new Element(new Symbol("testSymbol2",s2),3,0));
    if ( fmuP.print(el2).compare("testSymbol2[3]") )
        out += "TEST_ERROR: Element test 2: "+ fmuP.print(el2) + "\n";

    BasicPtr el3(new Element(new Symbol("testRefElement",s),2,1));
    if ( fmuP.print(el3).compare("r(testRefElement_2_1_)") )
        out += "TEST_ERROR: Element test 3: "+ fmuP.print(el3) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -8;
    return 0;
}

int fmuTestPow()
{
    std::string out = "";
    Shape s = Shape(3,3);

    BasicPtr pow1(new Pow(new Symbol("testSymbol"),3));
    if ( fmuP.print(pow1).compare("pow(testSymbol,3)") )
        out += "TEST_ERROR: Pow test 1: "+ fmuP.print(pow1) + "\n";
    BasicPtr pow2(new Pow(new Symbol("testSymbol"),3.5));
    if ( fmuP.print(pow2).compare("pow(testSymbol,3.5)") )
        out += "TEST_ERROR: Pow test 2: "+ fmuP.print(pow2) + "\n";

//    BasicPtr pow3(new Pow(new Symbol("testMatrix",s),3));
//    if ( fmuP.print(pow3).compare("pow(testMatrix,3)") )
//        out += "TEST_ERROR: Pow test 3: "+ fmuP.print(pow3) + "\n";

    //Folgendes schl�gt fehl, und muss noch gefixt werden (kein Printer Problem!)
//    BasicPtr pow4(new Pow(new Symbol("testMatrix",s),3.0));
//    if ( fmuP.print(pow4).compare("matpow(testMatrix,3)") )
//        out += "TEST_ERROR: Pow test 4: "+ fmuP.print(pow4) + "\n";

    //Folgendes muss einen Fehler ergeben, da Matrizen mit gebrochenen Exponenten nicht implementiert sind.
//    BasicPtr pow5(new Pow(new Symbol("testMatrix",s),3.5));
//    if ( fmuP.print(pow5).compare("(TEST_ERROR: FortranPrinter: Pow: Base is matrix but exponent is no Integer, it is of type '5' and expands to '3.5d0')") )
//        out += "TEST_ERROR: Pow test 5: "+ fmuP.print(pow5) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -16;
    return 0;
}

int fmuTestSkew()
{
    std::string out = "";
    Shape s = Shape(3);
    BasicPtr sk1(new Skew(new Symbol("testSymbol",s)));
    if ( fmuP.print(sk1).compare("(Error: CPrinter: Skew: Matrix operations not implemented in C)") )
        out += "TEST_ERROR: Skew test 1: "+ fmuP.print(sk1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -32;
    return 0;
}

int fmuTestZero()
{
    std::string out = "";
    Shape s = Shape(3,4);
    BasicPtr z1(new Zero(s));
    if ( fmuP.print(z1).compare("{{0}}") )
        out += "TEST_ERROR: Zero test 1: "+ fmuP.print(z1) + "\n";

    Shape s2= Shape(5);
    BasicPtr z2(new Zero(s2));
    if ( fmuP.print(z2).compare("{0}") )
        out += "TEST_ERROR: Zero test 2: "+ fmuP.print(z2) + "\n";

    BasicPtr z3(new Zero());
    if ( fmuP.print(z3).compare("0") )
        out += "TEST_ERROR: Zero test 3: "+ fmuP.print(z3) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -64;
    return 0;
}

int fmuTestSolve()
{
    std::string out = "";
    Shape s = Shape(4,4);
    Shape s2= Shape(4);
    BasicPtr so1(new Solve(new Symbol("testSymbolA",s),new Symbol("testSymbolB",s2)));
    if ( fmuP.print(so1).compare("(Error: CPrinter: Solve not possible at this point (cannot be inline))") )
        out += "TEST_ERROR: Solve test 1: "+ fmuP.print(so1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -256;
    return 0;
}

int fmuTestAbs()
{
    std::string out = "";
    Shape s = Shape(3,4);
    BasicPtr b1(new Abs(new Symbol("testSymbol",s)));
    if ( fmuP.print(b1).compare("fabs(testSymbol)") )
        out += "TEST_ERROR: Abs test 1: "+ fmuP.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -512;
    return 0;
}

int fmuTestAdd()
{
    std::string out = "";
    BasicPtr b1(new Add(new Symbol("testSymbol"),new Symbol("testSymbol2")));
    if ( fmuP.print(b1).compare("(testSymbol + testSymbol2)") )
        out += "TEST_ERROR: Add test 1: "+ fmuP.print(b1) + "\n";

    BasicPtrVec bv;
    bv.push_back(new Symbol("testSymbol"));
    bv.push_back(new Symbol("testSymbol2"));
    bv.push_back(new Neg(new Symbol("testSymbol3")));
    bv.push_back(new Symbol("testSymbol4"));
    BasicPtr b2(new Add(bv));
    if ( fmuP.print(b2).compare("(testSymbol + testSymbol2 - testSymbol3 + testSymbol4)") )
        out += "TEST_ERROR: Add test 2: "+ fmuP.print(b2) + "\n";

    bv.clear();
    bv.push_back(new Symbol("testSymbol"));
    bv.push_back(new Int(2));
    bv.push_back(new Neg(new Int(3)));
    bv.push_back(new Int(-4));
    bv.push_back(new Neg(new Int(-5)));
    BasicPtr b3(new Add(bv));
    if ( fmuP.print(b3).compare("(testSymbol + 2 - 3 - 4 - (-5))") )
        out += "TEST_ERROR: Add test 3: "+ fmuP.print(b3) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -1024;
    return 0;
}

int fmuTestDer()
{
    std::string out = "";
    Shape s = Shape(3,4);
    BasicPtr b1(new Der(new Symbol("testSymbol",s)));
    if ( fmuP.print(b1).compare("der_testSymbol") )
        out += "TEST_ERROR: Der test 1: "+ fmuP.print(b1) + "\n";

    BasicPtr b2(new Der(new Symbol("testRefSymbol")));
    if ( fmuP.print(b2).compare("r(der_testRefSymbol_)") )
        out += "TEST_ERROR: Der test 2: "+ fmuP.print(b2) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -2048;
    return 0;
}

int fmuTestMul()
{
    std::string out = "";
    BasicPtr b1(new Mul(new Symbol("testSymbol"),new Symbol("testSymbol2")));
    if ( fmuP.print(b1).compare("(testSymbol * testSymbol2)") )
        out += "TEST_ERROR: Mul test 1: "+ fmuP.print(b1) + "\n";

    BasicPtrVec bv;
    bv.push_back(new Symbol("testSymbol"));
    bv.push_back(new Symbol("testSymbol2"));
    bv.push_back(new Neg(new Symbol("testSymbol3")));
    bv.push_back(new Symbol("testSymbol4"));
    BasicPtr b2(new Mul(bv));
    if ( fmuP.print(b2).compare("(testSymbol * testSymbol2 * (-testSymbol3) * testSymbol4)") )
        out += "TEST_ERROR: Mul test 2: "+ fmuP.print(b2) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -4096;
    return 0;
}

int fmuTestNeg()
{
    std::string out = "";
    BasicPtr b1(new Neg(new Symbol("testSymbol")));
    if ( fmuP.print(b1).compare("(-testSymbol)") )
        out += "TEST_ERROR: Neg test 1: "+ fmuP.print(b1) + "\n";

    BasicPtr b2(new Neg(new Real(-2.5)));
    if ( fmuP.print(b2).compare("(-(-2.5))") )
        out += "TEST_ERROR: Neg test 2: "+ fmuP.print(b2) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -8192;
    return 0;
}

int fmuTestSign()
{
    std::string out = "";
    BasicPtr b1(new Sign(new Symbol("testSymbol")));
    if ( fmuP.print(b1).compare("(testSymbol>0?1:(testSymbol<0?-1:0))") )
        out += "TEST_ERROR: Sign test 1: "+ fmuP.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -16384;
    return 0;
}

int fmuTestTranspose()
{
    std::string out = "";
    BasicPtr b1(new Transpose(new Symbol("testSymbol")));
    if ( fmuP.print(b1).compare("(Error: CPrinter: Transpose: Matrix operations not implemented in C)") )
        out += "TEST_ERROR: Transpose test 1: "+ fmuP.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -32768;
    return 0;
}

int fmuTestUnknown()
{
    std::string out = "";
    BasicPtrVec bv;
    bv.push_back(new Symbol("testSymbol"));
    bv.push_back(new Int(2));
    bv.push_back(new Symbol("testSymbol3"));
    BasicPtr b1(new Unknown("testUnknownFunc",bv));
    if ( fmuP.print(b1).compare("testUnknownFunc(testSymbol,2,testSymbol3)") )
        out += "TEST_ERROR: Unknown test 1: "+ fmuP.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -65536;
    return 0;
}

int fmuTestTrig()
{
    std::string out = "";
    BasicPtr b1(new Acos(new Symbol("testSymbol")));
    if ( fmuP.print(b1).compare("acos(testSymbol)") )
        out += "TEST_ERROR: Acos test: "+ fmuP.print(b1) + "\n";

    BasicPtr b2(new Asin(new Symbol("testSymbol")));
    if ( fmuP.print(b2).compare("asin(testSymbol)") )
        out += "TEST_ERROR: Asin test: "+ fmuP.print(b2) + "\n";

    BasicPtr b3(new Atan(new Symbol("testSymbol")));
    if ( fmuP.print(b3).compare("atan(testSymbol)") )
        out += "TEST_ERROR: Atan test: "+ fmuP.print(b3) + "\n";

    BasicPtr b4(new Atan2(new Symbol("testSymbol"), new Symbol("test2")));
    if ( fmuP.print(b4).compare("atan2(testSymbol,test2)") )
        out += "TEST_ERROR: Atan2 test: "+ fmuP.print(b4) + "\n";

    BasicPtr b5(new Cos(new Symbol("testSymbol")));
    if ( fmuP.print(b5).compare("cos(testSymbol)") )
        out += "TEST_ERROR: Cos test: "+ fmuP.print(b5) + "\n";

    BasicPtr b6(new Sin(new Symbol("testSymbol")));
    if ( fmuP.print(b6).compare("sin(testSymbol)") )
        out += "TEST_ERROR: Sin test: "+ fmuP.print(b6) + "\n";

    BasicPtr b7(new Tan(new Symbol("testSymbol")));
    if ( fmuP.print(b7).compare("tan(testSymbol)") )
        out += "TEST_ERROR: Tan test: "+ fmuP.print(b7) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -131072;
    return 0;
}

int fmuTestVergl()
{
    std::string out = "";
    BasicPtr b1(new Equal(new Symbol("testSymbol"),new Symbol("test")));
    if ( fmuP.print(b1).compare("(testSymbol == test)") )
        out += "TEST_ERROR: Equal test: "+ fmuP.print(b1) + "\n";

    BasicPtr b2(new Greater(new Symbol("testSymbol2"),new Symbol("test2")));
    if ( fmuP.print(b2).compare("(testSymbol2>test2)") )
        out += "TEST_ERROR: Greater test: "+ fmuP.print(b2) + "\n";

    BasicPtr b3(new Less(new Symbol("testSymbol3"),new Symbol("test3")));
    if ( fmuP.print(b3).compare("(testSymbol3<test3)") )
        out += "TEST_ERROR: Less test: "+ fmuP.print(b3) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -262144;
    return 0;
}

int fmuTestIf()
{
    std::string out = "";
    BasicPtr b1(new If(new Symbol("testCond"), new Symbol("testArg1"), new Symbol("testArg2")));
    if ( fmuP.print(b1).compare("(testCond ? testArg1 : testArg2)") )
        out += "TEST_ERROR: If test 1: "+ fmuP.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -2097152;
    return 0;
}

int fmuTestComment()
{
    std::string out = "";
    if ( fmuP.comment("Testkommentar").compare(" /*Testkommentar*/") )
        out += "TEST_ERROR: Comment test 1: "+ fmuP.comment("Testkommentar") + "\n";

    std::cout << out;
    if (out.compare(""))
        return -524288;
    return 0;
}

int fmuTestDimension()
{
    std::string out = "";
    Shape s1 = Shape();
    Shape s2 = Shape(6);
    Shape s3 = Shape(7,5);

    if ( fmuP.dimension(new Symbol("testsymbol",s1)).compare("") )
        out += "TEST_ERROR: Dimension test 1: "+ fmuP.dimension(new Symbol("testsymbol",s1)) + "\n";

    if ( fmuP.dimension(new Symbol("testsymbol",s2)).compare("[6]") )
        out += "TEST_ERROR: Dimension test 2: "+ fmuP.dimension(new Symbol("testsymbol",s2)) + "\n";

    if ( fmuP.dimension(new Symbol("testsymbol",s3)).compare("[7][5]") )
        out += "TEST_ERROR: Dimension test 3: "+ fmuP.dimension(new Symbol("testsymbol",s3)) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -1048576;
    return 0;
}

int fmuTestBool()
{
    std::string out = "";
    BasicPtr b1(new Bool(true));
    if ( fmuP.print(b1).compare("fmiTrue") )
        out += "TEST_ERROR: Bool test 1: "+ fmuP.print(b1) + "\n";

    BasicPtr b2(new Bool(false));
    if ( fmuP.print(b2).compare("fmiFalse") )
        out += "TEST_ERROR: Bool test 2: "+ fmuP.print(b2) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -8388608;
    return 0;
}


int fmuMain()
{
    std::cout << "started FMUPrinter tests..." << std::endl;
    
    vR["testRefElement_2_1"] = 1;
    vR["testRefSymbol"] = 2;
    
    int res=0;
    res += fmuTestIntReal();
    res += fmuTestSymbol();
    res += fmuTestMatrix();
    res += fmuTestElement();
    res += fmuTestPow();
    res += fmuTestSkew();
    res += fmuTestZero();
    res += fmuTestSolve();
    res += fmuTestAbs();
    res += fmuTestNeg();
    res += fmuTestAdd();
    res += fmuTestMul();
    res += fmuTestDer();
    res += fmuTestSign();
    res += fmuTestTranspose();
    res += fmuTestUnknown();
    res += fmuTestTrig();
    res += fmuTestVergl();
    res += fmuTestIf();
    res += fmuTestBool();

    res += fmuTestComment();
    res += fmuTestDimension();

    if (res)
        std::cout << "completed FMUPrinter tests with errors! Errorcode: " << res << std::endl;
    else
        std::cout << "FMUPrinter tests successfull." << std::endl;

    return res;
}