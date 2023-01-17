#include "ModelicaPrinter.h"
#include "str.h"

using namespace Symbolics;

/*****************************************************************************/
ModelicaPrinter::ModelicaPrinter()
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
ModelicaPrinter::~ModelicaPrinter()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
std::string ModelicaPrinter::comment( const std::string &c ) const
/*****************************************************************************/
{
    if (c.empty())
        return "";
    return  " \"" + c + "\"";
}
/*****************************************************************************/

/*****************************************************************************/
std::string ModelicaPrinter::dimension( const BasicPtr &b )
/*****************************************************************************/
{
    if (b->is_Scalar())
        return "";
    return "[" + str(b->getShape().getDimension(1)) + "," + str(b->getShape().getDimension(2)) + "]";
}
/*****************************************************************************/

/*****************************************************************************/
std::string ModelicaPrinter::print_Mul( const Mul *mul )
/*****************************************************************************/
{

    if (mul == NULL) throw InternalError("ModelicaPrinter: Mul is NULL");
	bool makeScalar = false;
	if (mul->is_Scalar())
		for (size_t i=0; i<mul->getArgsSize(); ++i )
			if ( ! mul->getArg(i)->is_Scalar())
				makeScalar = true;
	if (makeScalar) // Wenn das Ergebnis der Multiplikation Skalar ist, in der Mul. jedoch Matritzen enthalten sind muss das Ergebnis skalarisiert werden
		return "scalar(" + join(ConstBasicPtr(mul), " * ") + ")";
	else
		return "(" + join(ConstBasicPtr(mul), " * ") + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string ModelicaPrinter::print_Matrix( const Matrix *c )
/*****************************************************************************/
{
    if (c==NULL) throw InternalError("ModelicaPrinter: Matrix is NULL");
	return Printer::print_Matrix(c, ',', ',',
									'{', '}',
									'{', '}',
									"\n    " );
}
/*****************************************************************************/


/*****************************************************************************/
std::string ModelicaPrinter::print_Inverse( const Inverse *c )
/*****************************************************************************/
{
  if (c == NULL) throw InternalError("ModelicaPrinter: Inverse is NULL");
  return "Modelica.Math.Matrices.inv(" + print(c->getArg()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string ModelicaPrinter::print_Pow( const Pow *pow )
/*****************************************************************************/
{
    if (pow == NULL) throw InternalError("ModelicaPrinter: Pow is NULL");
    return "(" + print(pow->getBase()) + "^" + print(pow->getExponent()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string ModelicaPrinter::print_Der( const Der *d )
/*****************************************************************************/
{
    if (d == NULL) throw InternalError("ModelicaPrinter: Der is NULL");
    return "der(" + print(d->getArg()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string ModelicaPrinter::print_Element( const Element *e )
/*****************************************************************************/
{
	if (e == NULL) throw InternalError("ModelicaPrinter: Element is NULL");
	return print(e->getArg(0)) + "[" + str(e->getRow()+1) + "," + str(e->getCol()+1) + "]";
}
/*****************************************************************************/

/*****************************************************************************/
std::string ModelicaPrinter::print_Skew( const Skew *s )
/*****************************************************************************/
{
    if (s == NULL) throw InternalError("ModelicaPrinter: Skew is NULL");
	return "skew(vector( " + print(s->getArg()) + "))";
	//Alternativ ginge auch:
	//std::string v = print(s->getArg());
    //return "{{0,-" + v + "[3,1], " + v + "[2,1]}, {" + v + "[3,1], 0, -" + v + "[1,1]}, {-" + v + "[2,1], " + v + "[1,1], 0}}";  

}
/*****************************************************************************/


/*****************************************************************************/
std::string ModelicaPrinter::print_Solve( const Solve *s )
/*****************************************************************************/
{
	if (s == NULL) throw InternalError("ModelicaPrinter: Solve is NULL");
	return "Modelica.Math.Matrices.solve2(" + print(s->getArg1()) + ", " + print(s->getArg2()) + ")";
	//solve2 erspart vector() und matrix() Umwandlungen, da Arg2 auch eine Matrix sein kann.
}
/*****************************************************************************/

/*****************************************************************************/
std::string ModelicaPrinter::print_Zero( const Zero *z )
/*****************************************************************************/
{
    if (z == NULL) throw InternalError("ModelicaPrinter: Zero is NULL");
	if (z->is_Scalar())
		return "0";
	return "zeros(" + str(z->getShape().getDimension(1)) + "," + str(z->getShape().getDimension(2)) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string ModelicaPrinter::print_If( const If *e )
/*****************************************************************************/
{
	if (e==NULL) throw InternalError("ModelicaPrinter: If is NULL");
    return "(if " + print(e->getArg(0)) + " then " + print(e->getArg(1)) + " else " + print(e->getArg(2)) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string ModelicaPrinter::print_Scalar( const Scalar *s )
/*****************************************************************************/
{
	if (s == NULL) throw InternalError("ModelicaPrinter: Scalar is NULL");
    return "scalar(" + print(s->getArg()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string ModelicaPrinter::print_Bool( const Bool *b )
/*****************************************************************************/
{
    if (b == NULL) throw InternalError("ModelicaPrinter: Bool is NULL");
    if (b->getValue())
        return "true";
    else
        return "false";
}
/*****************************************************************************/