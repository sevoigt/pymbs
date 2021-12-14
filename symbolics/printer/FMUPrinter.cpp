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

#include "FMUPrinter.h"
#include "str.h"

using namespace Symbolics;

/*****************************************************************************/
FMUPrinter::FMUPrinter(const std::map<std::string, int> *valueReferences)
/*****************************************************************************/
{
	m_valueReferencesP=valueReferences;
}
/*****************************************************************************/


/*****************************************************************************/
FMUPrinter::~FMUPrinter()
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
std::string FMUPrinter::print_Symbol( const Symbol *symbol )
/*****************************************************************************/
{
	if (symbol == NULL) throw InternalError("FMUPrinter: Symbol is NULL");
	std::string name = symbol->getName();
//	std::map<std::string,int>::const_iterator it = m_valueReferencesP->find(name);
//	if (it!=m_valueReferencesP->end())
    if (name == "time")
        return "comp->time";
	if (m_valueReferencesP->count(name))
		return "r(" + name + "_)";
	return name;
}
/*****************************************************************************/

/*****************************************************************************/
std::string FMUPrinter::print_Element( const Element *e )
/*****************************************************************************/
{ 
	if (e == NULL) throw InternalError("FMUPrinter: Element is NULL");
	BasicPtr arg = e->getArg(0);
	if (arg->is_Scalar()) //Ja - im FMUWriter werden skalare Elemente erstellt...
		return print(arg);
	if (arg->getType() == Type_Symbol) 
	{
		const Symbol *s = Util::getAsConstPtr<Symbol>(arg);
		std::string name;
		if (arg->is_Vector())
			name =  s->getName() + "_" + (e->getRow() > 0  ? str(e->getRow()) : str(e->getCol()));
		else
			name =  s->getName() + "_" + str(e->getRow()) + "_" + str(e->getCol());
		if (m_valueReferencesP->count(name))
			return "r(" + name + "_)";
	}

	if (arg->is_Vector())
	{
		if (e->getRow() > 0)
			return print(arg) + "[" + str(e->getRow()) + "]";
		else
			return print(arg) + "[" + str(e->getCol()) + "]";
	}
	return print(arg) + "[" + str(e->getRow()) + "][" + str(e->getCol()) + "]";
}
/*****************************************************************************/

/*****************************************************************************/
std::string FMUPrinter::print_Der( const Der *d )
/*****************************************************************************/
{
	if (d == NULL) throw InternalError("FMUPrinter: Der is NULL");
	std::string args = print(d->getArg());
	if (args.find("r(") == 0) //String beginnt mit "r(" (gefunden an Position 0)
		args.insert(2, "der_");
	else
		args = "der_" + args;
	return args;
}
/*****************************************************************************/

/*****************************************************************************/
std::string FMUPrinter::print_Bool( const Bool *b )
/*****************************************************************************/
{
    if (b == NULL) throw InternalError("FMUPrinter: Bool is NULL");
    if (b->getValue())
        return "fmiTrue";
    else
        return "fmiFalse";
}
/*****************************************************************************/
