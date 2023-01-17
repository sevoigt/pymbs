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
    SymmetricMatrix c( s );
    SymmetricMatrix uc( s );
    c.set(0,as);
    uc.set(0,as);
    Matrix v( Shape(3) );
    v = as,bs,cs;
    SymmetricMatrix m( Shape(3,3) );

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
    try {
      SymmetricMatrix v( Shape(3) );
        return -5;
    } catch (InternalError) {}
    catch (...) { return - 17; };

    try {
      SymmetricMatrix v( Shape(2,3) );
        return -6;
    } catch (InternalError) {}
    catch (...) { return - 18; };

    try {
      BasicPtrVec myvec;
      myvec.push_back(as);
      myvec.push_back(bs);
      myvec.push_back(cs);
      myvec.push_back(ds);
      myvec.push_back(es);

      SymmetricMatrix v(myvec,Shape(3,3) );
        return -7;
    } catch (ShapeError) {}
    catch (...) { return - 19; };

#pragma endregion
    } catch (IndexError) { return -1002; };

    try {
#pragma region Matrix
    // int-Matrix anlegen
    m = as,bs,cs,
           ds,es,
              fs;
    if (m.getShape() != Shape(2,3,3)) return -32;
    if (m.getType() != Type_Matrix) return -33;
    if (m(0,0) != as) return -34;
    if (m(0,1) != bs) return -35;
    if (m(0,2) != cs) return -36;
    if (m(1,0) != m(0,1)) return -37;
    if (m(1,1) != ds) return -38;
    if (m(1,2) != es) return -39;
    if (m(2,0) != m(0,2)) return -40;
    if (m(2,1) != m(1,2)) return -41;
    if (m(2,2) != fs) return -42;
    try {
        m(-1,-1);
        return -43;
    } catch (IndexError) {}
    catch (...) { return -44; };
    try {
        m(-1,0);
        return -45;
    } catch (IndexError) {}
    catch (...) { return -46; };
    try {
        m(0,-1);
        return -47;
    } catch (IndexError) {}
    catch (...) { return -48; };
    try {
        m(3,2);
        return -49;
    } catch (IndexError) {}
    catch (...) { return -50; };
    try {
        m(1,3);
        return -51;
    } catch (IndexError) {}
    catch (...) { return -52; };
    try {
        m(2,3);
        return -53;
    } catch (IndexError) {}
    catch (...) { return -54; };

#pragma endregion
    } catch (IndexError) { return -1003; };

    try    {
#pragma region Rechnen mit Konstanten (Skalar)
    // Skalar
    SymmetricMatrix cplus( c + c );
    if (!cplus.is_Scalar()) return -100;
    if (cplus(0) != c(0) + c(0)) return -101;
    SymmetricMatrix cminus( c - c );
    if (!cminus.is_Scalar()) return -102;
    if (cminus(0) != c(0) - c(0)) return -103;

    Matrix cplus1( c + uc );
    if (!cplus.is_Scalar()) return -104;
    if (cplus(0) != c(0) + c(0)) return -105;

    Matrix cplus2( uc + c );
    if (!cplus.is_Scalar()) return -106;
    if (cplus(0) != c(0) + c(0)) return -107;

    Matrix cmul( c * c );
    if (!cmul.is_Scalar()) return -108;
    if (cmul(0) != c(0)*c(0)) return -109;

    SymmetricMatrix cmulscal( c * Int(2) );
    if (!cmulscal.is_Scalar()) return -110;
    if (cmulscal(0) != c(0) * Int(2)) return -111;

#pragma endregion
    } catch (IndexError) { return -1004; };


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
    if (cmulmv.getShape() != Shape(1,3,1)) return -220;
    if (cmulmv(0) != (((m(0,0)*v(0)) + m(0,1)*v(1)) + m(0,2)*v(2)) ) return -221;
    if (cmulmv(1) != (((m(1,0)*v(0)) + m(1,1)*v(1)) + m(1,2)*v(2)) ) return -222;

    Matrix cmulvm( v.transpose() * m.transpose() );
    if (cmulvm.getShape() != Shape(1,1,3)) return -230;
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
    if (cmulmm.getShape() != Shape(2,3,3)) return -240;
    if (cmulmm(0,0) != (((m(0,0)*m(0,0)) + m(0,1)*m(0,1)) + m(0,2)*m(0,2)) ) return -241;
    if (cmulmm(0,1) != (((m(0,0)*m(1,0)) + m(0,1)*m(1,1)) + m(0,2)*m(1,2)) ) return -242;
    if (cmulmm(1,0) != (((m(1,0)*m(0,0)) + m(1,1)*m(0,1)) + m(1,2)*m(0,2)) ) return -243;
    if (cmulmm(1,1) != (((m(1,0)*m(1,0)) + m(1,1)*m(1,1)) + m(1,2)*m(1,2)) ) return -244;

    if( !((m*m).transpose() == m.transpose()*m.transpose()))
        return - 245;
    }
    catch (...) { return -246; };

    return 0;
}