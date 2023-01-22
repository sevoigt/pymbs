#include <iostream>
#include "Symbolics.h"
#include "Printer.h"

using namespace Symbolics;

//Statt f�r alle PrinterTest cpp Dateien zus�tzliche Headerdateien zu schreiben hier einfach versprechen,
//dass die ben�tigten Funktionen existieren (Ja das ist unsauber...)
int cMain();
int fmuMain();
int fortranMain();
int matMain();
int msMain();
int moMain();
int pyMain();

int main( int argc,  char *argv[])
{
    int res=0;

    res += cMain();
    std::cout << std::endl;
    res += fmuMain();
    std::cout << std::endl;
    res += fortranMain();
    std::cout << std::endl;
    res += matMain();
    std::cout << std::endl;
    res += msMain();
    std::cout << std::endl;
    res += moMain();
    std::cout << std::endl;
    res += pyMain();
    std::cout << std::endl;

    return res;
}