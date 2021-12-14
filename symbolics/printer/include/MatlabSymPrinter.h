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

#ifndef __MATLAB_SYM_PRINTER_H_
#define __MATLAB_SYM_PRINTER_H_

#include "MatlabPrinter.h"

namespace Symbolics
{
    class MatlabSymPrinter: public MatlabPrinter
    {
    public:
        // Konstruktor
        MatlabSymPrinter();
        // Destruktor
        ~MatlabSymPrinter();

        // Bei der Deklaration von symbolischen Variablen wird dimension gebraucht
        std::string dimension( BasicPtr const& basic );

    protected:
        // Funktionen die sich vom Standard unterscheiden, ueberschreiben
        std::string print_Atan2( const Atan2 *s );
        std::string print_Sign( const Sign *s );

    };
};

#endif // __MATLAB_SYM_PRINTER_H_
