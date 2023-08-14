#include "Printer.h"
#include "str.h"
#include <iostream>

using namespace Symbolics;

/*****************************************************************************/
Printer::Printer()
/*****************************************************************************/
{
    m_errorcount = 0;
}
/*****************************************************************************/


/*****************************************************************************/
Printer::~Printer()
/*****************************************************************************/
{
}
/*****************************************************************************/

// Beispielimplementierung
/*****************************************************************************/
//std::string Printer::comment( const std::string &c ) const
/*****************************************************************************/
//{
//    if (c.empty())
//        return "";
//    return " // " + c;
//}
/*****************************************************************************/

//Wrapperfunktion
/*****************************************************************************/
std::string Printer::comment2( Graph::Graph& g, SymbolPtr const& s ) const
/*****************************************************************************/
{
    return comment(g.getComment(s));
}
/*****************************************************************************/

/*****************************************************************************/
std::string Printer::dimension( const BasicPtr &b )
/*****************************************************************************/
{
    return error ("Printer: Get dimension not implemented in chosen export language");
    //if (b->is_Scalar())
    //    return "";
	//if (b->is_Vector())
    //    return "[" + str(b->getShape().getNumEl()) + "]";
    //return "[" + str(b->getShape().getDimension(1)) + "][" + str(b->getShape().getDimension(2)) + "]";
}
/*****************************************************************************/

/*****************************************************************************/
std::string Printer::print( BasicPtr const& basic )
/*****************************************************************************/
{
	if (basic.get() == NULL) throw InternalError("Printer: Basic is NULL");
	// Nach Typ unterscheiden
	switch(basic->getType())
	{
	case Type_Abs:
		return print_Abs( Util::getAsConstPtr<Abs>(basic) );
	case Type_Acos:
		return print_Acos( Util::getAsConstPtr<Acos>(basic) );
	case Type_Add:
		return print_Add( Util::getAsConstPtr<Add>(basic) );
	case Type_Asin:
		return print_Asin( Util::getAsConstPtr<Asin>(basic) );
	case Type_Atan:
		return print_Atan( Util::getAsConstPtr<Atan>(basic) );
	case Type_Atan2:
		return print_Atan2( Util::getAsConstPtr<Atan2>(basic) );
	case Type_Cos:
		return print_Cos( Util::getAsConstPtr<Cos>(basic) );
	case Type_Der:
		return print_Der( Util::getAsConstPtr<Der>(basic) );
	case Type_Element:
		return print_Element( Util::getAsConstPtr<Element>(basic) );
	case Type_Equal:
		return print_Equal( Util::getAsConstPtr<Equal>(basic) );
	case Type_Greater:
		return print_Greater( Util::getAsConstPtr<Greater>(basic) );
	case Type_If:
		return print_If( Util::getAsConstPtr<If>(basic) );
	case Type_Int:
		return print_Int( Util::getAsConstPtr<Int>(basic) );
	case Type_Inverse:
		return print_Inverse( Util::getAsConstPtr<Inverse>(basic) );
	case Type_Jacobian:
		return print_Jacobian( Util::getAsConstPtr<Jacobian>(basic) );
	case Type_Less:
		return print_Less( Util::getAsConstPtr<Less>(basic) );
	case Type_Matrix:
		return print_Matrix( Util::getAsConstPtr<Matrix>(basic) );
	case Type_Mul:
		return print_Mul( Util::getAsConstPtr<Mul>(basic) );
	case Type_Neg:
		return print_Neg( Util::getAsConstPtr<Neg>(basic) );
	case Type_Pow:
		return print_Pow( Util::getAsConstPtr<Pow>(basic) );
	case Type_Outer:
		return print_Outer( Util::getAsConstPtr<Outer>(basic) );
	case Type_Real:
		return print_Real( Util::getAsConstPtr<Real>(basic) );
	case Type_Scalar:
		return print_Scalar( Util::getAsConstPtr<Scalar>(basic) );
	case Type_Sign:
		return print_Sign( Util::getAsConstPtr<Sign>(basic) );
	case Type_Sin:
		return print_Sin( Util::getAsConstPtr<Sin>(basic) );
	case Type_Skew:
		return print_Skew( Util::getAsConstPtr<Skew>(basic) );
	case Type_Solve:
		return print_Solve( Util::getAsConstPtr<Solve>(basic) );
	case Type_Symbol:
		return print_Symbol( Util::getAsConstPtr<Symbol>(basic) );
	case Type_Tan:
		return print_Tan( Util::getAsConstPtr<Tan>(basic) );
	case Type_Transpose:
		return print_Transpose( Util::getAsConstPtr<Transpose>(basic) );
	case Type_Zero:
		return print_Zero( Util::getAsConstPtr<Zero>(basic) );
    case Type_Unknown:
        return print_Unknown( Util::getAsConstPtr<Unknown>(basic) );
    case Type_Bool:
        return print_Bool( Util::getAsConstPtr<Bool>(basic) );
	default:
		throw InternalError("Printer: 'print' called with unrecognized type!");
	}
}
/*****************************************************************************/

//Implementationsbeispiele der komplett virtuellen Funktionen

/*****************************************************************************/
//std::string Printer::print_Element( const Element *e )
/*****************************************************************************/
//{
//	if (e == NULL) throw InternalError("Printer: Element is NULL");
//	return print(e->getArg(0)) + "[" + str(e->getRow()) + "," + str(e->getCol()) + "]";
//}
/*****************************************************************************/

/*****************************************************************************/
//std::string Printer::print_Matrix( const Matrix *c )
/*****************************************************************************/
//{
//  if (c == NULL) throw InternalError("Printer: Matrix is NULL");
//	return print_Matrix(c, ',', ',',
//		'[', ']',
//		'[', ']');
//}
/*****************************************************************************/

///*****************************************************************************/
//std::string Printer::print_Pow( const Pow *pow )
/*****************************************************************************/
//{
//	if (pow == NULL) throw InternalError("Printer: Pow is NULL");
//	return "pow(" + print(pow->getBase()) + "," + str(pow->getExponent()) + ")";
//}
/*****************************************************************************/

/*****************************************************************************/
//std::string Printer::print_Skew( const Skew *s )
/*****************************************************************************/
//{
//	if (s == NULL) throw InternalError("Printer: Skew is NULL");
//	return "skew(" + print(s->getArg()) + ")";
//}
/*****************************************************************************/

/*****************************************************************************/
//std::string Printer::print_Zero( const Zero *z )
/*****************************************************************************/
//{
//	if (z==NULL) throw InternalError("Printer: Zero is NULL");
//  if (z->is_Scalar())
//      return "0";
//  return "zero(" + str(z->getShape().getDimension(1)) + "," + str(z->getShape().getDimension(2)) + ")";
//}
/*****************************************************************************/

//Die nur mit Fehlerausgabe implementierten Funktionen:

/*****************************************************************************/
std::string Printer::print_If( const If *e )
/*****************************************************************************/
{
	if (e==NULL) throw InternalError("Printer: If is NULL");
    return error ("Printer: Function 'If' not implemented in chosen export language");
    //Beispielimplementierung:
    //you need to use an inline expression, since there is no function for an assignement operator
	//return "if (" + print(e->getArg(0)) + ") " + print(e->getArg(1)) + " else " + print(e->getArg(2));
}
/*****************************************************************************/

/*****************************************************************************/
std::string Printer::print_Inverse( const Inverse *c )
/*****************************************************************************/
{
    if (c == NULL) throw InternalError("Printer: Inverse is NULL");
    return error ("Printer: Function 'Inverse' not implemented in chosen export language");
    //Beispielimplementierung:
	//return "inv(" + print(c->getArg()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string Printer::print_Jacobian( const Jacobian *j )
/*****************************************************************************/
{
	if (j==NULL) throw InternalError("Printer: Jacobian is NULL");
	return error ("Printer: Function 'Jacobian' not implemented in chosen export language");
    //Beispielimplementierung:
	//return "jacobian(" + print(j->getArg1()) + "," + print(j->getArg2()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string Printer::print_Scalar( const Scalar *s )
/*****************************************************************************/
{
	if (s == NULL) throw InternalError("Printer: Scalar is NULL");
    return error ("Printer: Function 'Scalar' not implemented in chosen export language");
    //Beispielimplementierung:
	//return "scalar(" + print(s->getArg()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string Printer::print_Solve( const Solve *s )
/*****************************************************************************/
{
	if (s == NULL) throw InternalError("Printer: Solve is NULL");
    return error ("Printer: Function 'Solve' not implemented in chosen export language");
    //Beispielimplementierung:
	//return "solve(" + print(s->getArg1()) + "," + print(s->getArg2()) + ")";
}
/*****************************************************************************/


//Implementation der Standardausgaben der restlichen Funktionen

/*****************************************************************************/
std::string Printer::print_Abs( const Abs *s )
/*****************************************************************************/
{
	if (s == NULL) throw InternalError("Printer: Abs is NULL");
	return "abs(" + print(s->getArg()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string Printer::print_Sign( const Sign *s )
/*****************************************************************************/
{
	if (s==NULL) throw InternalError("Printer: Sign is NULL");
    return "sign(" + print(s->getArg()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string Printer::print_Acos( const Acos *s )
/*****************************************************************************/
{
	if (s == NULL) throw InternalError("Printer: Acos is NULL");
	return "acos(" + print(s->getArg()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string Printer::print_Add( const Add *add )
/*****************************************************************************/
{
	if (add == NULL) throw InternalError("Printer: Add is NULL");
	return "(" + join(ConstBasicPtr(add), " + ", " - ") + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string Printer::print_Asin( const Asin *s )
/*****************************************************************************/
{
	if (s == NULL) throw InternalError("Printer: Asin is NULL");
	return "asin(" + print(s->getArg()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string Printer::print_Atan( const Atan *s )
/*****************************************************************************/
{
	if (s == NULL) throw InternalError("Printer: Atan is NULL");
	return "atan(" + print(s->getArg()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string Printer::print_Atan2( const Atan2 *s )
/*****************************************************************************/
{
	if (s == NULL) throw InternalError("Printer: Atan2 is NULL");
	return "atan2(" + print(s->getArg1()) + "," + print(s->getArg2()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string Printer::print_Cos( const Cos *c )
/*****************************************************************************/
{
	if (c == NULL) throw InternalError("Printer: Cos is NULL");
	return "cos(" + print(c->getArg()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string Printer::print_Der( const Der *d )
/*****************************************************************************/
{
	if (d == NULL) throw InternalError("Printer: Der is NULL");
    const BasicPtr arg = d->getArg();
    if ( ! (arg->getType() == Type_Symbol || (arg->getType() == Type_Element && arg->getArg(0)->getType() == Type_Symbol)))
        return error("Printer: Der: Argument of Der can only be Symbol or Element of Symbol in chosen export language, but it is of type '" 
                    + str(arg->getType()) + "' and expands to: '" + print(arg) + "'");
	return "der_" + print(arg);
}
/*****************************************************************************/

/*****************************************************************************/
std::string Printer::print_Int( const Int *c )
/*****************************************************************************/
{
    if (c == NULL) throw InternalError("Printer: Int is NULL");
    int val = c->getValue();
    if (val < 0)
	    return "(" + str(val) + ")"; // Klammern, damit das Minus nicht direkt auf einen anderen Operator folgt 
    else
        return str(val);
}
/*****************************************************************************/

/*****************************************************************************/
std::string Printer::print_Mul( const Mul *mul )
/*****************************************************************************/
{
	if (mul == NULL) throw InternalError("Printer: Mul is NULL");
	return "(" + join(ConstBasicPtr(mul), " * ") + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string Printer::print_Neg( const Neg *neg )
/*****************************************************************************/
{
	if (neg == NULL) throw InternalError("Printer: Neg is NULL");
	return "(-" + print(neg->getArg()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string Printer::print_Real( const Real *c )
/*****************************************************************************/
{
    if (c == NULL) throw InternalError("Printer: Real is NULL");
    double val = c->getValue();
    if (val < 0.0)
	    return "(" + str(val) + ")"; // Klammern, damit das Minus nicht direkt auf einen anderen Operator folgt 
    else
        return str(val);
}
/*****************************************************************************/

/*****************************************************************************/
std::string Printer::print_Sin( const Sin *s )
/*****************************************************************************/
{
	if (s == NULL) throw InternalError("Printer: Sin is NULL");
	return "sin(" + print(s->getArg()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string Printer::print_Symbol( const Symbol *symbol )
/*****************************************************************************/
{
	if (symbol == NULL) throw InternalError("Printer: Symbol is NULL");
	return symbol->getName();
}
/*****************************************************************************/

/*****************************************************************************/
std::string Printer::print_Tan( const Tan *s )
/*****************************************************************************/
{
	if (s == NULL) throw InternalError("Printer: Tan is NULL");
	return "tan(" + print(s->getArg()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string Printer::print_Transpose( const Transpose *s )
/*****************************************************************************/
{
	if (s == NULL) throw InternalError("Printer: Transpose is NULL");
	return "transpose(" + print(s->getArg()) + ")";
}
/*****************************************************************************/


/*****************************************************************************/
std::string Printer::print_Unknown( const Unknown *s )
/*****************************************************************************/
{
	if (s == NULL) throw InternalError("Printer: Unknown is NULL");
	return s->getName() + "(" + join(ConstBasicPtr(s), ",") + ")";
}
/*****************************************************************************/


/*****************************************************************************/
std::string Printer::join( ConstBasicPtr const& arg,  std::string const& sep)
/*****************************************************************************/
{
	// Groesse
	size_t n = arg->getArgsSize();

	// Muss ein Zeichen eingefuegt werden
	if (n == 0)
		return "";

	// Zusammenbauen
	std::string s = print(arg->getArg(0));
	for(size_t i=1; i<n; ++i)
	{
		s += sep + print(arg->getArg(i));
	}

	// Fertig
	return s;
};
/*****************************************************************************/


/*****************************************************************************/
std::string Printer::join( ConstBasicPtr const& arg,
						  std::string const& posSep,
						  std::string const& negSep)
/*****************************************************************************/
{
	// Groesse
	size_t n = arg->getArgsSize();

	// Muss ein Zeichen eingefuegt werden
	if (n == 0)
		return "";

	// Zusammenbauen
	std::string s = print(arg->getArg(0));
	for(size_t i=1; i<n; ++i)
	{
		BasicPtr a = arg->getArg(i);
		// + oder - hinzufuegen
		switch(a->getType())
		{
		case Type_Neg:
			{
				const Neg *neg = Util::getAsConstPtr<Neg>(a);
				s += negSep + print(neg->getArg());
				break;
			}
		case Type_Int:
			{
				const Int *i = Util::getAsConstPtr<Int>(a);
				if (i->getValue() < 0)
					s += negSep + print( Int::New( -i->getValue() ) );
				else
					s += posSep + print( a );
				break;
			}
		case Type_Real:
			{
				const Real *r = Util::getAsConstPtr<Real>(a);
				if (r->getValue() < 0)
					s += negSep + print( new Real( -r->getValue() ) );
				else
					s += posSep + print( a );
				break;
			}
		default:
			s += posSep + print(a);
			break;
		}
	}

	// Fertig
	return s;
};
/*****************************************************************************/

// Eine Matrix (Constant_Dbl, Constant_Int, Matrix) mit definierten Trennzeichen ausgeben:
// Matrix = <leftConstr> <leftRowBracket> ... <rightRowBracket> <rowSep> <leftRowBracket> ... <rightRowBracket> <rightConstr>
// Vektoren werden bei unterschiedlichem rowSep und colSep wie folgt ausgegeben:
//     Spaltenvektor = <leftConstr> ... <rowSep> ... <rightConstr>
//     Zeilenvektor  = <leftConstr> ... <colSep> ... <rightConstr>
// Bei identischem rowSep und colSep werden Vektoren wie Matrizen ausgeben, au�er es ist forceNoMatrixVectors gesetzt,
// womit allerdings die Information ob es sich um einen Zeilen oder Spaltenvektor handelt verloren geht.

/*****************************************************************************/
std::string Printer::print_Matrix( const Matrix *mat,
								   char colSep,  char rowSep, 
								   char leftConstr,  char rightConstr,
								   char leftRowBracket,  char rightRowBracket,
								   std::string matrixLineBreak,
								   bool forceNoMatrixVectors )
/*****************************************************************************/
{
	if (mat == NULL) throw InternalError("Printer: Matrix is NULL");

	// Skalar
	if (mat->is_Scalar())
	{
		return print((*mat)(0));
	}

	// Vektor
	std::string s = "";
	if (mat->is_Vector() && (colSep != rowSep || forceNoMatrixVectors))
	{
		// Gesamtgroesse
		size_t size = mat->getShape().getNumEl();
		char Sep= '!'; //irgendwas damit man es erkennt ;)
		// Spalten oder Zeilenvektor?
		if (mat->getShape().getDimension(1) > 1)
			Sep = rowSep;
		else if (mat->getShape().getDimension(2) > 1)
			Sep = colSep;
		//else throw InternalError("Printer: Unexpected error (Matrix is Vector but no dimension is > 1)");
		//Es kann wohl auftreten wenn Skalare als Vektoren deklariert sind. Aber dann sollte der Seperator nicht gebraucht werden -> wenn ein � im Code auftaucht ist was faul
		s += leftConstr; //TODO: Klammernunterscheidung (gibts �berhaupt ne Sprache die sowas hat?)
		for (size_t l=0;l<size;++l)
		{
			// Element anhaengen
			s += print((*mat)(l));
			// Hinter letztem Zeichen kein Trennzeichen

			if (l<(size-1)) s += Sep;
		}
		return s + rightConstr;
	}

	// Matrix
	if (mat->is_Matrix() || mat->is_Vector())
	{
		// Groesse
		size_t rows = mat->getShape().getDimension(1);
		size_t cols = mat->getShape().getDimension(2);

		s += leftConstr;
		for (size_t m=0;m<rows;++m)
		{
			s += leftRowBracket;
			for (size_t n=0;n<cols;++n)
			{
				// Element anhaengen
				s += print((*mat)(m,n));
				// Wenn wir noch nicht am Ende der Zeile sind
				if (n < (cols-1))
					s += colSep;
			}
			s += rightRowBracket;
			if (m < (rows-1))
				s = s+ rowSep + (mat->is_Vector() ? "" : matrixLineBreak ); //Vektoren ohne Zeilenumbruch ausgeben
		}
		return s + rightConstr;
	}

	// Error
	return error("Printer: Matrix: Element must be either Scalar, Vector or Matrix");
}
/*****************************************************************************/

/*****************************************************************************/
std::string Printer::print_Outer( const Outer *o )
/*****************************************************************************/
{
	if (o==NULL) throw InternalError("Printer: Outer is NULL");
	BasicPtr mul = Mul::New( o->getArg1(), Transpose::New(o->getArg2()));
	return print(mul);
}
/*****************************************************************************/

/*****************************************************************************/
std::string Printer::print_Equal( const Equal *e )
/*****************************************************************************/
{
	if (e==NULL) throw InternalError("Printer: Equal is NULL");
    return "(" + print(e->getArg1()) + " == " + print(e->getArg2()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string Printer::print_Greater( const Greater *e )
/*****************************************************************************/
{
	if (e==NULL) throw InternalError("Printer: Greater is NULL");
	return "(" + print(e->getArg1()) + ">" + print(e->getArg2()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string Printer::print_Less( const Less *e )
/*****************************************************************************/
{
	if (e==NULL) throw InternalError("Printer: Less is NULL");
	return "(" + print(e->getArg1()) + "<" + print(e->getArg2()) + ")";
}
/*****************************************************************************/

/*****************************************************************************/
std::string Printer::error( std::string s )
/*****************************************************************************/
{
    //throw InternalError(s); // sinnvoller ist folgendes:
    m_errorcount++;
    std::cerr << "Error: " << s << std::endl;
	return "(Error: " + s + ")";
}
/*****************************************************************************/

/*****************************************************************************/
int Printer::getErrorcount(bool reset)
/*****************************************************************************/
{
    int c = m_errorcount;
    if (reset) 
        m_errorcount = 0;
    return c;
}
/*****************************************************************************/