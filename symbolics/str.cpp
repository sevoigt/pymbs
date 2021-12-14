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

#include "str.h"
#include <iomanip>
#include <limits>

/*****************************************************************************/
std::string str(  bool b )
/*****************************************************************************/
{
    std::stringstream s;
    s << std::boolalpha << b; //boolalpha, damit die Ausgabe "true" und "false" statt "1" und "0" lautet
    return s.str();
}
/*****************************************************************************/

/*****************************************************************************/
std::string str(  double d )
/*****************************************************************************/
{
    std::stringstream s;
    s << std::setprecision(std::numeric_limits<double>::digits10) << d;
    return s.str();
}
/*****************************************************************************/

/*****************************************************************************/
std::string str(  int i )
/*****************************************************************************/
{
    std::stringstream s;
    s << i;
    return s.str();
}
/*****************************************************************************/

/*****************************************************************************/
std::string str( int i , int fieldwidth )
/*****************************************************************************/
{
    std::stringstream s;
    s << std::setfill('0') << std::setw(fieldwidth) << i;
    return s.str();
}
/*****************************************************************************/

/*****************************************************************************/
std::string str(  size_t i )
/*****************************************************************************/
{
    std::stringstream s;
    s << i;
    return s.str();
}
/*****************************************************************************/

/*****************************************************************************/
std::string str( size_t i , int fieldwidth )
/*****************************************************************************/
{
    std::stringstream s;
    s << std::setfill('0') << std::setw(fieldwidth) << i;
    return s.str();
}
/*****************************************************************************/

/*****************************************************************************/
std::string str( Symbolics::BasicPtr const& p )
/*****************************************************************************/
{
	return p->toString();
}
/*****************************************************************************/

/*****************************************************************************/
std::string str( Symbolics::ConstBasicPtr const& p )
/*****************************************************************************/
{
	return p->toString();
}
/*****************************************************************************/

/*****************************************************************************/
int fieldwidth( int maxElementCount )
/*****************************************************************************/
{
	int digits=1;
	for (int i = maxElementCount; i >= 10; i=i/10) 
		digits++;
	return digits;
}
/*****************************************************************************/