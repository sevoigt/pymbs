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

#include "NaryOp.h"
#include "Factory.h"

using namespace Symbolics;


/*****************************************************************************/
NaryOp::NaryOp( Basic_Type type,  BasicPtrVec const& args):
Basic(type)
/*****************************************************************************/
{
  reserveArgs(args.size());
	for(size_t i=0;i<args.size();++i)
  {
    addArg(args[i]);
  }
}
/*****************************************************************************/


/*****************************************************************************/
NaryOp::NaryOp( Basic_Type type, BasicPtr const& arg1, BasicPtr const& arg2 ):
Basic(type) 
/*****************************************************************************/
{
	// Args uebernehmen
	m_args.reserve(2);
  addArg(arg1);
  addArg(arg2);
}
/*****************************************************************************/


/*****************************************************************************/
NaryOp::~NaryOp()
/*****************************************************************************/
{
  for (size_t i=0;i<m_args.size();++i)
  {
    m_args[i].unregisterParent();
  }
   clearArgs();
}
/*****************************************************************************/

/*****************************************************************************/
std::string NaryOp::join( std::string const& sep) const
/*****************************************************************************/
{
    // Groesse
     size_t n = m_args.size();

    // Muss ein Zeichen eingefuegt werden
    if (n == 0)
        return "";
    if (n == 1)
        return getArg(0)->toString();

    // Zusammenbauen
    std::string s = getArg(0)->toString();
    for(size_t i=1; i<n; ++i)
    {
         s += sep + getArg(i)->toString();
    }

    // Fertig
    return s;
};
/*****************************************************************************/

/*****************************************************************************/
std::string NaryOp::join( std::string const& posSep,
                          std::string const& negSep) const
/*****************************************************************************/
{
    // Groesse
     size_t n = m_args.size();

    // Muss ein Zeichen eingefuegt werden
    if (n == 0)
        return "";
    if (n == 1)
        return getArg(0)->toString();

    // Zusammenbauen
    std::string s = getArg(0)->toString();
    for(size_t i=1; i<n; ++i)
    {
        // + oder - hinzufuegen
        if (getArg(i)->getType() == Type_Neg)
        {
            const Neg *neg = Util::getAsConstPtr<Neg>(getArg(i));
            s += negSep + neg->getArg()->toString();
        }
        else
          s += posSep + getArg(i)->toString();
    }

    // Fertig
    return s;
};
/*****************************************************************************/


/*****************************************************************************/
bool NaryOp::operator ==( Basic const& rhs) const
/*****************************************************************************/
{
    // Typen vergleichen
    if (getType() != rhs.getType()) 
      return false;

    // Vergleich Shape
    if (getShape() != rhs.getShape())
      return false;

    // Versuch zu casten
    const NaryOp *nary = dynamic_cast<const NaryOp*>(&rhs);
    if (nary == NULL) return false;

    // Anzahl der Argumente pruefen
    if (m_args.size() != nary->m_args.size()) return false;

    // Alle Argumente pruefen
    for (size_t i=0; i<m_args.size(); ++i)
        if (getArg(i) != nary->getArg(i)) return false;

    // Wenn er hier ankommt, dann sind sie wirklich gleich
    return true;
}
/*****************************************************************************/


/*****************************************************************************/
bool NaryOp::operator<(Basic  const& rhs) const
/*****************************************************************************/
{
    // Typen vergleichen
    if (getType() != rhs.getType())
        return getType() < rhs.getType();
    // Vergleich Shape
    if (getShape() != rhs.getShape())
        return (getShape() < rhs.getShape());
    // UnaryOp
    const NaryOp &naryop = dynamic_cast<const NaryOp &>(rhs);

    if (m_args.size() == naryop.m_args.size())
    {
        for (size_t i=0;i<m_args.size();++i)
        {
            BasicPtr rhsArg(naryop.getArg(i)); 
            // Args vergleichen
            if ((*getArg(i).get()) == (*rhsArg.get()))
                continue;
            return (*getArg(i).get()) < (*rhsArg.get());
        }
        // all args equal return false
        return false;
    }
    return m_args.size() < naryop.m_args.size();
}
/*****************************************************************************/

/*****************************************************************************/
void NaryOp::getAtoms(Basic::BasicSet& atoms)
/*****************************************************************************/
{
    for (size_t i=0; i<m_args.size(); ++i)
        getArg(i)->getAtoms(atoms);
}
/*****************************************************************************/

/*****************************************************************************/
void NaryOp::getAtoms(Basic::BasicSizeTMap& atoms)
/*****************************************************************************/
{
    for (size_t i=0; i<m_args.size(); ++i)
        getArg(i)->getAtoms(atoms);
}
/*****************************************************************************/


/*****************************************************************************/
 BasicPtr NaryOp::subs( ConstBasicPtr const& old_exp, 
                       BasicPtr const& new_exp)
/*****************************************************************************/
{
    if (*this == *old_exp.get())
        return new_exp;

    // subs args
    BasicPtrVec subs_args;
    subs_args.reserve( m_args.size() );
    for (size_t i=0; i<m_args.size(); ++i)
            subs_args.push_back(getArg(i)->subs(old_exp,new_exp));

    return Factory::newBasic( getType(), subs_args, getShape() );
}
/*****************************************************************************/

