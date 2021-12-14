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

#include "Zero.h"
#include "Int.h"
#include "Real.h"
#include "Bool.h"
#include "Util.h"
#include "Operators.h"
#include "Matrix.h"

using namespace Symbolics;

/*****************************************************************************/
BasicPtr Zero::zero = BasicPtr( new Zero() );
BasicPtr Zero::getArgres;
/*****************************************************************************/

/*****************************************************************************/
Zero::Zero(): Basic(Type_Zero)
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
Zero::Zero(Shape const& s): Basic(Type_Zero,s)
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
Zero::~Zero()
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
 BasicPtr Zero::iterateExp(Symbolics::Basic::Iterator &v)
/*****************************************************************************/
{
    return v.process_Arg( BasicPtr(this) );
}
/*****************************************************************************/

/*****************************************************************************/
bool Zero::operator==(  Basic const& rhs ) const
/*****************************************************************************/
{
    // Typ pruefen
    switch (rhs.getType())
    {
    case Type_Zero: 
        return true;
    case Type_Bool:
        {
            const Bool &b = dynamic_cast<const Bool&>(rhs);
            return false == b.getValue();
        }
    case Type_Int:
        {
            const Int &i = dynamic_cast<const Int&>(rhs);
            return 0 == i.getValue();
        }
    case Type_Real:
        {
            const Real &r = dynamic_cast<const Real&>(rhs);
            return 0 == r.getValue();
        }
    case Type_Matrix:
        {
            size_t argssize = rhs.getArgsSize();
            for (size_t i=0;i<argssize;++i)
              if (!(rhs.getArg(i) == 0))
                    return false;
            return true;
        }
    default:
        return false;
    }
}
/*****************************************************************************/
 
/*****************************************************************************/
 BasicPtr Zero::subs( ConstBasicPtr const& old_exp, BasicPtr const& new_exp)
/*****************************************************************************/
{
    // Sind wir es selbst, die ersetzt werden sollen?
    if (*this == *old_exp.get())
        return new_exp;

    // uns selbs zurueckgeben
    return BasicPtr(this);
}
/*****************************************************************************/


/*****************************************************************************/
BasicPtr Zero::getExactZero(Shape const& s)
/*****************************************************************************/
{
	if (s.getNrDimensions() == 0)
        return zero;
    return BasicPtr(new Zero(s));
}
/*****************************************************************************/


/*****************************************************************************/
BasicPtr Zero::getZero(Shape const& s)
/*****************************************************************************/
{
    if (s.getNumEl() == 1)
        return zero;
    return BasicPtr(new Zero(s));
}
/*****************************************************************************/


/*****************************************************************************/
bool Symbolics::operator== (Zero const& lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    if (rhs->getType() == Type_Zero)
    {
        return lhs.getShape() == rhs->getShape();

    }
    if (rhs->getType() == Type_Int)
    {
        const Int *crhs = Util::getAsConstPtr<Int>(rhs);
        return 0 == (*crhs).getValue();

    }
    if (rhs->getType() == Type_Real)
    {
        const Real *crhs = Util::getAsConstPtr<Real>(rhs);
        return 0 == (*crhs).getValue();

    }
    if (rhs->getType() == Type_Matrix)
    {
        size_t argssize = rhs->getArgsSize();
        for (size_t i=0;i<argssize;++i)
          if ((rhs->getArg(i) != Zero::getZero()))
                return false;
        return true;
    }
    return false;
}
/*****************************************************************************/
/*****************************************************************************/
bool Symbolics::operator== (BasicPtr const& lhs, Zero  const& rhs)
/*****************************************************************************/
{
    if (lhs->getType() == Type_Zero)
    {
        return lhs->getShape() == rhs.getShape();

    }
    if (lhs->getType() == Type_Int)
    {
        const Int *clhs = Util::getAsConstPtr<Int>(lhs);
        return 0 == (*clhs).getValue();

    }
    if (lhs->getType() == Type_Real)
    {
        const Real *clhs = Util::getAsConstPtr<Real>(lhs);
        return 0 == (*clhs).getValue();

    }
    if (lhs->getType() == Type_Matrix)
    {
        size_t argssize = lhs->getArgsSize();
        for (size_t i=0;i<argssize;++i)
          if ((lhs->getArg(i) != Zero::getZero()))
                return false;
        return true;
    }
    return false;
}
/*****************************************************************************/

/*****************************************************************************/
bool Symbolics::operator!= (Zero const& lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    if (rhs->getType() == Type_Zero)
    {
        return lhs.getShape() != rhs->getShape();
    }
    if (rhs->getType() == Type_Int)
    {
        const Int *crhs = Util::getAsConstPtr<Int>(rhs);
        return 0 != (*crhs).getValue();

    }
    if (rhs->getType() == Type_Real)
    {
        const Real *crhs = Util::getAsConstPtr<Real>(rhs);
        return 0 != (*crhs).getValue();

    }
    if (rhs->getType() == Type_Matrix)
    {
        size_t argssize = rhs->getArgsSize();
        for (size_t i=0;i<argssize;++i)
          if ((rhs->getArg(i) != Zero::getZero()))
                return true;
        return false;
    }
    return true;
}
/*****************************************************************************/
/*****************************************************************************/
bool Symbolics::operator!= (BasicPtr const& lhs, Zero  const& rhs)
/*****************************************************************************/
{
    if (lhs->getType() == Type_Zero)
    {
        return lhs->getShape() != rhs.getShape();;

    }
    if (lhs->getType() == Type_Int)
    {
        const Int *clhs = Util::getAsConstPtr<Int>(lhs);
        return 0 != (*clhs).getValue();

    }
    if (lhs->getType() == Type_Real)
    {
        const Real *clhs = Util::getAsConstPtr<Real>(lhs);
        return 0 != (*clhs).getValue();

    }
    if (lhs->getType() == Type_Matrix)
    {
        size_t argssize = lhs->getArgsSize();
        for (size_t i=0;i<argssize;++i)
          if ((lhs->getArg(i) != Zero::getZero()))
                return true;
        return false;
    }
    return true;
}
/*****************************************************************************/
