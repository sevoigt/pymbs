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

#include "PythonPrinter.h"
#include "str.h"

using namespace Symbolics;

/*****************************************************************************/
PythonPrinter::PythonPrinter()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
PythonPrinter::~PythonPrinter()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
std::string PythonPrinter::comment( const std::string &c ) const
/*****************************************************************************/
{
    if (c.empty())
        return "";
    return " # " + c;
}
/*****************************************************************************/

/*****************************************************************************/
std::string PythonPrinter::print_Matrix( const Matrix *c )
/*****************************************************************************/
{
    if (c == NULL) throw InternalError("PythonPrinter: Matrix is NULL");
    return "matrix(" + Printer::print_Matrix(c, ',', ',',
                                                '[', ']',
                                                '[', ']' ) + ")";
}
/*****************************************************************************/


/*****************************************************************************/
std::string PythonPrinter::print_Pow( const Pow *pow )
/*****************************************************************************/
{
    if (pow == NULL) throw InternalError("PythonPrinter: Pow is NULL");
		return "(" + print(pow->getBase()) + "**" + print(pow->getExponent()) + ")";
}
/*****************************************************************************/


/*****************************************************************************/
std::string PythonPrinter::print_Mul( const Mul *mul )
/*****************************************************************************/
{

    if (mul == NULL) throw InternalError("PythonPrinter: Mul is NULL");
	bool includesMatrix = false;
	if (mul->is_Scalar())
		for (size_t i=0; i<mul->getArgsSize(); ++i )
			if ( ! mul->getArg(i)->is_Scalar())
				includesMatrix = true;
	if (includesMatrix) // Wenn das Ergebnis der Multiplikation Skalar ist, in der Mul. jedoch Matritzen enthalten sind muss das Ergebnis skalarisiert werden
		return "(" + join(ConstBasicPtr(mul), " * ") + ")[0,0]";
	return "(" + join(ConstBasicPtr(mul), " * ") + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string PythonPrinter::print_Element( const Element *e )
/*****************************************************************************/
{
    if (e == NULL) throw InternalError("PythonPrinter: Element is NULL");
	return print(e->getArg(0)) + "[" + str(e->getRow()) + "," + str(e->getCol()) + "]";
}
/*****************************************************************************/


/*****************************************************************************/
std::string PythonPrinter::print_Solve( const Solve *s )
/*****************************************************************************/
{
    if (s == NULL) throw InternalError("PythonPrinter: Solve is NULL");
    return "matrix(linalg.solve(" + print(s->getArg1()) + "," + print(s->getArg2()) + "))";
	// Das Ergebnis von solve ist ein ndarray, muss somit "matrifiziert" werden
}
/*****************************************************************************/


/*****************************************************************************/
std::string PythonPrinter::print_Transpose( const Transpose *t )
/*****************************************************************************/
{
    if (t == NULL) throw InternalError("PythonPrinter: Transpose is NULL");
    return print(t->getArg()) + ".T";
}
/*****************************************************************************/


/*****************************************************************************/
std::string PythonPrinter::print_Scalar( const Scalar *s )
/*****************************************************************************/
{
    if (s == NULL) throw InternalError("PythonPrinter: Scalar is NULL");
    return print(s->getArg()) + "[0,0]";
}
/*****************************************************************************/


/*****************************************************************************/
std::string PythonPrinter::print_Skew( const Skew *s )
/*****************************************************************************/
{
    if (s == NULL) throw InternalError("PythonPrinter: Skew is NULL");
    std::string v = print(s->getArg());
    return "matrix([[0,-" + v + "[2,0], " + v + "[1,0]], [" + v + "[2,0], 0, -" + v + "[0,0]], [-" + v + "[1,0], " + v + "[0,0], 0]])";  

}
/*****************************************************************************/


/*****************************************************************************/
std::string PythonPrinter::print_Zero( const Zero *z )
/*****************************************************************************/
{
    if ( z == NULL) throw InternalError("PythonPrinter: Zero is NULL");
    if (z->is_Scalar())
		return "0";
    // Auch zero muss "matrifiziert" werden
    return "matrix(zeros( (" + str(z->getShape().getDimension(1)) + "," + str(z->getShape().getDimension(2)) + ") ))";

}
/*****************************************************************************/

/*****************************************************************************/
std::string PythonPrinter::print_If( const If *e )
/*****************************************************************************/
{
	if (e==NULL) throw InternalError("Printer: If is NULL");
    //we need to use an inline expression, since there is no function for an assignement operator
	return "(" + print(e->getArg(1)) + " if " + print(e->getArg(0)) + " else " + print(e->getArg(2)) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string PythonPrinter::print_Inverse( const Inverse *c )
/*****************************************************************************/
{
    if (c == NULL) throw InternalError("Printer: Inverse is NULL");
	return print(c->getArg()) + ".I";
}
/*****************************************************************************/

/*****************************************************************************/
std::string PythonPrinter::print_Bool( const Bool *b )
/*****************************************************************************/
{
    if (b == NULL) throw InternalError("PythonPrinter: Bool is NULL");
    if (b->getValue())
        return "True";
    else
        return "False";
}
/*****************************************************************************/
