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

#ifndef __FMU_PRINTER_H_
#define __FMU_PRINTER_H_

#include "CPrinter.h"
#include <map>

namespace Symbolics
{
    class FMUPrinter: public Symbolics::CPrinter
    {
    public:
        // Konstruktor
        FMUPrinter(const std::map<std::string, int> *valueReferences);
        // Destruktor
        ~FMUPrinter();

    protected:
		const std::map<std::string, int> *m_valueReferencesP;

		// Funktionen die sich vom CPrinter unterscheiden, ueberschreiben
        std::string print_Symbol( const Symbol *symbol );
		std::string print_Element( const Element *e );
		std::string print_Der( const Der *d );
        std::string print_Bool( const Bool *b );

    };
};

#endif // __FMU_PRINTER_H_
