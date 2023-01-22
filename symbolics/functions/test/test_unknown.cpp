#include <iostream>
#include "test_unknown.h"

int test_unknown( int &argc,  char *argv[])
{
    // Symbole und Konstanten anlegen
    BasicPtr a( new Symbol("a") );
    BasicPtr b( new Symbol("b") );
    BasicPtr c( new Symbol("c", Shape(3)) );

    // Verschiedene Vektoren
    BasicPtrVec oneArg;
    oneArg.push_back(a);
    BasicPtrVec twoArg(oneArg);
    twoArg.push_back(b);
    BasicPtrVec threeArg(twoArg);
    threeArg.push_back(c);

    // Einfach mal eine Funktion anlegen
    BasicPtr u1( new Unknown("U1", oneArg) );
    if (u1->getType() != Type_Unknown) return -1;
    if (u1->getShape() != Shape()) return -2;
    if (u1->getArg(0) != oneArg[0]) return -3;

    BasicPtr u2( new Unknown("U2", twoArg) );
    if (u2->getType() != Type_Unknown) return -10;
    if (u2->getShape() != Shape()) return -11;
    if (u2->getArg(0) != twoArg[0]) return -12;

    BasicPtr u3( new Unknown("U3", threeArg) );
    if (u3->getType() != Type_Unknown) return -20;
    if (u3->getShape() != Shape(3)) return -21;
    if (u3->getArg(0) != threeArg[0]) return -22;

    return 0;
}