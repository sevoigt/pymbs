#include "MatlabPrinter.h"
#include "str.h"

using namespace Symbolics;

/*****************************************************************************/
MatlabPrinter::MatlabPrinter()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
MatlabPrinter::~MatlabPrinter()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
std::string MatlabPrinter::comment( const std::string &c ) const
/*****************************************************************************/
{
    if (c.empty())
        return "";
    return " % " + c;
}
/*****************************************************************************/

/*****************************************************************************/
std::string MatlabPrinter::print_Matrix( const Matrix *c )
/*****************************************************************************/
{
    if (c == NULL) throw InternalError("MatlabPrinter: Matrix is NULL");
    return Printer::print_Matrix(c, ',', ';',
                                    '[', ']',
                                    '[', ']');
}
/*****************************************************************************/

/*****************************************************************************/
std::string MatlabPrinter::print_Pow( const Pow *pow )
/*****************************************************************************/
{
    if (pow == NULL) throw InternalError("MatlabPrinter: Pow is NULL");
    return "(" + print(pow->getBase()) + "^" + print(pow->getExponent()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string MatlabPrinter::print_Solve( const Solve *s )
/*****************************************************************************/
{
    if (s == NULL) throw InternalError("MatlabPrinter: Solve is NULL");
	//Was ist mit dsolve/solve/linsolve? --> wir nutzen "Backslash"-Solve
    return "(" + print(s->getArg1()) + "\\" + print(s->getArg2()) + ")";
}
/*****************************************************************************/

//*****************************************************************************/
std::string MatlabPrinter::print_Element( const Element *e )
//*****************************************************************************/
{
    if (e == NULL) throw InternalError("MatlabPrinter: Element is NULL");
    if (e->getArg(0)->getType() != Type_Symbol)
        error("MatlabPrinter: Element: Is only possible for Symbols, but here it is: " 
                + print(e->getArg(0)) + "(" + str(e->getRow()+1) + "," + str(e->getCol()+1) + ")");
    return print(e->getArg(0)) + "(" + str(e->getRow()+1) + "," + str(e->getCol()+1) + ")";
}
//*****************************************************************************/

/*****************************************************************************/
std::string MatlabPrinter::print_Skew( const Skew *s )
/*****************************************************************************/
{
    if (s == NULL) throw InternalError("MatlabPrinter: Skew is NULL");
    if (s->getArg()->getType() != Type_Symbol)
        error("MatlabPrinter: Skew: Is only possible for Symbols, but the Argument is: " + print(s->getArg()));
	std::string v = print(s->getArg());
    return "[[0,-" + v + "(3), " + v + "(2)];\n         [" + v + "(3), 0, -" + v + "(1)];\n         [-" + v + "(2), " + v + "(1), 0]]";
}
/*****************************************************************************/

/*****************************************************************************/
std::string MatlabPrinter::print_Zero( const Zero *z )
/*****************************************************************************/
{
	if (z == NULL) throw InternalError("MatlabPrinter: Zero is NULL");
	if (z->is_Scalar())
		return "0";
	return "zeros(" + str(z->getShape().getDimension(1)) + "," + str(z->getShape().getDimension(2)) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string MatlabPrinter::print_Symbol( const Symbol *symbol )
/*****************************************************************************/
{
	if (symbol == NULL) throw InternalError("MatlabPrinter: Symbol is NULL");
	std::string name = symbol->getName();
	char c = name.at(0);
	if ((c >= 'A' && c <='Z') || (c >= 'a' && c <= 'z'))
		return name;
	else
		return "M" + name;
}
/*****************************************************************************/

/*****************************************************************************/
std::string MatlabPrinter::print_Inverse( const Inverse *c )
/*****************************************************************************/
{
    if (c == NULL) throw InternalError("MatlabPrinter: Inverse is NULL");
	return "inv(" + print(c->getArg()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string MatlabPrinter::print_Jacobian( const Jacobian *j )
/*****************************************************************************/
{
	if (j==NULL) throw InternalError("MatlabPrinter: Jacobian is NULL");
	return "jacobian(" + print(j->getArg1()) + "," + print(j->getArg2()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string MatlabPrinter::print_Bool( const Bool *b )
/*****************************************************************************/
{
    if (b == NULL) throw InternalError("MatlabPrinter: Bool is NULL");
    if (b->getValue())
        return "true";
    else
        return "false";
}
/*****************************************************************************/
