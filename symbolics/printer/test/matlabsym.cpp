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
#include "MatlabSymPrinter.h"

using namespace Symbolics;

MatlabSymPrinter msp;

int msTestIntReal()
{
    std::string out = "";
    BasicPtr cint(new Int(-22));
    if ( msp.print(cint).compare("(-22)") )
        out += "TEST_ERROR: int msTest 1: "+ msp.print(cint) + "\n";

    BasicPtr cdbl(new Real(3.33));
    if ( msp.print(cdbl).compare("3.33") )
        out += "TEST_ERROR: Real test 1: "+ msp.print(cdbl) + "\n";

    BasicPtr cdbl2(new Real(0.0033));
    if ( msp.print(cdbl2).compare("0.0033") )
        out += "TEST_ERROR: Real test 2: "+ msp.print(cdbl2) + "\n";

    BasicPtr cdbl3(new Real(4.25e-10));
    if ( msp.print(cdbl3).compare("4.25e-010") )
        out += "TEST_ERROR: Real test 3: "+ msp.print(cdbl3) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -1;
    return 0;
}

int msTestSymbol()
{
    std::string out = "";
    BasicPtr sym1(new Symbol("abcd"));
    if ( msp.print(sym1).compare("abcd") )
        out += "TEST_ERROR: Symbol test 1: "+ msp.print(sym1) + "\n";

    BasicPtr sym2(new Symbol("EFG"));
    if ( msp.print(sym2).compare("EFG") )
        out += "TEST_ERROR: Symbol test 2: "+ msp.print(sym2) + "\n";

    BasicPtr sym3(new Symbol("_bcd"));
    if ( msp.print(sym3).compare("M_bcd") )
        out += "TEST_ERROR: Symbol test 3: "+ msp.print(sym3) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -2;
    return 0;
}

int msTestMatrix()
{
    std::string out = "";
    BasicPtr ma1(new Matrix(Shape(3,3)));
    if ( msp.print(ma1).compare("[[0,0,0];\n        [0,0,0];\n        [0,0,0]]") )
        out += "TEST_ERROR: Matrix test 1: "+ msp.print(ma1) + "\n";

    BasicPtrVec v;
    v.push_back(new Real(1.33));
    v.push_back(new Real(2.33));
    v.push_back(new Int(3));
    v.push_back(new Real(4.44));
    BasicPtr ma2(new Matrix(v,Shape(2,2)));
    if ( msp.print(ma2).compare("[[1.33,2.33];\n        [3,4.44]]") )
        out += "TEST_ERROR: Matrix test 2: "+ msp.print(ma2) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -4;
    return 0;
}

int msTestElement()
{
    std::string out = "";
    Shape s = Shape(3,4);
    BasicPtr el1(new Element(new Symbol("testSymbol",s),2,1));
    if ( msp.print(el1).compare("testSymbol(3,2)") )
        out += "TEST_ERROR: Element test 1: "+ msp.print(el1) + "\n";

    Shape s2= Shape(5);
    BasicPtr el2(new Element(new Symbol("testSymbol2",s2),3,0));
    if ( msp.print(el2).compare("testSymbol2(4,1)") )
        out += "TEST_ERROR: Element test 2: "+ msp.print(el2) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -8;
    return 0;
}

int msTestPow()
{
    std::string out = "";
    Shape s = Shape(3,3);

    BasicPtr pow1(new Pow(new Symbol("testSymbol"),3));
    if ( msp.print(pow1).compare("(testSymbol^3)") )
        out += "TEST_ERROR: Pow test 1: "+ msp.print(pow1) + "\n";
    BasicPtr pow2(new Pow(new Symbol("testSymbol"),3.5));
    if ( msp.print(pow2).compare("(testSymbol^3.5)") )
        out += "TEST_ERROR: Pow test 2: "+ msp.print(pow2) + "\n";

    BasicPtr pow3(new Pow(new Symbol("testMatrix",s),3));
    if ( msp.print(pow3).compare("(testMatrix^3)") )
        out += "TEST_ERROR: Pow test 3: "+ msp.print(pow3) + "\n";

    BasicPtr pow4(new Pow(new Symbol("testMatrix",s),3.0));
    if ( msp.print(pow4).compare("(testMatrix^3)") )
        out += "TEST_ERROR: Pow test 4: "+ msp.print(pow4) + "\n";

    BasicPtr pow5(new Pow(new Symbol("testMatrix",s),3.5));
    if ( msp.print(pow5).compare("(testMatrix^3.5)") )
        out += "TEST_ERROR: Pow test 5: "+ msp.print(pow5) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -16;
    return 0;
}

int msTestSkew()
{
    std::string out = "";
    Shape s = Shape(3);
    BasicPtr sk1(new Skew(new Symbol("testSymbol",s)));
    if ( msp.print(sk1).compare("[[0,-testSymbol(3), testSymbol(2)];\n         [testSymbol(3), 0, -testSymbol(1)];\n         [-testSymbol(2), testSymbol(1), 0]]") )
        out += "TEST_ERROR: Skew test 1: "+ msp.print(sk1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -32;
    return 0;
}

int msTestZero()
{
    std::string out = "";
    Shape s = Shape(3,4);
    BasicPtr z1(new Zero(s));
    if ( msp.print(z1).compare("zeros(3,4)") )
        out += "TEST_ERROR: Zero test 1: "+ msp.print(z1) + "\n";

    Shape s2= Shape(5);
    BasicPtr z2(new Zero(s2));
    if ( msp.print(z2).compare("zeros(5,1)") )
        out += "TEST_ERROR: Zero test 2: "+ msp.print(z2) + "\n";

    BasicPtr z3(new Zero());
    if ( msp.print(z3).compare("0") )
        out += "TEST_ERROR: Zero test 3: "+ msp.print(z3) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -64;
    return 0;
}

int msTestInverse()
{
    std::string out = "";
    Shape s = Shape(3,4);
    BasicPtr in1(new Inverse(new Symbol("testSymbol",s)));
    if ( msp.print(in1).compare("inv(testSymbol)") )
        out += "TEST_ERROR: Inverse test 1: "+ msp.print(in1) + "\n";

    BasicPtr ma1(new Matrix(Shape(3,3)));
    BasicPtr in2(new Inverse(ma1));
    if ( msp.print(in2).compare("inv([[0,0,0];\n        [0,0,0];\n        [0,0,0]])") )
        out += "TEST_ERROR: Inverse test 2: "+ msp.print(in2) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -128;
    return 0;
}

int msTestSolve()
{
    std::string out = "";
    Shape s = Shape(4,4);
    Shape s2= Shape(4);
    BasicPtr so1(new Solve(new Symbol("testSymbolA",s),new Symbol("testSymbolB",s2)));
    if ( msp.print(so1).compare("(testSymbolA\\testSymbolB)") )
        out += "TEST_ERROR: Solve test 1: "+ msp.print(so1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -256;
    return 0;
}

int msTestAbs()
{
    std::string out = "";
    Shape s = Shape(3,4);
    BasicPtr b1(new Abs(new Symbol("testSymbol",s)));
    if ( msp.print(b1).compare("abs(testSymbol)") )
        out += "TEST_ERROR: Abs test 1: "+ msp.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -512;
    return 0;
}

int msTestNeg()
{
    std::string out = "";
    BasicPtr b1(new Neg(new Symbol("testSymbol")));
    if ( msp.print(b1).compare("(-testSymbol)") )
        out += "TEST_ERROR: Neg test 1: "+ msp.print(b1) + "\n";

    BasicPtr b2(new Neg(new Real(-2.5)));
    if ( msp.print(b2).compare("(-(-2.5))") )
        out += "TEST_ERROR: Neg test 2: "+ msp.print(b2) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -8192;
    return 0;
}

int msTestAdd()
{
    std::string out = "";
    BasicPtr b1(new Add(new Symbol("testSymbol"),new Symbol("testSymbol2")));
    if ( msp.print(b1).compare("(testSymbol + testSymbol2)") )
        out += "TEST_ERROR: Add test 1: "+ msp.print(b1) + "\n";

    BasicPtrVec bv;
    bv.push_back(new Symbol("testSymbol"));
    bv.push_back(new Symbol("testSymbol2"));
    bv.push_back(new Neg(new Symbol("testSymbol3")));
    bv.push_back(new Symbol("testSymbol4"));
    BasicPtr b2(new Add(bv));
    if ( msp.print(b2).compare("(testSymbol + testSymbol2 - testSymbol3 + testSymbol4)") )
        out += "TEST_ERROR: Add test 2: "+ msp.print(b2) + "\n";

    bv.clear();
    bv.push_back(new Symbol("testSymbol"));
    bv.push_back(new Int(2));
    bv.push_back(new Neg(new Int(3)));
    bv.push_back(new Int(-4));
    bv.push_back(new Neg(new Int(-5)));
    BasicPtr b3(new Add(bv));
    if ( msp.print(b3).compare("(testSymbol + 2 - 3 - 4 - (-5))") )
        out += "TEST_ERROR: Add test 3: "+ msp.print(b3) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -1024;
    return 0;
}

int msTestDer()
{
    std::string out = "";
    Shape s = Shape(3,4);
    BasicPtr b1(new Der(new Symbol("testSymbol",s)));
    if ( msp.print(b1).compare("der_testSymbol") )
        out += "TEST_ERROR: Der test 1: "+ msp.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -2048;
    return 0;
}

int msTestMul()
{
    std::string out = "";
    BasicPtr b1(new Mul(new Symbol("testSymbol"),new Symbol("testSymbol2")));
    if ( msp.print(b1).compare("(testSymbol * testSymbol2)") )
        out += "TEST_ERROR: Mul test 1: "+ msp.print(b1) + "\n";

    BasicPtrVec bv;
    bv.push_back(new Symbol("testSymbol"));
    bv.push_back(new Symbol("testSymbol2"));
    bv.push_back(new Neg(new Symbol("testSymbol3")));
    bv.push_back(new Symbol("testSymbol4"));
    BasicPtr b2(new Mul(bv));
    if ( msp.print(b2).compare("(testSymbol * testSymbol2 * (-testSymbol3) * testSymbol4)") )
        out += "TEST_ERROR: Mul test 2: "+ msp.print(b2) + "\n";

    Shape s1 = Shape(1,1,4); //Spaltenvektoren müssen so erstellt werden, Shape(1,4) erzeugt eine Matrix (ndim=2)
    Shape s2 = Shape(4);
    Shape s3 = Shape(3,3);
    bv.clear();
    bv.push_back(new Symbol("testSymbol"));
    bv.push_back(new Symbol("testSymbol2",s1));
    bv.push_back(new Symbol("testSymbol3",s2));
    BasicPtr b3(new Mul(bv));
    if ( msp.print(b3).compare("(testSymbol * testSymbol2 * testSymbol3)") )
        out += "TEST_ERROR: Mul test 3: "+ msp.print(b3) + "\n";

    bv.clear();
    bv.push_back(new Symbol("testSymbol2",s3));
    bv.push_back(new Symbol("testSymbol3",s3));
    BasicPtr b4(new Mul(bv));
    if ( msp.print(b4).compare("(testSymbol2 * testSymbol3)") )
        out += "TEST_ERROR: Mul test 3: "+ msp.print(b4) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -4096;
    return 0;
}

int msTestSign()
{
    std::string out = "";
    BasicPtr b1(new Sign(new Symbol("testSymbol")));
    if ( msp.print(b1).compare("(testSymbol/abs(testSymbol))") )
        out += "TEST_ERROR: Sign test 1: "+ msp.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -16384;
    return 0;
}

int msTestTranspose()
{
    std::string out = "";
    BasicPtr b1(new Transpose(new Symbol("testSymbol")));
    if ( msp.print(b1).compare("transpose(testSymbol)") )
        out += "TEST_ERROR: Transpose test 1: "+ msp.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -32768;
    return 0;
}

int msTestUnknown()
{
    std::string out = "";
    BasicPtrVec bv;
    bv.push_back(new Symbol("testSymbol"));
    bv.push_back(new Int(2));
    bv.push_back(new Symbol("testSymbol3"));
    BasicPtr b1(new Unknown("testUnknownFunc",bv));
    if ( msp.print(b1).compare("testUnknownFunc(testSymbol,2,testSymbol3)") )
        out += "TEST_ERROR: Unknown test 1: "+ msp.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -65536;
    return 0;
}

int msTestTrig()
{
    std::string out = "";
    BasicPtr b1(new Acos(new Symbol("testSymbol")));
    if ( msp.print(b1).compare("acos(testSymbol)") )
        out += "TEST_ERROR: Acos test: "+ msp.print(b1) + "\n";

    BasicPtr b2(new Asin(new Symbol("testSymbol")));
    if ( msp.print(b2).compare("asin(testSymbol)") )
        out += "TEST_ERROR: Asin test: "+ msp.print(b2) + "\n";

    BasicPtr b3(new Atan(new Symbol("testSymbol")));
    if ( msp.print(b3).compare("atan(testSymbol)") )
        out += "TEST_ERROR: Atan test: "+ msp.print(b3) + "\n";

    BasicPtr b4(new Atan2(new Symbol("testSymbol"), new Symbol("test2")));
    if ( msp.print(b4).compare("(2*atan(testSymbol/((test2^2 + testSymbol^2)^0.5 + test2)))") )
        out += "TEST_ERROR: Atan2 test: "+ msp.print(b4) + "\n";

    BasicPtr b5(new Cos(new Symbol("testSymbol")));
    if ( msp.print(b5).compare("cos(testSymbol)") )
        out += "TEST_ERROR: Cos test: "+ msp.print(b5) + "\n";

    BasicPtr b6(new Sin(new Symbol("testSymbol")));
    if ( msp.print(b6).compare("sin(testSymbol)") )
        out += "TEST_ERROR: Sin test: "+ msp.print(b6) + "\n";

    BasicPtr b7(new Tan(new Symbol("testSymbol")));
    if ( msp.print(b7).compare("tan(testSymbol)") )
        out += "TEST_ERROR: Tan test: "+ msp.print(b7) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -131072;
    return 0;
}

int msTestVergl()
{
    std::string out = "";
    BasicPtr b1(new Equal(new Symbol("testSymbol"),new Symbol("test")));
    if ( msp.print(b1).compare("(testSymbol == test)") )
        out += "TEST_ERROR: Equal test: "+ msp.print(b1) + "\n";

    BasicPtr b2(new Greater(new Symbol("testSymbol2"),new Symbol("test2")));
    if ( msp.print(b2).compare("(testSymbol2>test2)") )
        out += "TEST_ERROR: Greater test: "+ msp.print(b2) + "\n";

    BasicPtr b3(new Less(new Symbol("testSymbol3"),new Symbol("test3")));
    if ( msp.print(b3).compare("(testSymbol3<test3)") )
        out += "TEST_ERROR: Less test: "+ msp.print(b3) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -262144;
    return 0;
}

int msTestComment()
{
    std::string out = "";
    if ( msp.comment("Testkommentar").compare(" % Testkommentar") )
        out += "TEST_ERROR: Comment test 1: "+ msp.comment("Testkommentar") + "\n";

    std::cout << out;
    if (out.compare(""))
        return -524288;
    return 0;
}

int msTestDimension()
{
    std::string out = "";
    Shape s1 = Shape();
    Shape s2 = Shape(6);
    Shape s3 = Shape(7,5);

    if ( msp.dimension(new Symbol("testsymbol",s1)).compare("") )
        out += "TEST_ERROR: Dimension test 1: "+ msp.dimension(new Symbol("testsymbol",s1)) + "\n";

    if ( msp.dimension(new Symbol("testsymbol",s2)).compare(",[6 1]") )
        out += "TEST_ERROR: Dimension test 2: "+ msp.dimension(new Symbol("testsymbol",s2)) + "\n";

    if ( msp.dimension(new Symbol("testsymbol",s3)).compare(",[7 5]") )
        out += "TEST_ERROR: Dimension test 3: "+ msp.dimension(new Symbol("testsymbol",s3)) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -1048576;
    return 0;
}

int msTestJacobian()
{
    std::string out = "";
    Shape s(4);
    BasicPtr b1(new Jacobian(new Symbol("testExp"), new Symbol("testSymbols",s)));
    if ( msp.print(b1).compare("jacobian(testExp,testSymbols)") )
        out += "TEST_ERROR: Jacobian test 1: "+ msp.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -4194304;
    return 0;
}

int msTestBool()
{
    std::string out = "";
    BasicPtr b1(new Bool(true));
    if ( msp.print(b1).compare("true") )
        out += "TEST_ERROR: Bool test 1: "+ msp.print(b1) + "\n";

    BasicPtr b2(new Bool(false));
    if ( msp.print(b2).compare("false") )
        out += "TEST_ERROR: Bool test 2: "+ msp.print(b2) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -8388608;
    return 0;
}


int msMain()
{
    std::cout << "started MatlabSymPrinter tests..." << std::endl;
    int res=0;
    res += msTestIntReal();
    res += msTestSymbol();
    res += msTestMatrix();
    res += msTestElement();
    res += msTestPow();
    res += msTestSkew();
    res += msTestZero();
    res += msTestInverse();
    res += msTestSolve();
    res += msTestAbs();
    res += msTestNeg();
    res += msTestAdd();
    res += msTestMul();
    res += msTestDer();
    res += msTestSign();
    res += msTestTranspose();
    res += msTestUnknown();
    res += msTestTrig();
    res += msTestVergl();
    res += msTestJacobian();
    res += msTestBool();

    res += msTestComment();
    res += msTestDimension();

    if (res)
        std::cout << "completed MatlabSymPrinter tests with errors! Errorcode: " << res << std::endl;
    else
        std::cout << "MatlabSymPrinter tests successfull." << std::endl;

    return res;
}