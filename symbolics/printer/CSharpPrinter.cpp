/*
Prints expressions in C# using the MathNet.Numerics library for linear algebra
*/

#include "CSharpPrinter.h"
#include "str.h"

using namespace Symbolics;



CSharpPrinter::CSharpPrinter()
{
}



CSharpPrinter::~CSharpPrinter()
{
}



std::string CSharpPrinter::comment( const std::string &c ) const
{
    if (c.empty())
        return "";
    return " /*" + c + "*/";
}



std::string CSharpPrinter::dimension( const BasicPtr &b )
{
    if (b->is_Scalar())
        return "";
	if (b->is_Vector())
        return str(b->getShape().getNumEl());
    return str(b->getShape().getDimension(1)) + ", " + str(b->getShape().getDimension(2));
}



std::string CSharpPrinter::print_Abs( const Abs *s )
{
	if (s == NULL) throw InternalError("CSharpPrinter: Abs is NULL");
	return "Math.Abs(" + print(s->getArg()) + ")";
}



std::string CSharpPrinter::print_Element( const Element *e )
{ 
	if (e == NULL) throw InternalError("CSharpPrinter: Element is NULL");
	if (e->getArg(0)->is_Vector())
	{
		if (e->getRow() > 0)
			return print(e->getArg(0)) + "[" + str(e->getRow()) + "]";
		else
			return print(e->getArg(0)) + "[" + str(e->getCol()) + "]";
	}
	return print(e->getArg(0)) + "[" + str(e->getRow()) + "," + str(e->getCol()) + "]";
}



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
	return "Math.Pow(" + print(pow->getBase()) + "," + print(pow->getExponent()) + ")";
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
	    return "0";       // Todo: write constructors for zero vector and matrix 
    return "0";
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


std::string CSharpPrinter::print_Jacobian( const Jacobian *j )
{    
    throw InternalError("CSharpPrinter: Jacobian not implemented");

    if (j == NULL) throw InternalError("CSharpPrinter: Jacobian is NULL");
    return print(j->getArg(0)) + ".Jacobian(???)";
}


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



std::string CSharpPrinter::print_Acos( const Acos *s )
{
    if (s == NULL) throw InternalError("CSharpPrinter: Acos is NUll");
    return "Math.Acos(" + print(s->getArg()) + ")";
}


std::string CSharpPrinter::print_Asin( const Asin *s )
{
    if (s == NULL) throw InternalError("CSharpPrinter: Asin is NUll");
    return "Math.Asin(" + print(s->getArg()) + ")";
}


std::string CSharpPrinter::print_Atan( const Atan *s )
{
    if (s == NULL) throw InternalError("CSharpPrinter: Atan is NUll");
    return "Math.Atan(" + print(s->getArg()) + ")";
}


std::string CSharpPrinter::print_Atan2( const Atan2 *s )
{
    if (s == NULL) throw InternalError("CSharpPrinter: Atan2 is NUll");
    return "Math.Atan2(" + print(s->getArg1()) + "," + print(s->getArg2()) + ")";
}


std::string CSharpPrinter::print_Cos( const Cos *c )
{
    if (c == NULL) throw InternalError("CSharpPrinter: Cos is NUll");
    return "Math.Cos(" + print(c->getArg()) + ")";
}


std::string CSharpPrinter::print_Sin( const Sin *s )
{
    if (s == NULL) throw InternalError("CSharpPrinter: Sin is NUll");
    return "Math.Sin(" + print(s->getArg()) + ")";
}


std::string CSharpPrinter::print_Tan( const Tan *s )
{
    if (s == NULL) throw InternalError("CSharpPrinter: Tan is NUll");
    return "Math.Tan(" + print(s->getArg()) + ")";
}