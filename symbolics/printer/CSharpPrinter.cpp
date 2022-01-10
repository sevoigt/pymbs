/*
Prints expressions in C#  with the Math.NET library.
*/

#include "CSharpPrinter.h"
#include "str.h"

using namespace Symbolics;

/*****************************************************************************/
CSharpPrinter::CSharpPrinter()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
CSharpPrinter::~CSharpPrinter()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
std::string CSharpPrinter::comment( const std::string &c ) const
/*****************************************************************************/
{
    if (c.empty())
        return "";
    return " /*" + c + "*/";
}
/*****************************************************************************/

/*****************************************************************************/
std::string CSharpPrinter::dimension( const BasicPtr &b )
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
std::string CSharpPrinter::print_Abs( const Abs *s )
/*****************************************************************************/
{
	if (s == NULL) throw InternalError("CSharpPrinter: Abs is NULL");
	return "Math.Abs(" + print(s->getArg()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string CSharpPrinter::print_Element( const Element *e )
/*****************************************************************************/
{ 
	if (e == NULL) throw InternalError("CSharpPrinter: Element is NULL");
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
//std::string CSharpPrinter::print_Int( const Int *c )
/*****************************************************************************/
//{ // Als double ausgeben damit es keine Probleme bei z.B. Divisionen gibt?
//  return str(c->getValue()) + ".0";
//}
/*****************************************************************************/

/*****************************************************************************/
std::string CSharpPrinter::print_Matrix( const Matrix *mat )
/*****************************************************************************/
{ // Diese Syntax ist nur zum initialisieren von Matrizen geeignet
    if (mat == NULL) throw InternalError("CSharpPrinter: Matrix is NULL");
    return Printer::print_Matrix(mat, ',', ',',
                                      '{', '}',
                                      '{', '}',
                                      "\n        ", true);
}
/*****************************************************************************/

/*****************************************************************************/
std::string CSharpPrinter::print_Pow( const Pow *pow )
/*****************************************************************************/
{
	if (pow == NULL) throw InternalError("CSharpPrinter: Pow is NULL");
	return "Math.Exp(" + print(pow->getBase()) + "," + print(pow->getExponent()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string CSharpPrinter::print_Skew( const Skew *s )
/*****************************************************************************/
{
	return error("CSharpPrinter: Skew: Matrix operations not implemented in C#");
	//if (s == NULL) return "Internal Error: Skew";
	//return "skew(" + print(s->getArg()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string CSharpPrinter::print_Solve( const Solve *s )
/*****************************************************************************/
{
    return error("CSharpPrinter: Solve not possible at this point (cannot be inline)");
//	if (s == NULL) throw InternalError("CSharpPrinter: Solve is NULL");
//	return "solve(" + print(s->getArg1()) + "," + print(s->getArg2()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string CSharpPrinter::print_Transpose( const Transpose *s )
/*****************************************************************************/
{
    if (s == NULL) throw InternalError("CSharpPrinter: Transpose is NULL");
	
	return print(s->getArg()) + ".Transpose()";
}
/*****************************************************************************/

/*****************************************************************************/
std::string CSharpPrinter::print_Zero( const Zero *z )
/*****************************************************************************/
{ 
	if (z==NULL) throw InternalError("CSharpPrinter: Zero is NULL");
    if (z->is_Scalar())
		return "0";
    if (z->is_Vector())
	    return "{0}"; 
    return "{{0}}";
    // C# initialisiert nicht angegebene Werte immer mit 0, darum passt dies 
    // für alle Vektoren und Matrizen
}
/*****************************************************************************/


/*****************************************************************************/
std::string CSharpPrinter::print_If( const If *e )
/*****************************************************************************/
{ 
	if (e==NULL) throw InternalError("CSharpPrinter: If is NULL");
    //Es muss die inline expression sein, da es ja kein Gleichheitszeichen als function gibt
    return print(e->getArg(0)) + " ? " + print(e->getArg(1)) + " : " + print(e->getArg(2));
}
/*****************************************************************************/


/*****************************************************************************/
std::string CSharpPrinter::print_Inverse( const Inverse *c )
/*****************************************************************************/
{
	if (c==NULL) throw InternalError("CSharpPrinter: Inverse is NULL");
	return print(c->getArg()) + ".Inverse()";
}
/*****************************************************************************/


/*****************************************************************************/
std::string CSharpPrinter::print_Sign( const Sign *s )
/*****************************************************************************/
{ 
	if (s==NULL) throw InternalError("CSharpPrinter: Sign is NULL");
	return "Math.Sign(" + print(s->getArg()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string CSharpPrinter::print_Bool( const Bool *b )
/*****************************************************************************/
{
    if (b == NULL) throw InternalError("CSharpPrinter: Bool is NULL");
    if (b->getValue())
        return "true";
    else
        return "false";
}
/*****************************************************************************/