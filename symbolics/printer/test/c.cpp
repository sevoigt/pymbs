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
#include "Symbolics.h"
#include "CPrinter.h"

using namespace Symbolics;

CPrinter cp;

int CTestIntReal()
{
    std::string out = "";
    BasicPtr cint(new Int(-22));
    if ( cp.print(cint).compare("(-22)") )
        out += "TEST_ERROR: int CTest 1: "+ cp.print(cint) + "\n";

    BasicPtr cdbl(new Real(3.33));
    if ( cp.print(cdbl).compare("3.33") )
        out += "TEST_ERROR: Real test 1: "+ cp.print(cdbl) + "\n";

    BasicPtr cdbl2(new Real(0.0033));
    if ( cp.print(cdbl2).compare("0.0033") )
        out += "TEST_ERROR: Real test 2: "+ cp.print(cdbl2) + "\n";

    BasicPtr cdbl3(new Real(4.25e-10));
    if ( cp.print(cdbl3).compare("4.25e-010") )
        out += "TEST_ERROR: Real test 3: "+ cp.print(cdbl3) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -1;
    return 0;
}

int CTestSymbol()
{
    std::string out = "";
    BasicPtr sym1(new Symbol("abcd"));
    if ( cp.print(sym1).compare("abcd") )
        out += "TEST_ERROR: Symbol test 1: "+ cp.print(sym1) + "\n";

    BasicPtr sym2(new Symbol("EFG"));
    if ( cp.print(sym2).compare("EFG") )
        out += "TEST_ERROR: Symbol test 2: "+ cp.print(sym2) + "\n";

    BasicPtr sym3(new Symbol("_bcd"));
    if ( cp.print(sym3).compare("_bcd") )
        out += "TEST_ERROR: Symbol test 3: "+ cp.print(sym3) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -2;
    return 0;
}

int CTestMatrix()
{
    std::string out = "";
    BasicPtr ma1(new Matrix(Shape(3,3)));
    if ( cp.print(ma1).compare("{{0,0,0},\n        {0,0,0},\n        {0,0,0}}") )
        out += "TEST_ERROR: Matrix test 1: "+ cp.print(ma1) + "\n";

    BasicPtrVec v;
    v.push_back(new Real(1.33));
    v.push_back(new Real(2.33));
    v.push_back(new Int(3));
    v.push_back(new Real(4.44));
    BasicPtr ma2(new Matrix(v,Shape(2,2)));
    if ( cp.print(ma2).compare("{{1.33,2.33},\n        {3,4.44}}") )
        out += "TEST_ERROR: Matrix test 2: "+ cp.print(ma2) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -4;
    return 0;
}

int CTestElement()
{
    std::string out = "";
    Shape s = Shape(3,4);
    BasicPtr el1(new Element(new Symbol("testSymbol",s),2,1));
    if ( cp.print(el1).compare("testSymbol[2][1]") )
        out += "TEST_ERROR: Element test 1: "+ cp.print(el1) + "\n";

    Shape s2= Shape(5);
    BasicPtr el2(new Element(new Symbol("testSymbol2",s2),3,0));
    if ( cp.print(el2).compare("testSymbol2[3]") )
        out += "TEST_ERROR: Element test 2: "+ cp.print(el2) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -8;
    return 0;
}

int CTestPow()
{
    std::string out = "";
    Shape s = Shape(3,3);

    BasicPtr pow1(new Pow(new Symbol("testSymbol"),3));
    if ( cp.print(pow1).compare("pow(testSymbol,3)") )
        out += "TEST_ERROR: Pow test 1: "+ cp.print(pow1) + "\n";
    BasicPtr pow2(new Pow(new Symbol("testSymbol"),3.5));
    if ( cp.print(pow2).compare("pow(testSymbol,3.5)") )
        out += "TEST_ERROR: Pow test 2: "+ cp.print(pow2) + "\n";

//    BasicPtr pow3(new Pow(new Symbol("testMatrix",s),3));
//    if ( cp.print(pow3).compare("pow(testMatrix,3)") )
//        out += "TEST_ERROR: Pow test 3: "+ cp.print(pow3) + "\n";

    //Folgendes schlägt fehl, und muss noch gefixt werden (kein Printer Problem!)
//    BasicPtr pow4(new Pow(new Symbol("testMatrix",s),3.0));
//    if ( cp.print(pow4).compare("matpow(testMatrix,3)") )
//        out += "TEST_ERROR: Pow test 4: "+ cp.print(pow4) + "\n";

    //Folgendes muss einen Fehler ergeben, da Matrizen mit gebrochenen Exponenten nicht implementiert sind.
//    BasicPtr pow5(new Pow(new Symbol("testMatrix",s),3.5));
//    if ( cp.print(pow5).compare("(TEST_ERROR: FortranPrinter: Pow: Base is matrix but exponent is no Integer, it is of type '5' and expands to '3.5d0')") )
//        out += "TEST_ERROR: Pow test 5: "+ cp.print(pow5) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -16;
    return 0;
}

int CTestSkew()
{
    std::string out = "";
    Shape s = Shape(3);
    BasicPtr sk1(new Skew(new Symbol("testSymbol",s)));
    if ( cp.print(sk1).compare("(Error: CPrinter: Skew: Matrix operations not implemented in C)") )
        out += "TEST_ERROR: Skew test 1: "+ cp.print(sk1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -32;
    return 0;
}

int CTestZero()
{
    std::string out = "";
    Shape s = Shape(3,4);
    BasicPtr z1(new Zero(s));
    if ( cp.print(z1).compare("{{0}}") )
        out += "TEST_ERROR: Zero test 1: "+ cp.print(z1) + "\n";

    Shape s2= Shape(5);
    BasicPtr z2(new Zero(s2));
    if ( cp.print(z2).compare("{0}") )
        out += "TEST_ERROR: Zero test 2: "+ cp.print(z2) + "\n";

    BasicPtr z3(new Zero());
    if ( cp.print(z3).compare("0") )
        out += "TEST_ERROR: Zero test 3: "+ cp.print(z3) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -64;
    return 0;
}

int CTestSolve()
{
    std::string out = "";
    Shape s = Shape(4,4);
    Shape s2= Shape(4);
    BasicPtr so1(new Solve(new Symbol("testSymbolA",s),new Symbol("testSymbolB",s2)));
    if ( cp.print(so1).compare("(Error: CPrinter: Solve not possible at this point (cannot be inline))") )
        out += "TEST_ERROR: Solve test 1: "+ cp.print(so1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -256;
    return 0;
}

int CTestAbs()
{
    std::string out = "";
    Shape s = Shape(3,4);
    BasicPtr b1(new Abs(new Symbol("testSymbol",s)));
    if ( cp.print(b1).compare("fabs(testSymbol)") )
        out += "TEST_ERROR: Abs test 1: "+ cp.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -512;
    return 0;
}

int CTestAdd()
{
    std::string out = "";
    BasicPtr b1(new Add(new Symbol("testSymbol"),new Symbol("testSymbol2")));
    if ( cp.print(b1).compare("(testSymbol + testSymbol2)") )
        out += "TEST_ERROR: Add test 1: "+ cp.print(b1) + "\n";

    BasicPtrVec bv;
    bv.push_back(new Symbol("testSymbol"));
    bv.push_back(new Symbol("testSymbol2"));
    bv.push_back(new Neg(new Symbol("testSymbol3")));
    bv.push_back(new Symbol("testSymbol4"));
    BasicPtr b2(new Add(bv));
    if ( cp.print(b2).compare("(testSymbol + testSymbol2 - testSymbol3 + testSymbol4)") )
        out += "TEST_ERROR: Add test 2: "+ cp.print(b2) + "\n";

    bv.clear();
    bv.push_back(new Symbol("testSymbol"));
    bv.push_back(new Int(2));
    bv.push_back(new Neg(new Int(3)));
    bv.push_back(new Int(-4));
    bv.push_back(new Neg(new Int(-5)));
    BasicPtr b3(new Add(bv));
    if ( cp.print(b3).compare("(testSymbol + 2 - 3 - 4 - (-5))") )
        out += "TEST_ERROR: Add test 3: "+ cp.print(b3) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -1024;
    return 0;
}

int CTestDer()
{
    std::string out = "";
    Shape s = Shape(3,4);
    BasicPtr b1(new Der(new Symbol("testSymbol",s)));
    if ( cp.print(b1).compare("der_testSymbol") )
        out += "TEST_ERROR: Der test 1: "+ cp.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -2048;
    return 0;
}

int CTestMul()
{
    std::string out = "";
    BasicPtr b1(new Mul(new Symbol("testSymbol"),new Symbol("testSymbol2")));
    if ( cp.print(b1).compare("(testSymbol * testSymbol2)") )
        out += "TEST_ERROR: Mul test 1: "+ cp.print(b1) + "\n";

    BasicPtrVec bv;
    bv.push_back(new Symbol("testSymbol"));
    bv.push_back(new Symbol("testSymbol2"));
    bv.push_back(new Neg(new Symbol("testSymbol3")));
    bv.push_back(new Symbol("testSymbol4"));
    BasicPtr b2(new Mul(bv));
    if ( cp.print(b2).compare("(testSymbol * testSymbol2 * (-testSymbol3) * testSymbol4)") )
        out += "TEST_ERROR: Mul test 2: "+ cp.print(b2) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -4096;
    return 0;
}

int CTestNeg()
{
    std::string out = "";
    BasicPtr b1(new Neg(new Symbol("testSymbol")));
    if ( cp.print(b1).compare("(-testSymbol)") )
        out += "TEST_ERROR: Neg test 1: "+ cp.print(b1) + "\n";

    BasicPtr b2(new Neg(new Real(-2.5)));
    if ( cp.print(b2).compare("(-(-2.5))") )
        out += "TEST_ERROR: Neg test 2: "+ cp.print(b2) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -8192;
    return 0;
}

int CTestSign()
{
    std::string out = "";
    BasicPtr b1(new Sign(new Symbol("testSymbol")));
    if ( cp.print(b1).compare("(testSymbol>0?1:(testSymbol<0?-1:0))") )
        out += "TEST_ERROR: Sign test 1: "+ cp.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -16384;
    return 0;
}

int CTestTranspose()
{
    std::string out = "";
    BasicPtr b1(new Transpose(new Symbol("testSymbol")));
    if ( cp.print(b1).compare("(Error: CPrinter: Transpose: Matrix operations not implemented in C)") )
        out += "TEST_ERROR: Transpose test 1: "+ cp.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -32768;
    return 0;
}

int CTestUnknown()
{
    std::string out = "";
    BasicPtrVec bv;
    bv.push_back(new Symbol("testSymbol"));
    bv.push_back(new Int(2));
    bv.push_back(new Symbol("testSymbol3"));
    BasicPtr b1(new Unknown("testUnknownFunc",bv));
    if ( cp.print(b1).compare("testUnknownFunc(testSymbol,2,testSymbol3)") )
        out += "TEST_ERROR: Unknown test 1: "+ cp.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -65536;
    return 0;
}

int CTestTrig()
{
    std::string out = "";
    BasicPtr b1(new Acos(new Symbol("testSymbol")));
    if ( cp.print(b1).compare("acos(testSymbol)") )
        out += "TEST_ERROR: Acos test: "+ cp.print(b1) + "\n";

    BasicPtr b2(new Asin(new Symbol("testSymbol")));
    if ( cp.print(b2).compare("asin(testSymbol)") )
        out += "TEST_ERROR: Asin test: "+ cp.print(b2) + "\n";

    BasicPtr b3(new Atan(new Symbol("testSymbol")));
    if ( cp.print(b3).compare("atan(testSymbol)") )
        out += "TEST_ERROR: Atan test: "+ cp.print(b3) + "\n";

    BasicPtr b4(new Atan2(new Symbol("testSymbol"), new Symbol("test2")));
    if ( cp.print(b4).compare("atan2(testSymbol,test2)") )
        out += "TEST_ERROR: Atan2 test: "+ cp.print(b4) + "\n";

    BasicPtr b5(new Cos(new Symbol("testSymbol")));
    if ( cp.print(b5).compare("cos(testSymbol)") )
        out += "TEST_ERROR: Cos test: "+ cp.print(b5) + "\n";

    BasicPtr b6(new Sin(new Symbol("testSymbol")));
    if ( cp.print(b6).compare("sin(testSymbol)") )
        out += "TEST_ERROR: Sin test: "+ cp.print(b6) + "\n";

    BasicPtr b7(new Tan(new Symbol("testSymbol")));
    if ( cp.print(b7).compare("tan(testSymbol)") )
        out += "TEST_ERROR: Tan test: "+ cp.print(b7) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -131072;
    return 0;
}

int CTestVergl()
{
    std::string out = "";
    BasicPtr b1(new Equal(new Symbol("testSymbol"),new Symbol("test")));
    if ( cp.print(b1).compare("(testSymbol == test)") )
        out += "TEST_ERROR: Equal test: "+ cp.print(b1) + "\n";

    BasicPtr b2(new Greater(new Symbol("testSymbol2"),new Symbol("test2")));
    if ( cp.print(b2).compare("(testSymbol2>test2)") )
        out += "TEST_ERROR: Greater test: "+ cp.print(b2) + "\n";

    BasicPtr b3(new Less(new Symbol("testSymbol3"),new Symbol("test3")));
    if ( cp.print(b3).compare("(testSymbol3<test3)") )
        out += "TEST_ERROR: Less test: "+ cp.print(b3) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -262144;
    return 0;
}

int CTestIf()
{
    std::string out = "";
    BasicPtr b1(new If(new Symbol("testCond"), new Symbol("testArg1"), new Symbol("testArg2")));
    if ( cp.print(b1).compare("(testCond ? testArg1 : testArg2)") )
        out += "TEST_ERROR: If test 1: "+ cp.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -2097152;
    return 0;
}

int CTestComment()
{
    std::string out = "";
    if ( cp.comment("Testkommentar").compare(" /*Testkommentar*/") )
        out += "TEST_ERROR: Comment test 1: "+ cp.comment("Testkommentar") + "\n";

    std::cout << out;
    if (out.compare(""))
        return -524288;
    return 0;
}

int CTestDimension()
{
    std::string out = "";
    Shape s1 = Shape();
    Shape s2 = Shape(6);
    Shape s3 = Shape(7,5);

    if ( cp.dimension(new Symbol("testsymbol",s1)).compare("") )
        out += "TEST_ERROR: Dimension test 1: "+ cp.dimension(new Symbol("testsymbol",s1)) + "\n";

    if ( cp.dimension(new Symbol("testsymbol",s2)).compare("[6]") )
        out += "TEST_ERROR: Dimension test 2: "+ cp.dimension(new Symbol("testsymbol",s2)) + "\n";

    if ( cp.dimension(new Symbol("testsymbol",s3)).compare("[7][5]") )
        out += "TEST_ERROR: Dimension test 3: "+ cp.dimension(new Symbol("testsymbol",s3)) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -1048576;
    return 0;
}

int CTestBool()
{
    std::string out = "";
    BasicPtr b1(new Bool(true));
    if ( cp.print(b1).compare("1") )
        out += "TEST_ERROR: Bool test 1: "+ cp.print(b1) + "\n";

    BasicPtr b2(new Bool(false));
    if ( cp.print(b2).compare("0") )
        out += "TEST_ERROR: Bool test 2: "+ cp.print(b2) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -8388608;
    return 0;
}


int cMain()
{
    std::cout << "started CPrinter tests..." << std::endl;
    int res=0;
    res += CTestIntReal();
    res += CTestSymbol();
    res += CTestMatrix();
    res += CTestElement();
    res += CTestPow();
    res += CTestSkew();
    res += CTestZero();
    res += CTestSolve();
    res += CTestAbs();
    res += CTestNeg();
    res += CTestAdd();
    res += CTestMul();
    res += CTestDer();
    res += CTestSign();
    res += CTestTranspose();
    res += CTestUnknown();
    res += CTestTrig();
    res += CTestVergl();
    res += CTestIf();
    res += CTestBool();

    res += CTestComment();
    res += CTestDimension();

    if (res)
        std::cout << "completed CPrinter tests with errors! Errorcode: " << res << std::endl;
    else
        std::cout << "CPrinter tests successfull." << std::endl;

    return res;
}