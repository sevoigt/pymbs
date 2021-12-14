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

using namespace Symbolics;

int main( int argc,  char *argv[])
{

    BasicPtr as(new Symbol("a"));
    BasicPtr bs(new Symbol("b"));
    BasicPtr cs(new Symbol("c"));
    BasicPtr ds(new Symbol("d"));
    BasicPtr es(new Symbol("e"));
    BasicPtr fs(new Symbol("f"));
    BasicPtr gs(new Symbol("g"));
 
    Shape s;
    Matrix c( s );
    c.set(0,as);
    Matrix v( Shape(3) );
    Matrix m( Shape(2,3) );

    try    {
#pragma region Skalar
    // Skalar anlegen
    if (c.getShape() != Shape(0,1,1)) return -2;
    if (c.getType() != Type_Matrix) return -3;
    if (c(0) != as) return -4;

#pragma endregion
    } catch (IndexError) { return -1001; };

    try    {
#pragma region Vektor
    // int-Vektor anlegen
    v = as,bs,cs;
    if (v.getShape() != Shape(1,3,1)) return -10;
    if (v.getType() != Type_Matrix) return -11;
    if (v(0) != as) return -12;
    if (v(1) != bs) return -13;
    if (v(2) != cs) return -14;
    try {
        v(-1);
        return -16;
    } catch (IndexError) {}
    catch (...) { return - 17; };
    try {
        v(3);
        return -18;
    } catch (IndexError) {}
    catch (...) { return - 19; };

#pragma endregion
    } catch (IndexError) { return -1002; };

    try {
#pragma region Matrix
    // int-Matrix anlegen
    m = as,bs,cs,
        ds,es,fs;
    if (m.getShape() != Shape(2,2,3)) return -32;
    if (m.getType() != Type_Matrix) return -33;
    if (m(0,0) != as) return -34;
    if (m(0,1) != bs) return -35;
    if (m(0,2) != cs) return -36;
    if (m(1,0) != ds) return -37;
    if (m(1,1) != es) return -38;
    if (m(1,2) != fs) return -39;
    try {
        m(-1,-1);
        return -40;
    } catch (IndexError) {}
    catch (...) { return -41; };
    try {
        m(-1,0);
        return -42;
    } catch (IndexError) {}
    catch (...) { return -43; };
    try {
        m(0,-1);
        return -44;
    } catch (IndexError) {}
    catch (...) { return -45; };
    try {
        m(2,2);
        return -46;
    } catch (IndexError) {}
    catch (...) { return -47; };
    try {
        m(1,3);
        return -48;
    } catch (IndexError) {}
    catch (...) { return -49; };
    try {
        m(2,3);
        return -50;
    } catch (IndexError) {}
    catch (...) { return -51; };

#pragma endregion
    } catch (IndexError) { return -1003; };

    try    {
#pragma region Rechnen mit Konstanten (Skalar)
    // Skalar
    Matrix cplus( c + c );
    if (!cplus.is_Scalar()) return -100;
    if (cplus(0) != c(0) + c(0)) return -101;
    Matrix cminus( c - c );
    if (!cminus.is_Scalar()) return -102;
    if (cminus(0) != c(0) - c(0)) return -103;

    Matrix cmul( c * c );
    if (!cmul.is_Scalar()) return -108;
    if (cmul(0) != c(0)*c(0)) return -109;

#pragma endregion
    } catch (IndexError) { return -1004; };

    try {
#pragma region Vektor/Skalar
    // Vektor +- Skalar
    Matrix cplusvs( v + c );
    if (cplusvs.getShape() != v.getShape()) return -110;
    if (cplusvs(0) != v(0) + c(0)) return -111;
    if (cplusvs(1) != v(1) + c(0)) return -112;
    if (cplusvs(2) != v(2) + c(0)) return -113;

    Matrix cminusvs( v - c );
    if (cminusvs.getShape() != v.getShape()) return -115;
    if (cminusvs(0) != v(0) - c(0)) return -116;
    if (cminusvs(1) != v(1) - c(0)) return -117;
    if (cminusvs(2) != v(2) - c(0)) return -118;


    // Skalar +- Vektor
    Matrix cplussv( c + v );
    if (cplussv.getShape() != v.getShape()) return -120;
    if (cplussv(0) != v(0) + c(0)) return -121;
    if (cplussv(1) != v(1) + c(0)) return -122;
    if (cplussv(2) != v(2) + c(0)) return -123;

    Matrix cminussv( c - v );
    if (cminussv.getShape() != v.getShape()) return -125;
    if (cminussv(0) != c(0) - v(0)) return -126;
    if (cminussv(1) != c(0) - v(1)) return -127;
    if (cminussv(2) != c(0) - v(2)) return -128;

    // Vektor +- Vektor
    Matrix cplusvv( v + v );
    if (cplusvv.getShape() != v.getShape()) return -130;
    if (cplusvv(0) != v(0) + v(0)) return -131;
    if (cplusvv(1) != v(1) + v(1)) return -132;
    if (cplusvv(2) != v(2) + v(2)) return -133;

    Matrix cminusvv( v - v );
    if (cminusvv.getShape() != v.getShape()) return -135;
    if (cminusvv(0) != v(0) - v(0)) return -136;
    if (cminusvv(1) != v(1) - v(1)) return -137;
    if (cminusvv(2) != v(2) - v(2)) return -138;

    // Skalar * Vektor, Vektor*Skalar
    Matrix cmulsv( c * v );
    if (cmulsv.getShape() != v.getShape()) return -140;
    std::cout << cmulsv(1)->toString() << std::endl;
    std::cout << (v(1) * c(0))->toString() << std::endl;
    if (cmulsv(0) != v(0) * c(0)) return -141;
    if (cmulsv(1) != v(1) * c(0)) return -142;
    if (cmulsv(2) != v(2) * c(0)) return -143;

    Matrix cmulvs( v * c  );
    if (cmulvs.getShape() != v.getShape()) return -145;
    if (cmulvs(0) != v(0) * c(0)) return -146;
    if (cmulvs(1) != v(1) * c(0)) return -147;
    if (cmulvs(2) != v(2) * c(0)) return -148;

    // Vektor*Vektor
    Matrix cmulvv( v.transpose() * v );
    if (cmulvv.getShape() != Shape() ) return -150;
    if (cmulvv(0) != as*as + bs*bs + cs*cs) return -151;

#pragma endregion
    } catch (IndexError) { return -1005; };

    try {
#pragma region Matrix/Skalar
    // Skalar +- Matrix
    Matrix cplussm( c + m );
    if (cplussm.getShape() != m.getShape()) return -160;
    if (cplussm(0,0) != m(0,0) + c(0)) return -161;
    if (cplussm(0,1) != m(0,1) + c(0)) return -162;
    if (cplussm(0,2) != m(0,2) + c(0)) return -163;
    if (cplussm(1,0) != m(1,0) + c(0)) return -164;
    if (cplussm(1,1) != m(1,1) + c(0)) return -165;
    if (cplussm(1,2) != m(1,2) + c(0)) return -166;

    Matrix cminussm( c - m );
    if (cminussm.getShape() != m.getShape()) return -167;
    if (cminussm(0,0) != c(0) - m(0,0)) return -168;
    if (cminussm(0,1) != c(0) - m(0,1)) return -169;
    if (cminussm(0,2) != c(0) - m(0,2)) return -170;
    if (cminussm(1,0) != c(0) - m(1,0)) return -171;
    if (cminussm(1,1) != c(0) - m(1,1)) return -172;
    if (cminussm(1,2) != c(0) - m(1,2)) return -173;

    // Matrix +- Skalar
    Matrix cplusms( m + c );
    if (cplusms.getShape() != m.getShape()) return -180;
    if (cplusms(0,0) != m(0,0) + c(0)) return -181;
    if (cplusms(0,1) != m(0,1) + c(0)) return -182;
    if (cplusms(0,2) != m(0,2) + c(0)) return -183;
    if (cplusms(1,0) != m(1,0) + c(0)) return -184;
    if (cplusms(1,1) != m(1,1) + c(0)) return -185;
    if (cplusms(1,2) != m(1,2) + c(0)) return -186;

    Matrix cminusms( m - c );
    if (cminusms.getShape() != m.getShape()) return -187;
    if (cminusms(0,0) != m(0,0) - c(0)) return -188;
    if (cminusms(0,1) != m(0,1) - c(0)) return -189;
    if (cminusms(0,2) != m(0,2) - c(0)) return -190;
    if (cminusms(1,0) != m(1,0) - c(0)) return -191;
    if (cminusms(1,1) != m(1,1) - c(0)) return -192;
    if (cminusms(1,2) != m(1,2) - c(0)) return -193;

    // Matrix * Skalar, Skalar * Matrix
    Matrix cmulsm( c * m );
    if (cmulsm.getShape() != m.getShape()) return -200;
    if (cmulsm(0,0) != m(0,0) * c(0)) return -201;
    if (cmulsm(0,1) != m(0,1) * c(0)) return -202;
    if (cmulsm(0,2) != m(0,2) * c(0)) return -203;
    if (cmulsm(1,0) != m(1,0) * c(0)) return -204;
    if (cmulsm(1,1) != m(1,1) * c(0)) return -205;
    if (cmulsm(1,2) != m(1,2) * c(0)) return -206;

    Matrix cmulms( m * c );
    if (cmulms.getShape() != m.getShape()) return -207;
    if (cmulms(0,0) != m(0,0) * c(0)) return -208;
    if (cmulms(0,1) != m(0,1) * c(0)) return -209;
    if (cmulms(0,2) != m(0,2) * c(0)) return -210;
    if (cmulms(1,0) != m(1,0) * c(0)) return -211;
    if (cmulms(1,1) != m(1,1) * c(0)) return -212;
    if (cmulms(1,2) != m(1,2) * c(0)) return -213;
#pragma endregion
    } catch (IndexError) { return -1006; };

    try {
#pragma region Matrix/Vektor
    // Matrix * Vektor, Vektor * Matrix
    Matrix cmulmv( m * v );
    if (cmulmv.getShape() != Shape(1,2,1)) return -220;
    if (cmulmv(0) != (((m(0,0)*v(0)) + m(0,1)*v(1)) + m(0,2)*v(2)) ) return -221;
    if (cmulmv(1) != (((m(1,0)*v(0)) + m(1,1)*v(1)) + m(1,2)*v(2)) ) return -222;
    try {
            m.transpose()*v;
            return -223;
    } catch (ShapeError) {}
    catch (...) { return -224; };

    Matrix cmulvm( v.transpose() * m.transpose() );
    if (cmulvm.getShape() != Shape(1,1,2)) return -230;
    if (cmulvm(0) != (((m(0,0)*v(0)) + m(0,1)*v(1)) + m(0,2)*v(2)) ) return -231;
    if (cmulvm(1) != (((v(0)*m(1,0)) + v(1)*m(1,1)) + v(2)*m(1,2)) ) return -232;
    try {
            v * m;
            return -233;
    } catch (ShapeError) {}
    catch (...) { return -234; };

#pragma endregion
    } catch (IndexError) { return -1007; };

    try {
#pragma region Matrix/Matrix
    // Matrix*Matrix
    Matrix cmulmm( m * m.transpose() );
    if (cmulmm.getShape() != Shape(2,2,2)) return -240;
    if (cmulmm(0,0) != (((m(0,0)*m(0,0)) + m(0,1)*m(0,1)) + m(0,2)*m(0,2)) ) return -241;
    if (cmulmm(0,1) != (((m(0,0)*m(1,0)) + m(0,1)*m(1,1)) + m(0,2)*m(1,2)) ) return -242;
    if (cmulmm(1,0) != (((m(1,0)*m(0,0)) + m(1,1)*m(0,1)) + m(1,2)*m(0,2)) ) return -243;
    if (cmulmm(1,1) != (((m(1,0)*m(1,0)) + m(1,1)*m(1,1)) + m(1,2)*m(1,2)) ) return -244;
    try {
            m*m;
            return -245;
    } catch (ShapeError) {}
    catch (...) { return -246; };
#pragma endregion
    } catch (IndexError) { return -1008; };

#pragma region Vergleich

    // Scalar == Scalar, Scalar < Scalar 
    BasicPtrVec vecas;
    vecas.push_back(as);
    Matrix c1(vecas,Shape());
    BasicPtrVec vecbs;
    vecbs.push_back(bs);
    Matrix c2(vecbs,Shape());
    if (c != c) return -1100;
    if (c != c1) return -1101;
    if (c < c1) return -1102;
    if (c2 < c1) return -1103;

    // Vector == Vector, Vector < Vector 
    Matrix cv1( Shape(3) );
    cv1 = as,bs,cs;
    Matrix cv2( Shape(2) );
    cv2 = as,bs;
    Matrix cv3( Shape(3) );
    cv3 = as,bs,ds;
    if (v != v) return -1108;
    if (v != cv1) return -1109;
    if (v < cv1) return -1110;
    if (cv3 < v) return -1111;
    if (cv1 < cv2) return -1112;

   // Vector == Scalar, Vector < Scalar
    if (v == c) return -1122;
    if (v < c) return -1123;

#pragma endregion

#pragma region TypeCast

    Int *cintp = new Int(1);
    BasicPtr cint(cintp);
    Real *cdblp = new Real(1);
    BasicPtr cdbl(cdblp);
    BasicPtr cint2 = new Int(2);
    BasicPtr cdbl2 = new Real(2.0);
    BasicPtr cint3 = new Int(3);
    BasicPtr cdbl3 = new Real(3.0);
    BasicPtr cint4 = new Int(4);
    BasicPtr cdbl4 = new Real(4.0);
    BasicPtr cint5 = new Int(5);
    BasicPtr cdbl5 = new Real(5.0);
    BasicPtr cint6 = new Int(6);
    BasicPtr cdbl6 = new Real(6.0);
    Matrix cvint( Shape(3) );
    cvint = 1,2,3;
    Matrix cvdbl( Shape(3) );
    cvdbl = 1.0,2.0,3.0;
    Matrix cmint( Shape(2,3) );
    cmint = 1,2,3,
            4,5,6;
    Matrix cmdbl( Shape(2,3) );
    cmdbl = 1.0,2.0,3.0,
            4.0,5.0,6.0;

    BasicPtrVec cintvec;
    cintvec.push_back(cint);
    Matrix cintmatrix(cintvec, Shape());
    Matrix mint(cintvec, Shape());
    if (cintmatrix != mint) return -1200;

    BasicPtrVec cdblvec;
    cdblvec.push_back(cdbl);
    Matrix cdblmatrix(cdblvec, Shape());
    Matrix mdbl(cdblvec, Shape());
    if (cdblmatrix != mdbl) return -1201;

    Matrix cvintmatrix(cvint);
    Matrix cvintmat( Shape(3));
    cvintmat = cint,cint2,cint3;
    if (cvintmatrix != cvintmat) return -1203;

    Matrix cvdblmatrix(cvdbl);
    Matrix cvdblmat( Shape(3));
    cvdblmat = cdbl,cdbl2,cdbl3;
    if (cvdblmatrix != cvdblmat) return -1204;

    Matrix cmintmatrix(cmint);
    Matrix cmintmat( Shape(2,3));
    cmintmat = cint,cint2,cint3,
               cint4,cint5,cint6;
    if (cmintmatrix != cmintmat) return -1205;

    Matrix cmdblmatrix(cmdbl);
    Matrix cmdblmat( Shape(2,3));
    cmdblmat = cdbl,cdbl2,cdbl3,
               cdbl4,cdbl5,cdbl6;
    if (cmdblmatrix != cmdblmat) return -1206;

#pragma endregion


#pragma region Derivative

    BasicPtr dercmdblmatrix = cmdblmatrix.der();
    if (!Util::is_Zero(dercmdblmatrix)) return -1300;

#pragma endregion

    return 0;
}