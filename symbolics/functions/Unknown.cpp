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

#include "Unknown.h"
#include "Der.h"

using namespace Symbolics;

/*****************************************************************************/
Unknown::Unknown( std::string const& name, BasicPtrVec const& args  ): NaryOp(Type_Unknown, args), m_name(name)
/*****************************************************************************/
{
    // Alle Argumente den "Argumenten" hinzufuegen
    // Shape soll sich wie "+" verhalten, Irgendetwas muss man ja annehmen
	  m_shape = Shape();
    for(size_t i=0; i<getArgsSize(); ++i )
        m_shape += getArg(i)->getShape();
}
/*****************************************************************************/

/*****************************************************************************/
Unknown::~Unknown()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
std::string Unknown::toString() const
/*****************************************************************************/
{
#ifdef _DEBUG
    return "Unknown(" + join(",") + ")";
#else
    return "Unknown(" + join(" + ") + ")";
#endif
}
/*****************************************************************************/


/*****************************************************************************/
BasicPtr Unknown::simplify()
/*****************************************************************************/
{
    if (m_simplified)
        return BasicPtr(this);
    // simplify args
    simplifyArgs();
    m_simplified = true;
    return BasicPtr(this);
}
/*****************************************************************************/


/*****************************************************************************/
 BasicPtr Unknown::subs( ConstBasicPtr const& old_exp, 
                                   BasicPtr const& new_exp)
/*****************************************************************************/
{
    if (*this == *old_exp.get())
        return new_exp;

    // subs args
    BasicPtrVec subs_args;
    subs_args.reserve(getArgsSize());
    for (size_t i=0; i<getArgsSize(); ++i)
        subs_args.push_back(getArg(i)->subs(old_exp,new_exp));

    return BasicPtr( new Unknown(m_name, subs_args) );
}
/*****************************************************************************/


/*****************************************************************************/
 BasicPtr Unknown::iterateExp(Iterator &v)
/*****************************************************************************/
{
    // iterate args
    BasicPtrVec iter_args;
    iter_args.reserve(getArgsSize());
    for (size_t i=0; i<getArgsSize(); ++i)
    {
        iter_args.push_back(getArg(i)->iterateExp(v));
    }
    return v.process_Arg(BasicPtr(new Unknown(m_name, iter_args)));
}
/*****************************************************************************/

 /*****************************************************************************/
BasicPtr Unknown::New( std::string const& name, BasicPtrVec const& args )
/*****************************************************************************/
{
    return BasicPtr(new Unknown(name,args));
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Unknown::der()
/*****************************************************************************/
{
  return BasicPtr(new Der(BasicPtr(this)));
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Unknown::der(BasicPtr const& symbol)
/*****************************************************************************/
{
    BasicPtrVec args;
    args.reserve(getArgsSize());

    // Nun alles differenzieren
    for(size_t i=0; i<getArgsSize(); ++i)
      args.push_back(getArg(i)->der(symbol));

  return New(m_name,args);
}
/*****************************************************************************/