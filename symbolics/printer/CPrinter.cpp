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

#include "CPrinter.h"
#include "str.h"

using namespace Symbolics;

/*****************************************************************************/
CPrinter::CPrinter()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
CPrinter::~CPrinter()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
std::string CPrinter::comment( const std::string &c ) const
/*****************************************************************************/
{
    if (c.empty())
        return "";
    return " /*" + c + "*/";
}
/*****************************************************************************/

/*****************************************************************************/
std::string CPrinter::dimension( const BasicPtr &b )
/*****************************************************************************/
{
    if (b->is_Scalar())
        return "";
	if (b->is_Vector())
        return "[" + str(b->getShape().getNumEl()) + "]";
    return "[" + str(b->getShape().getDimension(1)) + "][" + str(b->getShape().getDimension(2)) + "]";
}
/*****************************************************************************/

/*****************************************************************************/
std::string CPrinter::print_Abs( const Abs *s )
/*****************************************************************************/
{ // Wir arbeiten ja immer mit doubles...
	if (s == NULL) throw InternalError("CPrinter: Abs is NULL");
	return "fabs(" + print(s->getArg()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string CPrinter::print_Element( const Element *e )
/*****************************************************************************/
{ 
	if (e == NULL) throw InternalError("CPrinter: Element is NULL");
	if (e->getArg(0)->is_Vector())
	{
		if (e->getRow() > 0)
			return print(e->getArg(0)) + "[" + str(e->getRow()) + "]";
		else
			return print(e->getArg(0)) + "[" + str(e->getCol()) + "]";
	}
	return print(e->getArg(0)) + "[" + str(e->getRow()) + "][" + str(e->getCol()) + "]";
}
/*****************************************************************************/

/*****************************************************************************/
//std::string CPrinter::print_Int( const Int *c )
/*****************************************************************************/
//{ // Als double ausgeben damit es keine Probleme bei z.B. Divisionen gibt?
//  return str(c->getValue()) + ".0";
//}
/*****************************************************************************/

/*****************************************************************************/
std::string CPrinter::print_Matrix( const Matrix *mat )
/*****************************************************************************/
{ // Diese Syntax ist nur zum initialisieren von Matrizen geeignet
    if (mat == NULL) throw InternalError("CPrinter: Matrix is NULL");
    return Printer::print_Matrix(mat, ',', ',',
                                      '{', '}',
                                      '{', '}',
                                      "\n        ", true);
}
/*****************************************************************************/

/*****************************************************************************/
std::string CPrinter::print_Pow( const Pow *pow )
/*****************************************************************************/
{
	if (pow == NULL) throw InternalError("CPrinter: Pow is NULL");
	return "pow(" + print(pow->getBase()) + "," + print(pow->getExponent()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string CPrinter::print_Skew( const Skew *s )
/*****************************************************************************/
{
	return error("CPrinter: Skew: Matrix operations not implemented in C");
	//if (s == NULL) return "Internal Error: Skew";
	//return "skew(" + print(s->getArg()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string CPrinter::print_Solve( const Solve *s )
/*****************************************************************************/
{
    return error("CPrinter: Solve not possible at this point (cannot be inline)");
//	if (s == NULL) throw InternalError("CPrinter: Solve is NULL");
//	return "solve(" + print(s->getArg1()) + "," + print(s->getArg2()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string CPrinter::print_Transpose( const Transpose *s )
/*****************************************************************************/
{
	return error("CPrinter: Transpose: Matrix operations not implemented in C");
	//if (s == NULL) return "Internal Error: Transpose";
	//return "transpose(" + print(s->getArg()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string CPrinter::print_Zero( const Zero *z )
/*****************************************************************************/
{ 
	if (z==NULL) throw InternalError("CPrinter: Zero is NULL");
    if (z->is_Scalar())
		return "0";
    if (z->is_Vector())
	    return "{0}"; 
    return "{{0}}";
    // C initialisiert nicht angegebene Werte immer mit 0, darum passt dies für alle Vektoren und Matrizen
}
/*****************************************************************************/

/*****************************************************************************/
std::string CPrinter::print_If( const If *e )
/*****************************************************************************/
{ 
	if (e==NULL) throw InternalError("CPrinter: If is NULL");
    //Es muss die inline expression sein, da es ja kein Gleichheitszeichen als function gibt
    return "(" + print(e->getArg(0)) + " ? " + print(e->getArg(1)) + " : " + print(e->getArg(2)) + ")";
  //  return "if (" + print(e->getArg(0)) + ")\n        "
  //          + print(e->getArg(1)) + ";\n    else\n        "
  //          + print(e->getArg(2));
}
/*****************************************************************************/

/*****************************************************************************/
std::string CPrinter::print_Sign( const Sign *s )
/*****************************************************************************/
{ 
	if (s==NULL) throw InternalError("CPrinter: Sign is NULL");
	return "(" + print(s->getArg()) + ">0?1:(" + print(s->getArg()) + "<0?-1:0))";
    //Alternativ:
    //return "(" + print(s->getArg()) + "?(" + print(s->getArg()) + ">0?1:-1):0)";
}
/*****************************************************************************/

/*****************************************************************************/
std::string CPrinter::print_Bool( const Bool *b )
/*****************************************************************************/
{
    if (b == NULL) throw InternalError("CPrinter: Bool is NULL");
    if (b->getValue())
        return "1";
    else
        return "0";
}
/*****************************************************************************/