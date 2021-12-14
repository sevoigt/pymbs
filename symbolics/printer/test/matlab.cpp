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
#include "MatlabPrinter.h"

using namespace Symbolics;

MatlabPrinter mp;

int matTestIntReal()
{
    std::string out = "";
    BasicPtr cint(new Int(-22));
    if ( mp.print(cint).compare("(-22)") )
        out += "TEST_ERROR: int matTest 1: "+ mp.print(cint) + "\n";

    BasicPtr cdbl(new Real(3.33));
    if ( mp.print(cdbl).compare("3.33") )
        out += "TEST_ERROR: Real test 1: "+ mp.print(cdbl) + "\n";

    BasicPtr cdbl2(new Real(0.0033));
    if ( mp.print(cdbl2).compare("0.0033") )
        out += "TEST_ERROR: Real test 2: "+ mp.print(cdbl2) + "\n";

    BasicPtr cdbl3(new Real(4.25e-10));
    if ( mp.print(cdbl3).compare("4.25e-010") )
        out += "TEST_ERROR: Real test 3: "+ mp.print(cdbl3) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -1;
    return 0;
}

int matTestSymbol()
{
    std::string out = "";
    BasicPtr sym1(new Symbol("abcd"));
    if ( mp.print(sym1).compare("abcd") )
        out += "TEST_ERROR: Symbol test 1: "+ mp.print(sym1) + "\n";

    BasicPtr sym2(new Symbol("EFG"));
    if ( mp.print(sym2).compare("EFG") )
        out += "TEST_ERROR: Symbol test 2: "+ mp.print(sym2) + "\n";

    BasicPtr sym3(new Symbol("_bcd"));
    if ( mp.print(sym3).compare("M_bcd") )
        out += "TEST_ERROR: Symbol test 3: "+ mp.print(sym3) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -2;
    return 0;
}

int matTestMatrix()
{
    std::string out = "";
    BasicPtr ma1(new Matrix(Shape(3,3)));
    if ( mp.print(ma1).compare("[[0,0,0];\n        [0,0,0];\n        [0,0,0]]") )
        out += "TEST_ERROR: Matrix test 1: "+ mp.print(ma1) + "\n";

    BasicPtrVec v;
    v.push_back(new Real(1.33));
    v.push_back(new Real(2.33));
    v.push_back(new Int(3));
    v.push_back(new Real(4.44));
    BasicPtr ma2(new Matrix(v,Shape(2,2)));
    if ( mp.print(ma2).compare("[[1.33,2.33];\n        [3,4.44]]") )
        out += "TEST_ERROR: Matrix test 2: "+ mp.print(ma2) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -4;
    return 0;
}

int matTestElement()
{
    std::string out = "";
    Shape s = Shape(3,4);
    BasicPtr el1(new Element(new Symbol("testSymbol",s),2,1));
    if ( mp.print(el1).compare("testSymbol(3,2)") )
        out += "TEST_ERROR: Element test 1: "+ mp.print(el1) + "\n";

    Shape s2= Shape(5);
    BasicPtr el2(new Element(new Symbol("testSymbol2",s2),3,0));
    if ( mp.print(el2).compare("testSymbol2(4,1)") )
        out += "TEST_ERROR: Element test 2: "+ mp.print(el2) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -8;
    return 0;
}

int matTestPow()
{
    std::string out = "";
    Shape s = Shape(3,3);

    BasicPtr pow1(new Pow(new Symbol("testSymbol"),3));
    if ( mp.print(pow1).compare("(testSymbol^3)") )
        out += "TEST_ERROR: Pow test 1: "+ mp.print(pow1) + "\n";
    BasicPtr pow2(new Pow(new Symbol("testSymbol"),3.5));
    if ( mp.print(pow2).compare("(testSymbol^3.5)") )
        out += "TEST_ERROR: Pow test 2: "+ mp.print(pow2) + "\n";

    BasicPtr pow3(new Pow(new Symbol("testMatrix",s),3));
    if ( mp.print(pow3).compare("(testMatrix^3)") )
        out += "TEST_ERROR: Pow test 3: "+ mp.print(pow3) + "\n";

    BasicPtr pow4(new Pow(new Symbol("testMatrix",s),3.0));
    if ( mp.print(pow4).compare("(testMatrix^3)") )
        out += "TEST_ERROR: Pow test 4: "+ mp.print(pow4) + "\n";

    BasicPtr pow5(new Pow(new Symbol("testMatrix",s),3.5));
    if ( mp.print(pow5).compare("(testMatrix^3.5)") )
        out += "TEST_ERROR: Pow test 5: "+ mp.print(pow5) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -16;
    return 0;
}

int matTestSkew()
{
    std::string out = "";
    Shape s = Shape(3);
    BasicPtr sk1(new Skew(new Symbol("testSymbol",s)));
    if ( mp.print(sk1).compare("[[0,-testSymbol(3), testSymbol(2)];\n         [testSymbol(3), 0, -testSymbol(1)];\n         [-testSymbol(2), testSymbol(1), 0]]") )
        out += "TEST_ERROR: Skew test 1: "+ mp.print(sk1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -32;
    return 0;
}

int matTestZero()
{
    std::string out = "";
    Shape s = Shape(3,4);
    BasicPtr z1(new Zero(s));
    if ( mp.print(z1).compare("zeros(3,4)") )
        out += "TEST_ERROR: Zero test 1: "+ mp.print(z1) + "\n";

    Shape s2= Shape(5);
    BasicPtr z2(new Zero(s2));
    if ( mp.print(z2).compare("zeros(5,1)") )
        out += "TEST_ERROR: Zero test 2: "+ mp.print(z2) + "\n";

    BasicPtr z3(new Zero());
    if ( mp.print(z3).compare("0") )
        out += "TEST_ERROR: Zero test 3: "+ mp.print(z3) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -64;
    return 0;
}

int matTestInverse()
{
    std::string out = "";
    Shape s = Shape(3,4);
    BasicPtr in1(new Inverse(new Symbol("testSymbol",s)));
    if ( mp.print(in1).compare("inv(testSymbol)") )
        out += "TEST_ERROR: Inverse test 1: "+ mp.print(in1) + "\n";

    BasicPtr ma1(new Matrix(Shape(3,3)));
    BasicPtr in2(new Inverse(ma1));
    if ( mp.print(in2).compare("inv([[0,0,0];\n        [0,0,0];\n        [0,0,0]])") )
        out += "TEST_ERROR: Inverse test 2: "+ mp.print(in2) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -128;
    return 0;
}

int matTestSolve()
{
    std::string out = "";
    Shape s = Shape(4,4);
    Shape s2= Shape(4);
    BasicPtr so1(new Solve(new Symbol("testSymbolA",s),new Symbol("testSymbolB",s2)));
    if ( mp.print(so1).compare("(testSymbolA\\testSymbolB)") )
        out += "TEST_ERROR: Solve test 1: "+ mp.print(so1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -256;
    return 0;
}

int matTestAbs()
{
    std::string out = "";
    Shape s = Shape(3,4);
    BasicPtr b1(new Abs(new Symbol("testSymbol",s)));
    if ( mp.print(b1).compare("abs(testSymbol)") )
        out += "TEST_ERROR: Abs test 1: "+ mp.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -512;
    return 0;
}

int matTestNeg()
{
    std::string out = "";
    BasicPtr b1(new Neg(new Symbol("testSymbol")));
    if ( mp.print(b1).compare("(-testSymbol)") )
        out += "TEST_ERROR: Neg test 1: "+ mp.print(b1) + "\n";

    BasicPtr b2(new Neg(new Real(-2.5)));
    if ( mp.print(b2).compare("(-(-2.5))") )
        out += "TEST_ERROR: Neg test 2: "+ mp.print(b2) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -8192;
    return 0;
}

int matTestAdd()
{
    std::string out = "";
    BasicPtr b1(new Add(new Symbol("testSymbol"),new Symbol("testSymbol2")));
    if ( mp.print(b1).compare("(testSymbol + testSymbol2)") )
        out += "TEST_ERROR: Add test 1: "+ mp.print(b1) + "\n";

    BasicPtrVec bv;
    bv.push_back(new Symbol("testSymbol"));
    bv.push_back(new Symbol("testSymbol2"));
    bv.push_back(new Neg(new Symbol("testSymbol3")));
    bv.push_back(new Symbol("testSymbol4"));
    BasicPtr b2(new Add(bv));
    if ( mp.print(b2).compare("(testSymbol + testSymbol2 - testSymbol3 + testSymbol4)") )
        out += "TEST_ERROR: Add test 2: "+ mp.print(b2) + "\n";

    bv.clear();
    bv.push_back(new Symbol("testSymbol"));
    bv.push_back(new Int(2));
    bv.push_back(new Neg(new Int(3)));
    bv.push_back(new Int(-4));
    bv.push_back(new Neg(new Int(-5)));
    BasicPtr b3(new Add(bv));
    if ( mp.print(b3).compare("(testSymbol + 2 - 3 - 4 - (-5))") )
        out += "TEST_ERROR: Add test 3: "+ mp.print(b3) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -1024;
    return 0;
}

int matTestDer()
{
    std::string out = "";
    Shape s = Shape(3,4);
    BasicPtr b1(new Der(new Symbol("testSymbol",s)));
    if ( mp.print(b1).compare("der_testSymbol") )
        out += "TEST_ERROR: Der test 1: "+ mp.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -2048;
    return 0;
}

int matTestMul()
{
    std::string out = "";
    BasicPtr b1(new Mul(new Symbol("testSymbol"),new Symbol("testSymbol2")));
    if ( mp.print(b1).compare("(testSymbol * testSymbol2)") )
        out += "TEST_ERROR: Mul test 1: "+ mp.print(b1) + "\n";

    BasicPtrVec bv;
    bv.push_back(new Symbol("testSymbol"));
    bv.push_back(new Symbol("testSymbol2"));
    bv.push_back(new Neg(new Symbol("testSymbol3")));
    bv.push_back(new Symbol("testSymbol4"));
    BasicPtr b2(new Mul(bv));
    if ( mp.print(b2).compare("(testSymbol * testSymbol2 * (-testSymbol3) * testSymbol4)") )
        out += "TEST_ERROR: Mul test 2: "+ mp.print(b2) + "\n";

    Shape s1 = Shape(1,1,4); //Spaltenvektoren müssen so erstellt werden, Shape(1,4) erzeugt eine Matrix (ndim=2)
    Shape s2 = Shape(4);
    Shape s3 = Shape(3,3);
    bv.clear();
    bv.push_back(new Symbol("testSymbol"));
    bv.push_back(new Symbol("testSymbol2",s1));
    bv.push_back(new Symbol("testSymbol3",s2));
    BasicPtr b3(new Mul(bv));
    if ( mp.print(b3).compare("(testSymbol * testSymbol2 * testSymbol3)") )
        out += "TEST_ERROR: Mul test 3: "+ mp.print(b3) + "\n";

    bv.clear();
    bv.push_back(new Symbol("testSymbol2",s3));
    bv.push_back(new Symbol("testSymbol3",s3));
    BasicPtr b4(new Mul(bv));
    if ( mp.print(b4).compare("(testSymbol2 * testSymbol3)") )
        out += "TEST_ERROR: Mul test 3: "+ mp.print(b4) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -4096;
    return 0;
}

int matTestSign()
{
    std::string out = "";
    BasicPtr b1(new Sign(new Symbol("testSymbol")));
    if ( mp.print(b1).compare("sign(testSymbol)") )
        out += "TEST_ERROR: Sign test 1: "+ mp.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -16384;
    return 0;
}

int matTestTranspose()
{
    std::string out = "";
    BasicPtr b1(new Transpose(new Symbol("testSymbol")));
    if ( mp.print(b1).compare("transpose(testSymbol)") )
        out += "TEST_ERROR: Transpose test 1: "+ mp.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -32768;
    return 0;
}

int matTestUnknown()
{
    std::string out = "";
    BasicPtrVec bv;
    bv.push_back(new Symbol("testSymbol"));
    bv.push_back(new Int(2));
    bv.push_back(new Symbol("testSymbol3"));
    BasicPtr b1(new Unknown("testUnknownFunc",bv));
    if ( mp.print(b1).compare("testUnknownFunc(testSymbol,2,testSymbol3)") )
        out += "TEST_ERROR: Unknown test 1: "+ mp.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -65536;
    return 0;
}

int matTestTrig()
{
    std::string out = "";
    BasicPtr b1(new Acos(new Symbol("testSymbol")));
    if ( mp.print(b1).compare("acos(testSymbol)") )
        out += "TEST_ERROR: Acos test: "+ mp.print(b1) + "\n";

    BasicPtr b2(new Asin(new Symbol("testSymbol")));
    if ( mp.print(b2).compare("asin(testSymbol)") )
        out += "TEST_ERROR: Asin test: "+ mp.print(b2) + "\n";

    BasicPtr b3(new Atan(new Symbol("testSymbol")));
    if ( mp.print(b3).compare("atan(testSymbol)") )
        out += "TEST_ERROR: Atan test: "+ mp.print(b3) + "\n";

    BasicPtr b4(new Atan2(new Symbol("testSymbol"), new Symbol("test2")));
    if ( mp.print(b4).compare("atan2(testSymbol,test2)") )
        out += "TEST_ERROR: Atan2 test: "+ mp.print(b4) + "\n";

    BasicPtr b5(new Cos(new Symbol("testSymbol")));
    if ( mp.print(b5).compare("cos(testSymbol)") )
        out += "TEST_ERROR: Cos test: "+ mp.print(b5) + "\n";

    BasicPtr b6(new Sin(new Symbol("testSymbol")));
    if ( mp.print(b6).compare("sin(testSymbol)") )
        out += "TEST_ERROR: Sin test: "+ mp.print(b6) + "\n";

    BasicPtr b7(new Tan(new Symbol("testSymbol")));
    if ( mp.print(b7).compare("tan(testSymbol)") )
        out += "TEST_ERROR: Tan test: "+ mp.print(b7) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -131072;
    return 0;
}

int matTestVergl()
{
    std::string out = "";
    BasicPtr b1(new Equal(new Symbol("testSymbol"),new Symbol("test")));
    if ( mp.print(b1).compare("(testSymbol == test)") )
        out += "TEST_ERROR: Equal test: "+ mp.print(b1) + "\n";

    BasicPtr b2(new Greater(new Symbol("testSymbol2"),new Symbol("test2")));
    if ( mp.print(b2).compare("(testSymbol2>test2)") )
        out += "TEST_ERROR: Greater test: "+ mp.print(b2) + "\n";

    BasicPtr b3(new Less(new Symbol("testSymbol3"),new Symbol("test3")));
    if ( mp.print(b3).compare("(testSymbol3<test3)") )
        out += "TEST_ERROR: Less test: "+ mp.print(b3) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -262144;
    return 0;
}

int matTestComment()
{
    std::string out = "";
    if ( mp.comment("Testkommentar").compare(" % Testkommentar") )
        out += "TEST_ERROR: Comment test 1: "+ mp.comment("Testkommentar") + "\n";

    std::cout << out;
    if (out.compare(""))
        return -524288;
    return 0;
}

int matTestJacobian()
{
    std::string out = "";
    Shape s(4);
    BasicPtr b1(new Jacobian(new Symbol("testExp"), new Symbol("testSymbols",s)));
    if ( mp.print(b1).compare("jacobian(testExp,testSymbols)") )
        out += "TEST_ERROR: Jacobian test 1: "+ mp.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -4194304;
    return 0;
}

int matTestBool()
{
    std::string out = "";
    BasicPtr b1(new Bool(true));
    if ( mp.print(b1).compare("true") )
        out += "TEST_ERROR: Bool test 1: "+ mp.print(b1) + "\n";

    BasicPtr b2(new Bool(false));
    if ( mp.print(b2).compare("false") )
        out += "TEST_ERROR: Bool test 2: "+ mp.print(b2) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -8388608;
    return 0;
}

int matMain()
{
    std::cout << "started MatlabPrinter tests..." << std::endl;
    int res=0;
    res += matTestIntReal();
    res += matTestSymbol();
    res += matTestMatrix();
    res += matTestElement();
    res += matTestPow();
    res += matTestSkew();
    res += matTestZero();
    res += matTestInverse();
    res += matTestSolve();
    res += matTestAbs();
    res += matTestNeg();
    res += matTestAdd();
    res += matTestMul();
    res += matTestDer();
    res += matTestSign();
    res += matTestTranspose();
    res += matTestUnknown();
    res += matTestTrig();
    res += matTestVergl();
    res += matTestJacobian();
    res += matTestBool();

    res += matTestComment();

    if (res)
        std::cout << "completed MatlabPrinter tests with errors! Errorcode: " << res << std::endl;
    else
        std::cout << "MatlabPrinter tests successfull." << std::endl;

    return res;
}