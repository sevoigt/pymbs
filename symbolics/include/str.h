#ifndef __STR_H_
#define __STR_H_

#include <string>
#include <sstream>
#include "Basic.h"

std::string str( bool b );
std::string str( double d );
std::string str( int d );
// Formatierungsm�glichkeit (f�hrende Nullen) hinzugef�gt. Damit es m�glichst wenig Nebenwirkungen
// hat als Funktions�berladung, man k�nnte es auch als eine Funktion mit Default Parameter (mit 0 oder 1) umsetzen
std::string str( int d, int fieldwidth );
std::string str( size_t i );
std::string str( size_t i, int fieldwidth );
std::string str( Symbolics::BasicPtr const& p );
std::string str( Symbolics::ConstBasicPtr const& p );

// Zur Umrechnung einer Elementanzahl in die Stellenanzahl (bzw. einfach Bestimmung der Stellen einer Zahl)
int fieldwidth( int maxElementCount );


#endif // __STR_H_