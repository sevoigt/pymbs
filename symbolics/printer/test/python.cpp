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
#include "PythonPrinter.h"

using namespace Symbolics;

PythonPrinter pp;

int pyTestIntReal()
{
    std::string out = "";
    BasicPtr cint(new Int(-22));
    if ( pp.print(cint).compare("(-22)") )
        out += "TEST_ERROR: int pyTest 1: "+ pp.print(cint) + "\n";

    BasicPtr cdbl(new Real(3.33));
    if ( pp.print(cdbl).compare("3.33") )
        out += "TEST_ERROR: Real test 1: "+ pp.print(cdbl) + "\n";

    BasicPtr cdbl2(new Real(0.0033));
    if ( pp.print(cdbl2).compare("0.0033") )
        out += "TEST_ERROR: Real test 2: "+ pp.print(cdbl2) + "\n";

    BasicPtr cdbl3(new Real(4.25e-10));
    if ( pp.print(cdbl3).compare("4.25e-010") )
        out += "TEST_ERROR: Real test 3: "+ pp.print(cdbl3) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -1;
    return 0;
}

int pyTestSymbol()
{
    std::string out = "";
    BasicPtr sym1(new Symbol("abcd"));
    if ( pp.print(sym1).compare("abcd") )
        out += "TEST_ERROR: Symbol test 1: "+ pp.print(sym1) + "\n";

    BasicPtr sym2(new Symbol("EFG"));
    if ( pp.print(sym2).compare("EFG") )
        out += "TEST_ERROR: Symbol test 2: "+ pp.print(sym2) + "\n";

    BasicPtr sym3(new Symbol("_bcd"));
    if ( pp.print(sym3).compare("_bcd") )
        out += "TEST_ERROR: Symbol test 3: "+ pp.print(sym3) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -2;
    return 0;
}

int pyTestMatrix()
{
    std::string out = "";
    BasicPtr ma1(new Matrix(Shape(3,3)));
    if ( pp.print(ma1).compare("matrix([[0,0,0],\n        [0,0,0],\n        [0,0,0]])") )
        out += "TEST_ERROR: Matrix test 1: "+ pp.print(ma1) + "\n";

    BasicPtrVec v;
    v.push_back(new Real(1.33));
    v.push_back(new Real(2.33));
    v.push_back(new Int(3));
    v.push_back(new Real(4.44));
    BasicPtr ma2(new Matrix(v,Shape(2,2)));
    if ( pp.print(ma2).compare("matrix([[1.33,2.33],\n        [3,4.44]])") )
        out += "TEST_ERROR: Matrix test 2: "+ pp.print(ma2) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -4;
    return 0;
}

int pyTestElement()
{
    std::string out = "";
    Shape s = Shape(3,4);
    BasicPtr el1(new Element(new Symbol("testSymbol",s),2,1));
    if ( pp.print(el1).compare("testSymbol[2,1]") )
        out += "TEST_ERROR: Element test 1: "+ pp.print(el1) + "\n";

    Shape s2= Shape(5);
    BasicPtr el2(new Element(new Symbol("testSymbol2",s2),3,0));
    if ( pp.print(el2).compare("testSymbol2[3,0]") )
        out += "TEST_ERROR: Element test 2: "+ pp.print(el2) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -8;
    return 0;
}

int pyTestPow()
{
    std::string out = "";
    Shape s = Shape(3,3);

    BasicPtr pow1(new Pow(new Symbol("testSymbol"),3));
    if ( pp.print(pow1).compare("(testSymbol**3)") )
        out += "TEST_ERROR: Pow test 1: "+ pp.print(pow1) + "\n";
    BasicPtr pow2(new Pow(new Symbol("testSymbol"),3.5));
    if ( pp.print(pow2).compare("(testSymbol**3.5)") )
        out += "TEST_ERROR: Pow test 2: "+ pp.print(pow2) + "\n";

    BasicPtr pow3(new Pow(new Symbol("testMatrix",s),3));
    if ( pp.print(pow3).compare("(testMatrix**3)") )
        out += "TEST_ERROR: Pow test 3: "+ pp.print(pow3) + "\n";

    BasicPtr pow4(new Pow(new Symbol("testMatrix",s),3.0));
    if ( pp.print(pow4).compare("(testMatrix**3)") )
        out += "TEST_ERROR: Pow test 4: "+ pp.print(pow4) + "\n";

    BasicPtr pow5(new Pow(new Symbol("testMatrix",s),3.5));
    if ( pp.print(pow5).compare("(testMatrix**3.5)") )
        out += "TEST_ERROR: Pow test 5: "+ pp.print(pow5) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -16;
    return 0;
}

int pyTestSkew()
{
    std::string out = "";
    Shape s = Shape(3);
    BasicPtr sk1(new Skew(new Symbol("testSymbol",s)));
    if ( pp.print(sk1).compare("matrix([[0,-testSymbol[2,0], testSymbol[1,0]], [testSymbol[2,0], 0, -testSymbol[0,0]], [-testSymbol[1,0], testSymbol[0,0], 0]])") )
        out += "TEST_ERROR: Skew test 1: "+ pp.print(sk1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -32;
    return 0;
}

int pyTestZero()
{
    std::string out = "";
    Shape s = Shape(3,4);
    BasicPtr z1(new Zero(s));
    if ( pp.print(z1).compare("matrix(zeros( (3,4) ))") )
        out += "TEST_ERROR: Zero test 1: "+ pp.print(z1) + "\n";

    Shape s2= Shape(5);
    BasicPtr z2(new Zero(s2));
    if ( pp.print(z2).compare("matrix(zeros( (5,1) ))") )
        out += "TEST_ERROR: Zero test 2: "+ pp.print(z2) + "\n";

    BasicPtr z3(new Zero());
    if ( pp.print(z3).compare("0") )
        out += "TEST_ERROR: Zero test 3: "+ pp.print(z3) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -64;
    return 0;
}

int pyTestInverse()
{
    std::string out = "";
    Shape s = Shape(3,4);
    BasicPtr in1(new Inverse(new Symbol("testSymbol",s)));
    if ( pp.print(in1).compare("testSymbol.I") )
        out += "TEST_ERROR: Inverse test 1: "+ pp.print(in1) + "\n";

    BasicPtr ma1(new Matrix(Shape(3,3)));
    BasicPtr in2(new Inverse(ma1));
    if ( pp.print(in2).compare("matrix([[0,0,0],\n        [0,0,0],\n        [0,0,0]]).I") )
        out += "TEST_ERROR: Inverse test 2: "+ pp.print(in2) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -128;
    return 0;
}

int pyTestSolve()
{
    std::string out = "";
    Shape s = Shape(4,4);
    Shape s2= Shape(4);
    BasicPtr so1(new Solve(new Symbol("testSymbolA",s),new Symbol("testSymbolB",s2)));
    if ( pp.print(so1).compare("matrix(linalg.solve(testSymbolA,testSymbolB))") )
        out += "TEST_ERROR: Solve test 1: "+ pp.print(so1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -256;
    return 0;
}

int pyTestAbs()
{
    std::string out = "";
    Shape s = Shape(3,4);
    BasicPtr b1(new Abs(new Symbol("testSymbol",s)));
    if ( pp.print(b1).compare("abs(testSymbol)") )
        out += "TEST_ERROR: Abs test 1: "+ pp.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -512;
    return 0;
}

int pyTestNeg()
{
    std::string out = "";
    BasicPtr b1(new Neg(new Symbol("testSymbol")));
    if ( pp.print(b1).compare("(-testSymbol)") )
        out += "TEST_ERROR: Neg test 1: "+ pp.print(b1) + "\n";

    BasicPtr b2(new Neg(new Real(-2.5)));
    if ( pp.print(b2).compare("(-(-2.5))") )
        out += "TEST_ERROR: Neg test 2: "+ pp.print(b2) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -8192;
    return 0;
}

int pyTestAdd()
{
    std::string out = "";
    BasicPtr b1(new Add(new Symbol("testSymbol"),new Symbol("testSymbol2")));
    if ( pp.print(b1).compare("(testSymbol + testSymbol2)") )
        out += "TEST_ERROR: Add test 1: "+ pp.print(b1) + "\n";

    BasicPtrVec bv;
    bv.push_back(new Symbol("testSymbol"));
    bv.push_back(new Symbol("testSymbol2"));
    bv.push_back(new Neg(new Symbol("testSymbol3")));
    bv.push_back(new Symbol("testSymbol4"));
    BasicPtr b2(new Add(bv));
    if ( pp.print(b2).compare("(testSymbol + testSymbol2 - testSymbol3 + testSymbol4)") )
        out += "TEST_ERROR: Add test 2: "+ pp.print(b2) + "\n";

    bv.clear();
    bv.push_back(new Symbol("testSymbol"));
    bv.push_back(new Int(2));
    bv.push_back(new Neg(new Int(3)));
    bv.push_back(new Int(-4));
    bv.push_back(new Neg(new Int(-5)));
    BasicPtr b3(new Add(bv));
    if ( pp.print(b3).compare("(testSymbol + 2 - 3 - 4 - (-5))") )
        out += "TEST_ERROR: Add test 3: "+ pp.print(b3) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -1024;
    return 0;
}

int pyTestDer()
{
    std::string out = "";
    Shape s = Shape(3,4);
    BasicPtr b1(new Der(new Symbol("testSymbol",s)));
    if ( pp.print(b1).compare("der_testSymbol") )
        out += "TEST_ERROR: Der test 1: "+ pp.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -2048;
    return 0;
}

int pyTestMul()
{
    std::string out = "";
    BasicPtr b1(new Mul(new Symbol("testSymbol"),new Symbol("testSymbol2")));
    if ( pp.print(b1).compare("(testSymbol * testSymbol2)") )
        out += "TEST_ERROR: Mul test 1: "+ pp.print(b1) + "\n";

    BasicPtrVec bv;
    bv.push_back(new Symbol("testSymbol"));
    bv.push_back(new Symbol("testSymbol2"));
    bv.push_back(new Neg(new Symbol("testSymbol3")));
    bv.push_back(new Symbol("testSymbol4"));
    BasicPtr b2(new Mul(bv));
    if ( pp.print(b2).compare("(testSymbol * testSymbol2 * (-testSymbol3) * testSymbol4)") )
        out += "TEST_ERROR: Mul test 2: "+ pp.print(b2) + "\n";

    Shape s1 = Shape(1,1,4); //Spaltenvektoren müssen so erstellt werden, Shape(1,4) erzeugt eine Matrix (ndim=2)
    Shape s2 = Shape(4);
    Shape s3 = Shape(3,3);
    bv.clear();
    bv.push_back(new Symbol("testSymbol"));
    bv.push_back(new Symbol("testSymbol2",s1));
    bv.push_back(new Symbol("testSymbol3",s2));
    BasicPtr b3(new Mul(bv));
    if ( pp.print(b3).compare("(testSymbol * testSymbol2 * testSymbol3)[0,0]") )
        out += "TEST_ERROR: Mul test 3: "+ pp.print(b3) + "\n";

    bv.clear();
    bv.push_back(new Symbol("testSymbol2",s3));
    bv.push_back(new Symbol("testSymbol3",s3));
    BasicPtr b4(new Mul(bv));
    if ( pp.print(b4).compare("(testSymbol2 * testSymbol3)") )
        out += "TEST_ERROR: Mul test 3: "+ pp.print(b4) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -4096;
    return 0;
}

int pyTestSign()
{
    std::string out = "";
    BasicPtr b1(new Sign(new Symbol("testSymbol")));
    if ( pp.print(b1).compare("sign(testSymbol)") )
        out += "TEST_ERROR: Sign test 1: "+ pp.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -16384;
    return 0;
}

int pyTestTranspose()
{
    std::string out = "";
    BasicPtr b1(new Transpose(new Symbol("testSymbol")));
    if ( pp.print(b1).compare("testSymbol.T") )
        out += "TEST_ERROR: Transpose test 1: "+ pp.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -32768;
    return 0;
}

int pyTestUnknown()
{
    std::string out = "";
    BasicPtrVec bv;
    bv.push_back(new Symbol("testSymbol"));
    bv.push_back(new Int(2));
    bv.push_back(new Symbol("testSymbol3"));
    BasicPtr b1(new Unknown("testUnknownFunc",bv));
    if ( pp.print(b1).compare("testUnknownFunc(testSymbol,2,testSymbol3)") )
        out += "TEST_ERROR: Unknown test 1: "+ pp.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -65536;
    return 0;
}

int pyTestTrig()
{
    std::string out = "";
    BasicPtr b1(new Acos(new Symbol("testSymbol")));
    if ( pp.print(b1).compare("acos(testSymbol)") )
        out += "TEST_ERROR: Acos test: "+ pp.print(b1) + "\n";

    BasicPtr b2(new Asin(new Symbol("testSymbol")));
    if ( pp.print(b2).compare("asin(testSymbol)") )
        out += "TEST_ERROR: Asin test: "+ pp.print(b2) + "\n";

    BasicPtr b3(new Atan(new Symbol("testSymbol")));
    if ( pp.print(b3).compare("atan(testSymbol)") )
        out += "TEST_ERROR: Atan test: "+ pp.print(b3) + "\n";

    BasicPtr b4(new Atan2(new Symbol("testSymbol"), new Symbol("test2")));
    if ( pp.print(b4).compare("atan2(testSymbol,test2)") )
        out += "TEST_ERROR: Atan2 test: "+ pp.print(b4) + "\n";

    BasicPtr b5(new Cos(new Symbol("testSymbol")));
    if ( pp.print(b5).compare("cos(testSymbol)") )
        out += "TEST_ERROR: Cos test: "+ pp.print(b5) + "\n";

    BasicPtr b6(new Sin(new Symbol("testSymbol")));
    if ( pp.print(b6).compare("sin(testSymbol)") )
        out += "TEST_ERROR: Sin test: "+ pp.print(b6) + "\n";

    BasicPtr b7(new Tan(new Symbol("testSymbol")));
    if ( pp.print(b7).compare("tan(testSymbol)") )
        out += "TEST_ERROR: Tan test: "+ pp.print(b7) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -131072;
    return 0;
}

int pyTestVergl()
{
    std::string out = "";
    BasicPtr b1(new Equal(new Symbol("testSymbol"),new Symbol("test")));
    if ( pp.print(b1).compare("(testSymbol == test)") )
        out += "TEST_ERROR: Equal test: "+ pp.print(b1) + "\n";

    BasicPtr b2(new Greater(new Symbol("testSymbol2"),new Symbol("test2")));
    if ( pp.print(b2).compare("(testSymbol2>test2)") )
        out += "TEST_ERROR: Greater test: "+ pp.print(b2) + "\n";

    BasicPtr b3(new Less(new Symbol("testSymbol3"),new Symbol("test3")));
    if ( pp.print(b3).compare("(testSymbol3<test3)") )
        out += "TEST_ERROR: Less test: "+ pp.print(b3) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -262144;
    return 0;
}

int pyTestComment()
{
    std::string out = "";
    if ( pp.comment("Testkommentar").compare(" # Testkommentar") )
        out += "TEST_ERROR: Comment test 1: "+ pp.comment("Testkommentar") + "\n";

    std::cout << out;
    if (out.compare(""))
        return -524288;
    return 0;
}

int pyTestIf()
{
    std::string out = "";
    BasicPtr b1(new If(new Symbol("testCond"), new Symbol("testArg1"), new Symbol("testArg2")));
    if ( pp.print(b1).compare("(testArg1 if testCond else testArg2)") )
        out += "TEST_ERROR: If test 1: "+ pp.print(b1) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -2097152;
    return 0;
}

int pyTestBool()
{
    std::string out = "";
    BasicPtr b1(new Bool(true));
    if ( pp.print(b1).compare("True") )
        out += "TEST_ERROR: Bool test 1: "+ pp.print(b1) + "\n";

    BasicPtr b2(new Bool(false));
    if ( pp.print(b2).compare("False") )
        out += "TEST_ERROR: Bool test 2: "+ pp.print(b2) + "\n";

    std::cout << out;
    if (out.compare(""))
        return -8388608;
    return 0;
}

int pyMain()
{
    std::cout << "started PythonPrinter tests..." << std::endl;
    int res=0;
    res += pyTestIntReal();
    res += pyTestSymbol();
    res += pyTestMatrix();
    res += pyTestElement();
    res += pyTestPow();
    res += pyTestSkew();
    res += pyTestZero();
    res += pyTestInverse();
    res += pyTestSolve();
    res += pyTestAbs();
    res += pyTestNeg();
    res += pyTestAdd();
    res += pyTestMul();
    res += pyTestDer();
    res += pyTestSign();
    res += pyTestTranspose();
    res += pyTestUnknown();
    res += pyTestTrig();
    res += pyTestVergl();
    res += pyTestIf();
    res += pyTestBool();

    res += pyTestComment();
    
    if (res)
        std::cout << "completed PythonPrinter tests with errors! Errorcode: " << res << std::endl;
    else
        std::cout << "PythonPrinter tests successfull." << std::endl;

    return res;
}