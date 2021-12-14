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

#include "Int.h"
#include "Bool.h"
#include "Util.h"
#include "Operators.h"
#include "Matrix.h"

using namespace Symbolics;

/*****************************************************************************/
 BasicPtr Int::one = BasicPtr( new Int(1) );
 BasicPtr Int::minusOne = BasicPtr( new Int(-1) );
 BasicPtr Int::getArgres;
/*****************************************************************************/


/*****************************************************************************/
Int::Int( const int value): Basic(Type_Int), m_value(value)
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
Int::~Int()
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Int::New( const int i)
/*****************************************************************************/
{
    switch (i)
    {
    case -1:
      return Int::getMinusOne();
    case 0:
        return Zero::getZero();
    case 1:
        return Int::getOne();
    default:
        return BasicPtr(new Int(i));
    }
}
/*****************************************************************************/

/*****************************************************************************/
 BasicPtr Int::iterateExp(Symbolics::Basic::Iterator &v)
/*****************************************************************************/
{
    return v.process_Arg( BasicPtr(this) );
}
/*****************************************************************************/

/*****************************************************************************/
bool Int::operator==(  int const& rhs ) const
/*****************************************************************************/
{
    return m_value == rhs;
}
/*****************************************************************************/

/*****************************************************************************/
bool Int::operator==(  Int const& rhs ) const
/*****************************************************************************/
{
    return m_value == rhs.getValue();
}
/*****************************************************************************/

/*****************************************************************************/
bool Int::operator==(  Basic const& rhs ) const
/*****************************************************************************/
{
    // Typ pruefen
    switch (rhs.getType())
    {
    case Type_Zero:
        {
            if (m_shape != rhs.getShape()) return true;
            return m_value == 0;
        }
    case Type_Bool:
        {
            const Bool &b = dynamic_cast<const Bool&>(rhs);
            return (m_value==0?false:true) == b.getValue();
        }
    case Type_Int:
        {
            const Int &i = dynamic_cast<const Int&>(rhs);
            return m_value == i.getValue();
        }
    case Type_Real:
        {
            const Real &r = dynamic_cast<const Real&>(rhs);
            return m_value == r.getValue();
        }
    default:
        return false;
    }
}
/*****************************************************************************/

/*****************************************************************************/
bool Int::operator!=(  int const& rhs ) const
/*****************************************************************************/
{
    return m_value != rhs;
}
/*****************************************************************************/

/*****************************************************************************/
bool Int::operator!=(  Int const& rhs ) const
/*****************************************************************************/
{
    return m_value != rhs.getValue();
}
/*****************************************************************************/

/*****************************************************************************/
bool Int::operator!=(  Basic const& rhs ) const
/*****************************************************************************/
{
    // Typ pruefen
    switch (rhs.getType())
    {
    case Type_Zero:
        {
            if (m_shape != rhs.getShape()) return true;
            return m_value != 0;
        }
    case Type_Bool:
        {
            const Bool &b = dynamic_cast<const Bool&>(rhs);
            return (m_value==0?false:true) != b.getValue();
        }
    case Type_Int:
        {
            const Int &i = dynamic_cast<const Int&>(rhs);
            return m_value != i.getValue();
        }
    case Type_Real:
        {
            const Real &r = dynamic_cast<const Real&>(rhs);
            return m_value != r.getValue();
        }
    default:
        return true;
    }
}
/*****************************************************************************/

/*****************************************************************************/
bool Int::operator<(  Basic const& rhs ) const
/*****************************************************************************/
{
    // Typ pruefen
    switch (rhs.getType())
    {
    case Type_Zero:
        {
            return m_value < 0;
        }
    case Type_Bool:
        {
            const Bool &b = dynamic_cast<const Bool&>(rhs);
            return (m_value==0?false:true) < b.getValue();
        }
    case Type_Int:
        {
            const Int &i = dynamic_cast<const Int&>(rhs);
            return m_value < i.getValue();
        }
    case Type_Real:
        {
            const Real &r = dynamic_cast<const Real&>(rhs);
            return m_value < r.getValue();
        }
    default:
        return getType() < rhs.getType();
    }
}
/*****************************************************************************/

/*****************************************************************************/
Int Int::operator+ ( Int const& rhs)
/*****************************************************************************/
{
    return Int(m_value+rhs.getValue());
}
/*****************************************************************************/

/*****************************************************************************/
Int Int::operator- ( Int const& rhs)
/*****************************************************************************/
{
    return Int(m_value-rhs.getValue());
}
/*****************************************************************************/

/*****************************************************************************/
Int Int::operator* ( Int const& rhs)
/*****************************************************************************/
{
    return Int(m_value*rhs.getValue());
}
/*****************************************************************************/

/*****************************************************************************/
Real Int::operator+ ( Real const& rhs)
/*****************************************************************************/
{
    return Real(static_cast<double>(m_value)+rhs.getValue());
}
/*****************************************************************************/

/*****************************************************************************/
Real Int::operator- ( Real const& rhs)
/*****************************************************************************/
{
    return Real(static_cast<double>(m_value)-rhs.getValue());
}
/*****************************************************************************/

/*****************************************************************************/
Real Int::operator* ( Real const& rhs)
/*****************************************************************************/
{
    return Real(static_cast<double>(m_value)*rhs.getValue());
}
/*****************************************************************************/

/*****************************************************************************/
Int Int::operator+ ( int const& rhs)
/*****************************************************************************/
{
    return Int(m_value+rhs);
}
/*****************************************************************************/

/*****************************************************************************/
Int Int::operator- ( int const& rhs)
/*****************************************************************************/
{
    return Int(m_value-rhs);
}
/*****************************************************************************/

/*****************************************************************************/
Int Int::operator* ( int const& rhs)
/*****************************************************************************/
{
    return Int(m_value*rhs);
}
/*****************************************************************************/

/*****************************************************************************/
Real Int::operator+ ( double const& rhs)
/*****************************************************************************/
{
    return Real(static_cast<double>(m_value)+rhs);
}
/*****************************************************************************/

/*****************************************************************************/
Real Int::operator- ( double const& rhs)
/*****************************************************************************/
{
    return Real(static_cast<double>(m_value)-rhs);
}
/*****************************************************************************/

/*****************************************************************************/
Real Int::operator* ( double const& rhs)
/*****************************************************************************/
{
    return Real(static_cast<double>(m_value)*rhs);
}
/*****************************************************************************/

/*****************************************************************************/
 BasicPtr Int::subs( ConstBasicPtr const& old_exp, BasicPtr const& new_exp)
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
bool Symbolics::operator== (Int const& lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    if (rhs->getType() == Type_Int)
    {
        const Int *crhs = Util::getAsConstPtr<Int>(rhs);
        return lhs.getValue() == (*crhs).getValue();

    }
    return false;
}
/*****************************************************************************/
/*****************************************************************************/
bool Symbolics::operator== (BasicPtr const& lhs, Int  const& rhs)
/*****************************************************************************/
{
    if (lhs->getType() == Type_Int)
    {
        const Int *clhs = Util::getAsConstPtr<Int>(lhs);
        return (*clhs).getValue() == rhs.getValue();

    }
    return false;
}
/*****************************************************************************/

/*****************************************************************************/
bool Symbolics::operator!= (Int const& lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    if (rhs->getType() == Type_Int)
    {
        const Int *crhs = Util::getAsConstPtr<Int>(rhs);
        return lhs.getValue() != (*crhs).getValue();

    }
    return false;
}
/*****************************************************************************/
/*****************************************************************************/
bool Symbolics::operator!= (BasicPtr const& lhs, Int  const& rhs)
/*****************************************************************************/
{
    if (lhs->getType() == Type_Int)
    {
        const Int *clhs = Util::getAsConstPtr<Int>(lhs);
        return (*clhs).getValue() != rhs.getValue();

    }
    return false;
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Symbolics::operator+ (Int lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    BasicPtr l( new Int(lhs) );
    return l + rhs;
}
/*****************************************************************************/
/*****************************************************************************/
BasicPtr Symbolics::operator+ (BasicPtr const& lhs, Int rhs)
/*****************************************************************************/
{
    BasicPtr r( new Int(rhs) );
    return lhs + r;
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Symbolics::operator- (Int lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    BasicPtr l( new Int(lhs) );
    return l - rhs;
}
/*****************************************************************************/
/*****************************************************************************/
BasicPtr Symbolics::operator- (BasicPtr const& lhs, Int rhs)
/*****************************************************************************/
{
    BasicPtr r( new Int(rhs) );
    return lhs - r;
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Symbolics::operator* (Int lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    BasicPtr l( new Int(lhs) );
    return l * rhs;
}
/*****************************************************************************/
/*****************************************************************************/
BasicPtr Symbolics::operator* (BasicPtr const& lhs, Int rhs)
/*****************************************************************************/
{
    BasicPtr r( new Int(rhs) );
    return lhs * r;
}
/*****************************************************************************/

