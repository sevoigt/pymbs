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

#include "BinaryOp.h"
#include "str.h"
#include "Factory.h"

using namespace Symbolics;

/*****************************************************************************/
BasicPtr BinaryOp::getArgres;
/*****************************************************************************/

/*****************************************************************************/
BinaryOp::BinaryOp( Basic_Type type,  BasicPtrVec const& args): Basic(type)
/*****************************************************************************/
{
    m_arg1.registerParent(this);
    m_arg2.registerParent(this);
    // args pruefen
    if (args.size() != 2) throw InternalError("BinaryOp: Args must be of length 2, not " + str(args.size()) + "!");
    // args uebernehmen
    setArg1(args[0]);
    setArg2(args[1]);
}
/*****************************************************************************/


/*****************************************************************************/
BinaryOp::BinaryOp( Basic_Type type, BasicPtr const& arg1, BasicPtr const& arg2):
Basic(type)
/*****************************************************************************/
{
    m_arg1.registerParent(this);
    m_arg2.registerParent(this);
	// Args uebernehmen
    setArg1(arg1);
    setArg2(arg2);
}
/*****************************************************************************/


/*****************************************************************************/
BinaryOp::~BinaryOp()
/*****************************************************************************/
{
  m_arg1.unregisterParent();
  m_arg2.unregisterParent();
}
/*****************************************************************************/


/*****************************************************************************/
bool BinaryOp::operator ==( Basic const& rhs) const
/*****************************************************************************/
{
    // Typen vergleichen
    if (getType() != rhs.getType()) 
      return false;

    // Vergleich Shape
    if (getShape() != rhs.getShape())
      return false;

    // Versuch zu casten
    const BinaryOp *binary = dynamic_cast<const BinaryOp*>(&rhs);
    if (binary == NULL) return false;
        
    return ((getArg1() == binary->getArg1()) && (getArg2() == binary->getArg2()));
}
/*****************************************************************************/


/*****************************************************************************/
bool BinaryOp::operator<(Basic const& rhs) const
/*****************************************************************************/
{
    // Typen vergleichen
    if (getType() != rhs.getType())
        return getType() < rhs.getType();

    // Vergleich Shape
    if (getShape() != rhs.getShape())
        return (getShape() < rhs.getShape());

    // UnaryOp
    const BinaryOp &binaryop = dynamic_cast<const BinaryOp &>(rhs);
    BasicPtr rhsArg1(binaryop.getArg1()); 
    BasicPtr rhsArg2(binaryop.getArg2()); 

    // Args vergleichen
    if ((*getArg1().get()) == (*rhsArg1.get()))
        return (*getArg2().get()) < (*rhsArg2.get());
    return (*getArg1().get()) < (*rhsArg1.get());
}
/*****************************************************************************/

/*****************************************************************************/
void BinaryOp::getAtoms(Basic::BasicSet& atoms)
/*****************************************************************************/
{
    getArg1()->getAtoms(atoms);
    getArg2()->getAtoms(atoms);
}
/*****************************************************************************/

/*****************************************************************************/
void BinaryOp::getAtoms(Basic::BasicSizeTMap& atoms)
/*****************************************************************************/
{
    getArg1()->getAtoms(atoms);
    getArg2()->getAtoms(atoms);
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr BinaryOp::subs( ConstBasicPtr const& old_exp, 
                       BasicPtr const& new_exp)
/*****************************************************************************/
{
    if (*this == *old_exp.get())
        return new_exp;

    // subs args
    BasicPtr subs_arg1 = getArg1()->subs(old_exp,new_exp);
    BasicPtr subs_arg2 = getArg2()->subs(old_exp,new_exp);
    return Factory::newBasic( getType(), subs_arg1, subs_arg2, getShape() );
}
/*****************************************************************************/

