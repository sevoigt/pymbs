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

#ifndef __STR_H_
#define __STR_H_

#include <string>
#include <sstream>
#include "Basic.h"

std::string str( bool b );
std::string str( double d );
std::string str( int d );
// Formatierungsmöglichkeit (führende Nullen) hinzugefügt. Damit es möglichst wenig Nebenwirkungen
// hat als Funktionsüberladung, man könnte es auch als eine Funktion mit Default Parameter (mit 0 oder 1) umsetzen
std::string str( int d, int fieldwidth );
std::string str( size_t i );
std::string str( size_t i, int fieldwidth );
std::string str( Symbolics::BasicPtr const& p );
std::string str( Symbolics::ConstBasicPtr const& p );

// Zur Umrechnung einer Elementanzahl in die Stellenanzahl (bzw. einfach Bestimmung der Stellen einer Zahl)
int fieldwidth( int maxElementCount );


#endif // __STR_H_