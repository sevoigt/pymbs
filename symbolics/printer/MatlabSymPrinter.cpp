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

#include "MatlabSymPrinter.h"
#include "str.h"
#include <iostream>

using namespace Symbolics;

/*****************************************************************************/
MatlabSymPrinter::MatlabSymPrinter()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
MatlabSymPrinter::~MatlabSymPrinter()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
std::string MatlabSymPrinter::dimension( const BasicPtr &b )
/*****************************************************************************/
{
    if (b->is_Scalar())
        return "";
    return ",[" + str(b->getShape().getDimension(1)) + " " + str(b->getShape().getDimension(2)) + "]";
}
/*****************************************************************************/

/*****************************************************************************/
std::string MatlabSymPrinter::print_Atan2( const Atan2 *s )
/*****************************************************************************/
{
	if (s == NULL) throw InternalError("Printer: Atan2 is NULL");
    std::cerr << "Warning: MatlabSymPrinter: Atan2 replaced by approximation, since Atan2 function cannot be used with Matlabs Symbolic Math Toolbox." << std::endl;
	return "(2*atan(" + print(s->getArg1()) + "/((" + print(s->getArg2()) + "^2 + " + print(s->getArg1()) + "^2)^0.5 + " + print(s->getArg2()) + ")))";
    //Alternativ könnte man auch einfach atan2 durch atan ersetzen:
    //return "atan(" + print(s->getArg1()) + "/" + print(s->getArg2()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string MatlabSymPrinter::print_Sign( const Sign *s )
/*****************************************************************************/
{
	if (s==NULL) throw InternalError("Printer: Sign is NULL");
    std::cerr << "Note: MatlabSymPrinter: Sign(X) replaced by X/abs(X), since Sign function cannot be used with Matlabs Symbolic Math Toolbox." << std::endl;
    return "(" + print(s->getArg()) + "/abs(" + print(s->getArg()) + "))";
}
/*****************************************************************************/
