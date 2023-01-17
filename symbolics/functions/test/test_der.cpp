#include <iostream>
#include "test_der.h"

int test_der( int &argc,  char *argv[])
{
    // Symbol und Konstanten anlegen
    BasicPtr a( new Symbol("a") );
    BasicPtr dera(new Der(a));

    if (dera->getShape() != a->getShape() ) return -1;


    // der(der(a))
    try
    {
        BasicPtr derdera = dera->der();
    }
    catch (InternalError) { }
    catch (...) { return -3; };

    // TODO

    return 0;
}