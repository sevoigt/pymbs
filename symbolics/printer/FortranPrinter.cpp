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

#include "FortranPrinter.h"
#include "str.h"

using namespace Symbolics;

/*****************************************************************************/
FortranPrinter::FortranPrinter()
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
FortranPrinter::~FortranPrinter()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
std::string FortranPrinter::comment( const std::string &c ) const
/*****************************************************************************/
{
    if (c.empty())
        return "";
    return " ! " + c;
}
/*****************************************************************************/

/*****************************************************************************/
std::string FortranPrinter::dimension( const BasicPtr &b )
/*****************************************************************************/
{
    if (b->is_Scalar())
        return "";
    return ", dimension(" + str(b->getShape().getDimension(1)) + "," + str(b->getShape().getDimension(2)) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string FortranPrinter::print_Mul( const Mul *mul )
/*****************************************************************************/
{
	if (mul == NULL) throw InternalError("FortranPrinter: Mul is NULL");
	std::string s="(", t="";
    size_t n = mul->getArgsSize();
    bool includes_matmul = false;
    for (size_t i=0; i<n-1; ++i)
    {
		if (!mul->getArg(i)->is_Scalar() && !mul->getArg(i+1)->is_Scalar()) {
			s += "matmul(" + print(mul->getArg(i)) + ",";
			t += ")";
			includes_matmul = true;
		} else {
			s+= print(mul->getArg(i)) + t + "*";
			t="";
		}
    }
    s += print(mul->getArg(n-1)) + t + ")";
	if (includes_matmul && mul->is_Scalar()) //TODO: was ist mit (1,1) Matrizen? 
		s = "scalar" + s; //keine Klammern nötig, diese sollten schon da sein
    return s;
}
/*****************************************************************************/

/*****************************************************************************/
std::string FortranPrinter::print_Element( const Element *e )
/*****************************************************************************/
{
	if (e == NULL) throw InternalError("FortranPrinter: Element is NULL");
    if (e->getArg(0)->getType() != Type_Symbol)
        error("FortranPrinter: Element: Is only possible for Symbols, but here it is: " 
                + print(e->getArg(0)) + "(" + str(e->getRow()+1) + "," + str(e->getCol()+1) + ")");
	return print(e->getArg(0)) + "(" + str(e->getRow()+1) + "," + str(e->getCol()+1) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string FortranPrinter::print_Int( const Int *c )
/*****************************************************************************/
{
    if (c == NULL) throw InternalError("FortranPrinter: Int is NULL");
    int val = c->getValue();
    if (val < 0)
        return "(" + str(val) + "d0)"; // Klammern, damit das Minus nicht direkt auf einen anderen Operator folgt 
    else
	    return str(val) + "d0"; // Gibt keinen Int, alle Real :-)
}
/*****************************************************************************/

/*****************************************************************************/
std::string FortranPrinter::print_Matrix( const Matrix *mat )
/*****************************************************************************/
{
	if (mat == NULL) throw InternalError("FortranPrinter: Matrix is NULL");

	// Skalar
	if (mat->is_Scalar())
		return print((*mat)(0));

	//Vektor und Matrix
	std::string s = "reshape((/";

	size_t rows = mat->getShape().getDimension(1);
	size_t cols = mat->getShape().getDimension(2);

		for (size_t m=0;m<cols;++m)
		{
			for (size_t n=0;n<rows;++n) {
				s += print((*mat)(n,m));
				if ((m+1)*(n+1) < rows*cols) //immer Komma außer am Ende
					s += ", "; 
			}
			if (cols > 1 && m < (rows-1)) //Matritzen mit Zeilenumbrüchen zur besseren Lesbarkeit
				s += "&\n        ";
		}
	
	s += "/), (/";
	
	s+= str(rows) + "," + str(cols) + "/))";
	return s;
}
/*****************************************************************************/

/*****************************************************************************/
std::string FortranPrinter::print_Inverse( const Inverse *c )
/*****************************************************************************/
{ //TODO: Untested weil kein Beispiel vorhanden?
  if (c == NULL) throw InternalError("FortranPrinter: Inverse is NULL");
  return "MIGS(" + print(c->getArg()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string FortranPrinter::print_Pow( const Pow *pow )
/*****************************************************************************/
{
	if (pow == NULL) throw InternalError("FortranPrinter: Pow is NULL");
	if (pow->getBase()->is_Scalar())
		return "(" + print(pow->getBase()) + "**" + print(pow->getExponent()) + ")";
    
    if (!pow->getBase()->is_Matrix())
        return error("FortranPrinter: Pow: Base is neither Scalar nor Matrix, it expands to: '" + print(pow->getBase()) + "'");
	if (pow->getExponent()->getType() != Type_Int)
		return error("FortranPrinter: Pow: Base is matrix but exponent is no Integer, it is of type '"
                    + str(pow->getExponent()->getType()) + "' and expands to '" + print(pow->getExponent()) + "'");
    int exp_value = Util::getAsConstPtr<Int>(pow->getExponent())->getValue();
	return "matpow(" + print(pow->getBase()) + "," + str(exp_value) + ")"; 
    //Exponent darf nicht mit print ausgegeben werden, da ein "echter" Integer benoetigt wird.
}
/*****************************************************************************/

/*****************************************************************************/
std::string FortranPrinter::print_Real( const Real *c )
/*****************************************************************************/
{ //TODO: eigentlich unschöne gehackte Lösung. Andere Ideen?
    if (c == NULL) throw InternalError("FortranPrinter: Real is NULL");
	std::string s = "";
	size_t pos = 0;
	s= str(c->getValue());
	if ((pos = s.find_first_of("eE")) != std::string::npos)
		s[pos]='d';
	else
		s+="d0";

    if (c->getValue() < 0.0)
        return "(" + s + ")"; // Klammern, damit das Minus nicht direkt auf einen anderen Operator folgt 
    else
        return s;
}
/*****************************************************************************/

/*****************************************************************************/
std::string FortranPrinter::print_Scalar( const Scalar *s )
/*****************************************************************************/
{  // Diese Funktion sollte so funktionieren, wird aber scheinbar kaum genutzt - momentan wird es in mul "emuliert"
	if (s == NULL) throw InternalError("FortranPrinter: Scalar is NULL");
	return "scalar(" + print(s->getArg()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string FortranPrinter::print_Skew( const Skew *s )
/*****************************************************************************/
{ 
	if (s == NULL) throw InternalError("FortranPrinter: Skew is NULL");
    return "skew(" + print(s->getArg()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string FortranPrinter::print_Solve( const Solve *s )
/*****************************************************************************/
{
	if (s == NULL) throw InternalError("FortranPrinter: Solve is NULL");
	return "LEGS(" + print(s->getArg1()) + ", " + print(s->getArg2()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string FortranPrinter::print_Symbol( const Symbol *symbol )
/*****************************************************************************/
{
	if (symbol == NULL) throw InternalError("FortranPrinter: Symbol is NULL");
	std::string name = symbol->getName();
	char c = name.at(0);
	if ((c >= 'A' && c <='Z') || (c >= 'a' && c <= 'z'))
		return name;
	else
		return "F" + name;
}
/*****************************************************************************/

/*****************************************************************************/
std::string FortranPrinter::print_Zero( const Zero *z )
/*****************************************************************************/
{
	if (z==NULL) throw InternalError("FortranPrinter: Zero is NULL");
	if (z->is_Scalar())
		return "0d0";
	Shape const& sh = z->getShape();
	size_t el = sh.getNumEl();
	std::string s = "(/";
	for (size_t i = 0; i < el; ++i) { 
		s+= "0d0";
		if (i+1 < el)
			s += ", ";
	}		
	return "reshape(" + s + "/),(/" + str(sh.getDimension(1)) + "," + str(sh.getDimension(2)) + "/))";
}
/*****************************************************************************/

/*****************************************************************************/
//std::string FortranPrinter::print_If( const If *e )
/*****************************************************************************/
//{ // Inline if in Fortran nicht vorhanden, evtl in Funktion auslagern...
//	if (e==NULL) throw InternalError("FortranPrinter: If is NULL");
//	return "if (" + print(e->getArg(0)) + ") then\n        " + print(e->getArg(1)) + "\n    else\n        " + print(e->getArg(2)) + "\n    end if";
//}
/*****************************************************************************/

/*****************************************************************************/
std::string FortranPrinter::print_Sign( const Sign *s )
/*****************************************************************************/
{
	if (s==NULL) throw InternalError("FortranPrinter: Sign is NULL");
	return "sign(1d0, " + print(s->getArg()) + ")"; 
    //Achtung: 0 ergibt hier +1 statt 0 wie in den anderen Sprachen
}
/*****************************************************************************/

/*****************************************************************************/
std::string FortranPrinter::print_Bool( const Bool *b )
/*****************************************************************************/
{
    if (b == NULL) throw InternalError("FortranPrinter: Bool is NULL");
    if (b->getValue())
        return ".TRUE.";
    else
        return ".FALSE.";
}
/*****************************************************************************/
