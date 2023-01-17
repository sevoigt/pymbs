#include <iostream>
#include "Symbolics.h"

using namespace Symbolics;

int main( int argc,  char *argv[])
{
    Int cint(42);
    Real cdbl(1.234 );
    Matrix cvint( Shape(3) );
    Matrix cvdbl( Shape(4) );
    Matrix cmint( Shape(2,3) );
    Matrix cmdbl( Shape(3,2) );

    try    {
#pragma region Anlegen von Konstanten (Skalar)
    // int-Skalar anlegen
    if (cint.getShape() != Shape(0,1,1)) return -2;
    if (cint.getType() != Type_Int) return -3;
    if (cint != 42) return -4;

    // double-Skalar anlegen
    if (cdbl.getShape() != Shape(0,1,1)) return -6;
    if (cdbl.getType() != Type_Real) return -7;
    if (cdbl != 1.234) return -8;
#pragma endregion
    } catch (IndexError) { return -1001; };

    try    {
#pragma region Anlegen von Konstanten (Vektor)
    // int-Vektor anlegen
    cvint = 1,2,3;
    if (cvint.getShape() != Shape(1,3,1)) return -10;
    if (cvint.getType() != Type_Matrix) return -11;
    if (cvint(0) != 1) return -12;
    if (cvint(1) != 2) return -13;
    if (cvint(2) != 3) return -14;
    try {
        cvint(-1);
        return -16;
    } catch (IndexError) {}
    catch (...) { return - 17; };
    try {
        cvint(3);
        return -18;
    } catch (IndexError) {}
    catch (...) { return - 19; };

    // dbl-Vektor anlegen
    cvdbl = 1.1,2.2,3.3,4.4;
    if (cvdbl.getShape() != Shape(1,4,1)) return -21;
    if (cvdbl.getType() != Type_Matrix) return -22;
    if (cvdbl(0) != 1.1) return -23;
    if (cvdbl(1) != 2.2) return -24;
    if (cvdbl(2) != 3.3) return -25;
    if (cvdbl(3) != 4.4) return -26;

#pragma endregion
    } catch (IndexError) { return -1002; };

    try {
#pragma region Anlegen von Konstanten (Matrix)
    // int-Matrix anlegen
    cmint = 1,2,3,
            4,5,6;
    if (cmint.getShape() != Shape(2,2,3)) return -32;
    if (cmint.getType() != Type_Matrix) return -33;
    if (cmint(0,0) != 1) return -34;
    if (cmint(0,1) != 2) return -35;
    if (cmint(0,2) != 3) return -36;
    if (cmint(1,0) != 4) return -37;
    if (cmint(1,1) != 5) return -38;
    if (cmint(1,2) != 6) return -39;
    try {
        cmint(-1,-1);
        return -40;
    } catch (IndexError) {}
    catch (...) { return -41; };
    try {
        cmint(-1,0);
        return -42;
    } catch (IndexError) {}
    catch (...) { return -43; };
    try {
        cmint(0,-1);
        return -44;
    } catch (IndexError) {}
    catch (...) { return -45; };
    try {
        cmint(2,2);
        return -46;
    } catch (IndexError) {}
    catch (...) { return -47; };
    try {
        cmint(1,3);
        return -48;
    } catch (IndexError) {}
    catch (...) { return -49; };
    try {
        cmint(2,3);
        return -50;
    } catch (IndexError) {}
    catch (...) { return -51; };

    // dbl-Matrix anlegen
    cmdbl = 1.1,2.2,
            3.3,4.4,
            5.5,6.6;
    if (cmdbl.getShape() != Shape(2,3,2)) return -61;
    if (cmdbl.getType() != Type_Matrix) return -62;
    if (cmdbl(0,0) != 1.1) return -63;
    if (cmdbl(0,1) != 2.2) return -64;
    if (cmdbl(1,0) != 3.3) return -65;
    if (cmdbl(1,1) != 4.4) return -66;
    if (cmdbl(2,0) != 5.5) return -66;
    if (cmdbl(2,1) != 6.6) return -66;

#pragma endregion
    } catch (IndexError) { return -1003; };

    try    {
#pragma region Rechnen mit Konstanten (Skalar)
    // Skalar
    Int cplus( cint + cint );
    if (!cplus.is_Scalar()) return -100;
    if (cplus != cint*2) return -101;
    Int cminus( cint - cint );
    if (!cminus.is_Scalar()) return -102;
    if (cminus != 0) return -103;

    Real cdplus( cdbl + cdbl );
    if (!cdplus.is_Scalar()) return -104;
    if (cdplus != cdbl*2) return -105;
    Real cdminus( cdbl - cdbl );
    if (!cdminus.is_Scalar()) return -106;
    if (cdminus != 0.0) return -107;

    Int cmul( cint * cint );
    if (!cmul.is_Scalar()) return -108;
    if (cmul != cint*cint) return -109;

#pragma endregion
    } catch (IndexError) { return -1004; };

    try {
#pragma region Vektor/Skalar
    // Vektor +- Skalar
    Matrix cplusvs( cvint + cint );
    if (cplusvs.getShape() != cvint.getShape()) return -110;
    if (cplusvs(0) != cvint(0) + cint) return -111;
    if (cplusvs(1) != cvint(1) + cint) return -112;
    if (cplusvs(2) != cvint(2) + cint) return -113;

    Matrix cminusvs( cvint - cint );
    if (cminusvs.getShape() != cvint.getShape()) return -115;
    if (cminusvs(0) != cvint(0) - cint) return -116;
    if (cminusvs(1) != cvint(1) - cint) return -117;
    if (cminusvs(2) != cvint(2) - cint) return -118;


    // Skalar +- Vektor
    Matrix cplussv( cint + cvint );
    if (cplussv.getShape() != cvint.getShape()) return -120;
    if (cplussv(0) != cvint(0) + cint) return -121;
    if (cplussv(1) != cvint(1) + cint) return -122;
    if (cplussv(2) != cvint(2) + cint) return -123;

    Matrix cminussv( cint - cvint );
    if (cminussv.getShape() != cvint.getShape()) return -125;
    if (cminussv(0) != cint - cvint(0)) return -126;
    if (cminussv(1) != cint - cvint(1)) return -127;
    if (cminussv(2) != cint - cvint(2)) return -128;

    // Vektor +- Vektor
    Matrix cplusvv( cvint + cvint );
    if (cplusvv.getShape() != cvint.getShape()) return -130;
    if (cplusvv(0) != cvint(0)+cvint(0)) return -131;
    if (cplusvv(1) != cvint(1)+cvint(1)) return -132;
    if (cplusvv(2) != cvint(2)+cvint(2)) return -133;

    Matrix cminusvv( cvint - cvint );
    if (cminusvv.getShape() != cvint.getShape()) return -135;
    if (cminusvv(0) != cvint(0)-cvint(0)) return -136;
    if (cminusvv(1) != cvint(1)-cvint(1)) return -137;
    if (cminusvv(2) != cvint(2)-cvint(2)) return -138;

    // Skalar * Vektor, Vektor*Skalar
    Matrix cmulsv( cint * cvint );
    if (cmulsv.getShape() != cvint.getShape()) return -140;
    if (cmulsv(0) != cint * cvint(0)) return -141;
    if (cmulsv(1) != cint * cvint(1)) return -142;
    if (cmulsv(2) != cint * cvint(2)) return -143;

    Matrix cmulvs( cvint * cint  );
    if (cmulvs.getShape() != cvint.getShape()) return -145;
    if (cmulvs(0) != cvint(0) * cint) return -146;
    if (cmulvs(1) != cvint(1) * cint) return -147;
    if (cmulvs(2) != cvint(2) * cint) return -148;

    // Vektor*Vektor
    Matrix cmulvv( cvint.transpose() * cvint );
    if (cmulvv.getShape() != Shape() ) return -150;
    if (cmulvv(0)->simplify() != 1+4+9) return -151;

#pragma endregion
    } catch (IndexError) { return -1005; };

    try {
#pragma region Matrix/Skalar
    // Skalar +- Matrix
    Matrix cplussm( cint + cmint );
    if (cplussm.getShape() != cmint.getShape()) return -160;
    if (cplussm(0,0) != cmint(0,0) + cint) return -161;
    if (cplussm(0,1) != cmint(0,1) + cint) return -162;
    if (cplussm(0,2) != cmint(0,2) + cint) return -163;
    if (cplussm(1,0) != cmint(1,0) + cint) return -164;
    if (cplussm(1,1) != cmint(1,1) + cint) return -165;
    if (cplussm(1,2) != cmint(1,2) + cint) return -166;

    Matrix cminussm( cint - cmint );
    if (cminussm.getShape() != cmint.getShape()) return -167;
    if (cminussm(0,0) != cint - cmint(0,0)) return -168;
    if (cminussm(0,1) != cint - cmint(0,1)) return -169;
    if (cminussm(0,2) != cint - cmint(0,2)) return -170;
    if (cminussm(1,0) != cint - cmint(1,0)) return -171;
    if (cminussm(1,1) != cint - cmint(1,1)) return -172;
    if (cminussm(1,2) != cint - cmint(1,2)) return -173;

    // Matrix +- Skalar
    Matrix cplusms( cmint + cint );
    if (cplusms.getShape() != cmint.getShape()) return -180;
    if (cplusms(0,0) != cmint(0,0) + cint) return -181;
    if (cplusms(0,1) != cmint(0,1) + cint) return -182;
    if (cplusms(0,2) != cmint(0,2) + cint) return -183;
    if (cplusms(1,0) != cmint(1,0) + cint) return -184;
    if (cplusms(1,1) != cmint(1,1) + cint) return -185;
    if (cplusms(1,2) != cmint(1,2) + cint) return -186;

    Matrix cminusms( cmint - cint );
    if (cminusms.getShape() != cmint.getShape()) return -187;
    if (cminusms(0,0) != cmint(0,0) - cint) return -188;
    if (cminusms(0,1) != cmint(0,1) - cint) return -189;
    if (cminusms(0,2) != cmint(0,2) - cint) return -190;
    if (cminusms(1,0) != cmint(1,0) - cint) return -191;
    if (cminusms(1,1) != cmint(1,1) - cint) return -192;
    if (cminusms(1,2) != cmint(1,2) - cint) return -193;

    // Matrix * Skalar, Skalar * Matrix
    Matrix cmulsm( cint * cmint );
    if (cmulsm.getShape() != cmint.getShape()) return -200;
    if (cmulsm(0,0) != cint * cmint(0,0) ) return -201;
    if (cmulsm(0,1) != cint * cmint(0,1) ) return -202;
    if (cmulsm(0,2) != cint * cmint(0,2) ) return -203;
    if (cmulsm(1,0) != cint * cmint(1,0) ) return -204;
    if (cmulsm(1,1) != cint * cmint(1,1) ) return -205;
    if (cmulsm(1,2) != cint * cmint(1,2) ) return -206;

    Matrix cmulms( cmint * cint );
    if (cmulms.getShape() != cmint.getShape()) return -207;
    if (cmulms(0,0) != cmint(0,0) * cint) return -208;
    if (cmulms(0,1) != cmint(0,1) * cint) return -209;
    if (cmulms(0,2) != cmint(0,2) * cint) return -210;
    if (cmulms(1,0) != cmint(1,0) * cint) return -211;
    if (cmulms(1,1) != cmint(1,1) * cint) return -212;
    if (cmulms(1,2) != cmint(1,2) * cint) return -213;
#pragma endregion
    } catch (IndexError) { return -1006; };

    try {
#pragma region Matrix/Vektor
    // Matrix * Vektor, Vektor * Matrix
    Matrix cmulmv( cmint * cvint );
    if (cmulmv.getShape() != Shape(1,2,1)) return -220;
    if (cmulmv(0) != cmint(0,0)*cvint(0) + cmint(0,1)*cvint(1) + cmint(0,2)*cvint(2) ) return -221;
    if (cmulmv(1) != cmint(1,0)*cvint(0) + cmint(1,1)*cvint(1) + cmint(1,2)*cvint(2) ) return -222;
    try {
            cmint.transpose()*cvint;
            return -223;
    } catch (ShapeError) {}
    catch (...) { return -224; };

    Matrix cmulvm( cvint.transpose() * cmint.transpose() );
    if (cmulvm.getShape() != Shape(1,1,2)) return -230;
    if (cmulvm(0) != cmint(0,0)*cvint(0) + cmint(0,1)*cvint(1) + cmint(0,2)*cvint(2) ) return -231;
    if (cmulvm(1) != cmint(1,0)*cvint(0) + cmint(1,1)*cvint(1) + cmint(1,2)*cvint(2) ) return -232;
    try {
            cvint * cmint;
            return -233;
    } catch (ShapeError) {}
    catch (...) { return -234; };

#pragma endregion
    } catch (IndexError) { return -1007; };

    try {
#pragma region Matrix/Matrix
    // Matrix*Matrix
    Matrix cmulmm( cmint * cmint.transpose() );
    if (cmulmm.getShape() != Shape(2,2,2)) return -240;
    if (cmulmm(0,0) != cmint(0,0)*cmint(0,0) + cmint(0,1)*cmint(0,1) + cmint(0,2)*cmint(0,2) ) return -241;
    if (cmulmm(0,1) != cmint(0,0)*cmint(1,0) + cmint(0,1)*cmint(1,1) + cmint(0,2)*cmint(1,2) ) return -242;
    if (cmulmm(1,0) != cmint(1,0)*cmint(0,0) + cmint(1,1)*cmint(0,1) + cmint(1,2)*cmint(0,2) ) return -243;
    if (cmulmm(1,1) != cmint(1,0)*cmint(1,0) + cmint(1,1)*cmint(1,1) + cmint(1,2)*cmint(1,2) ) return -244;
    try {
            cmint*cmint;
            return -245;
    } catch (ShapeError) {}
    catch (...) { return -246; };
#pragma endregion
    } catch (IndexError) { return -1008; };

#pragma region Vergleich

    // Scalar == Scalar, Scalar < Scalar 
    Int cint1(42);
    Int cint2(43);
    if (cint != cint) return -1100;
    if (cint != cint1) return -1101;
    if (cint < cint1) return -1102;
    if (cint2 < cint1) return -1103;

    Real cdbl1(1.234 );
    Real cdbl2(1.235 );
    if (cdbl != cdbl) return -1104;
    if (cdbl != cdbl1) return -1105;
    if (cdbl < cdbl1) return -1106;
    if (cdbl2 < cdbl1) return -1107;

    // Vector == Vector, Vector < Vector 
    Matrix cvint1( Shape(3) );
    cvint1 = 1,2,3;
    Matrix cvint2( Shape(2) );
    cvint2 = 1,2;
    Matrix cvint3( Shape(3) );
    cvint3 = 1,2,4;
    if (cvint != cvint) return -1108;
    if (cvint != cvint1) return -1109;
    if (cvint < cvint1) return -1110;
    if (cvint3 < cvint) return -1111;
    if (cvint1 < cvint2) return -1112;

    Matrix cvdbl1( Shape(4) );
    cvdbl1 = 1.1,2.2,3.3,4.4;
    Matrix cvdbl2( Shape(3) );
    cvdbl2 = 1.1,2.2,3.3;
    Matrix cvdbl3( Shape(4) );
    cvdbl3 = 1.1,2.2,3.4,4.4;
    if (cvdbl != cvdbl) return -1113;
    if (cvdbl != cvdbl1) return -1114;
    if (cvdbl < cvdbl1) return -1115;
    if (cvdbl3 < cvdbl) return -1116;
    if (cvdbl1 < cvdbl2) return -1117;
    if (!(cvdbl2 < cvdbl1)) return -1118;

    // Vector == Scalar, Vector < Scalar
    if (cvdbl == cdbl) return -1120;
    if (cvdbl < cdbl) return -1121;
    if (cvint == cint) return -1122;
    if (cvint < cint) return -1123;

#pragma endregion

#pragma region TypeCast

    Real cintdbl(cint);
    Real dbl42(42.0);
    if (cintdbl != dbl42) return -1200;

    Matrix cvintdbl(cvint);
    Matrix cvdbl4( Shape(3));
    cvdbl4 = 1.0,2.0,3.0;
    if (cvintdbl != cvdbl4) return -1201;

    Matrix cmintdbl(cmint);
    Matrix cmdbl4( Shape(2,3));
    cmdbl4 = 1.0,2.0,3.0,
             4.0,5.0,6.0;
    if (cmintdbl != cmdbl4) return -1202;

#pragma endregion

#pragma region Derivative

    BasicPtr dercint = cint.der();
    if (dercint != Int::getZero()) return -1300;
    BasicPtr dercdbl = cdbl.der();
    if (dercdbl != Int::getZero()) return -1301;

    BasicPtr a(new Symbol("a"));
    BasicPtr dercinta = cint.der(a);
    if (dercinta != Int::getZero()) return -1302;
    BasicPtr dercdbla = cdbl.der(a);
    if (dercdbla != Int::getZero()) return -1303;



#pragma endregion

#pragma region Bool

    // Bool(true) = true
    Bool cbool_true(true);
    if (cbool_true.getValue() != true) return -2001;

    // Bool(false) = false
    Bool cbool_false(false);
    if (cbool_false.getValue() != false) return -2002;
    // true == false == false
    if (cbool_true == cbool_false)  return -2003;
    // true != false == true
    if (!(cbool_true != cbool_false))  return -2004;
    // true < false = false
    if (cbool_true < cbool_false)  return -2005;
    // true < true = false
    if (cbool_true < cbool_true)  return -2006;
    // ArgSize() = 0
    if (cbool_true.getArgsSize() != 0) return -2007;
    // der(true) = InternalError
    try
    {
      BasicPtr derb = cbool_true.der();
    }
    catch(InternalError){}
    catch (...) { return -2008; };

    Real crealtwo(2.0);
    Real crealone(1.0);
    Real crealzero(0.0);
    Real crealmtwo(-2.0);
    Int cinttwo(2);
    Int cintone(1);
    Int cintzero(0);
    Int cintmtwo(-2);

    // true < 2.0 = false
    if (cbool_true < crealtwo) return -2009;
    // true < 0.0 = false
    if (cbool_true < crealzero) return -2010;
    // true < -2.0 = false
    if (cbool_true < crealmtwo) return -2011;

    // true < 2 = false
    if (cbool_true < crealtwo) return -2012;
    // true < 0 = false
    if (cbool_true < crealzero) return -2013;
    // true < -2 = false
    if (cbool_true < crealmtwo) return -2015;

    // true == true = true
    if (!(cbool_true == true)) return -2016;
    // false == false = true
    if (!(cbool_false == false)) return -2017;
    // false == true = false
    if (cbool_false == true) return -2018;
    // true == false = false
    if (cbool_true == false) return -2019;
    // true != true = false
    if (cbool_true != true) return -2020;
    // false != false = false
    if (cbool_false != false) return -2021;
    // true != false = true
    if (!(cbool_true != false)) return -2022;
    // false != true = false
    if (!(cbool_false != true)) return -2023;

    // true == 1.0 = true
    if (!(cbool_true == crealone)) return -2024;
    // false == 0.0 = true
    if (!(cbool_false == crealzero)) return -2025;
    // false == 1.0 = false
    if (cbool_false == crealone) return -2026;
    // true == 0.0 = false
    if (cbool_true == crealzero) return -2027;
    // true != 1.0 = false
    if (cbool_true != crealone) return -2028;
    // false != 0.0 = false
    if (cbool_false != crealzero) return -2029;
    // true != 0.0 = true
    if (!(cbool_true != crealzero)) return -2030;
    // false != 1.0 = false
    if (!(cbool_false != crealone)) return -2031;

    // true == 1 = true
    if (!(cbool_true == cintone)) return -2032;
    // false == 0 = true
    if (!(cbool_false == cintzero)) return -2033;
    // false == 1 = false
    if (cbool_false == cintone) return -2034;
    // true == 0 = false
    if (cbool_true == cintzero) return -2035;
    // true != 1 = false
    if (cbool_true != cintone) return -2036;
    // false != 0 = false
    if (cbool_false != cintzero) return -2037;
    // true != 0 = true
    if (!(cbool_true != cintzero)) return -2038;
    // false != 1 = false
    if (!(cbool_false != cintone)) return -2039;

    if (cbool_true.getShape() != Shape(0,1,1)) return -2040;
    if (cbool_true.getType() != Type_Bool) return -2041;

    Basic::BasicSet atoms;
    cbool_true.getAtoms(atoms);
    if (atoms.size() != 0) return -2042;

#pragma endregion

    return 0;
}