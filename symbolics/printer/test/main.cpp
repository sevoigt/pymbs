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
#include "Printer.h"

using namespace Symbolics;

//Statt für alle PrinterTest cpp Dateien zusätzliche Headerdateien zu schreiben hier einfach versprechen,
//dass die benötigten Funktionen existieren (Ja das ist unsauber...)
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