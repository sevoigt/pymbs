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

#include "Real.h"
#include "Bool.h"
#include "Util.h"
#include "Operators.h"
#include "Matrix.h"

using namespace Symbolics;

/*****************************************************************************/
BasicPtr Real::getArgres;
/*****************************************************************************/

/*****************************************************************************/
Real::Real( const double value): Basic(Type_Real), m_value(value)
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
Real::Real( const Int i ): Basic(Type_Real),
m_value(static_cast<double>(i.getValue()))
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
Real::~Real()
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
 BasicPtr Real::iterateExp(Symbolics::Basic::Iterator &v)
/*****************************************************************************/
{
    return v.process_Arg( BasicPtr(this) );
}
/*****************************************************************************/

/*****************************************************************************/
bool Real::operator==(  double const& rhs ) const
/*****************************************************************************/
{
    return m_value == rhs;
}
/*****************************************************************************/

/*****************************************************************************/
bool Real::operator==(  Real const& rhs ) const
/*****************************************************************************/
{
    return m_value == rhs.getValue();
}
/*****************************************************************************/

/*****************************************************************************/
bool Real::operator==(  Basic const& rhs ) const
/*****************************************************************************/
{
    // Typ pruefen
    switch (rhs.getType())
    {
    case Type_Zero:
        {
            if (m_shape != rhs.getShape()) return true;
            return m_value == 0.0;
        }
    case Type_Bool:
        {
            const Bool &b = dynamic_cast<const Bool&>(rhs);
            return (m_value==0.0?false:true) == b.getValue();
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
bool Real::operator!=(  double const& rhs ) const
/*****************************************************************************/
{
    return m_value != rhs;
}
/*****************************************************************************/

/*****************************************************************************/
bool Real::operator!=(  Real const& rhs ) const
/*****************************************************************************/
{
    return m_value != rhs.getValue();
}
/*****************************************************************************/

/*****************************************************************************/
bool Real::operator!=(  Basic const& rhs ) const
/*****************************************************************************/
{
    // Typ pruefen
    switch (rhs.getType())
    {
    case Type_Zero:
        {
            if (m_shape != rhs.getShape()) return true;
            return m_value != 0.0;
        }
    case Type_Bool:
        {
            const Bool &b = dynamic_cast<const Bool&>(rhs);
            return (m_value==0.0?false:true) != b.getValue();
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
bool Real::operator<(  Basic const& rhs ) const
/*****************************************************************************/
{
    // Typ pruefen
    switch (rhs.getType())
    {
    case Type_Zero:
        {
            return m_value < 0.0; 
        }
    case Type_Bool:
        {
            const Bool &b = dynamic_cast<const Bool&>(rhs);
            return (m_value==0.0?false:true) < b.getValue();
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
Real Real::operator+ ( Int const& rhs)
/*****************************************************************************/
{
    return Real(m_value+static_cast<double>(rhs.getValue()));
}
/*****************************************************************************/

/*****************************************************************************/
Real Real::operator- ( Int const& rhs)
/*****************************************************************************/
{
    return Real(m_value-static_cast<double>(rhs.getValue()));
}
/*****************************************************************************/

/*****************************************************************************/
Real Real::operator* ( Int const& rhs)
/*****************************************************************************/
{
    return Real(m_value*static_cast<double>(rhs.getValue()));
}
/*****************************************************************************/

/*****************************************************************************/
Real Real::operator+ ( Real const& rhs)
/*****************************************************************************/
{
    return Real(m_value+rhs.getValue());
}
/*****************************************************************************/

/*****************************************************************************/
Real Real::operator- ( Real const& rhs)
/*****************************************************************************/
{
    return Real(m_value-rhs.getValue());
}
/*****************************************************************************/

/*****************************************************************************/
Real Real::operator* ( Real const& rhs)
/*****************************************************************************/
{
    return Real(m_value*rhs.getValue());
}
/*****************************************************************************/

/*****************************************************************************/
Real Real::operator+ ( int const& rhs)
/*****************************************************************************/
{
    return Real(m_value+static_cast<double>(rhs));
}
/*****************************************************************************/

/*****************************************************************************/
Real Real::operator- ( int const& rhs)
/*****************************************************************************/
{
    return Real(m_value-static_cast<double>(rhs));
}
/*****************************************************************************/

/*****************************************************************************/
Real Real::operator* ( int const& rhs)
/*****************************************************************************/
{
    return Real(m_value*static_cast<double>(rhs));
}
/*****************************************************************************/

/*****************************************************************************/
Real Real::operator+ ( double const& rhs)
/*****************************************************************************/
{
    return Real(m_value+rhs);
}
/*****************************************************************************/

/*****************************************************************************/
Real Real::operator- ( double const& rhs)
/*****************************************************************************/
{
    return Real(m_value-rhs);
}
/*****************************************************************************/

/*****************************************************************************/
Real Real::operator* ( double const& rhs)
/*****************************************************************************/
{
    return Real(m_value*rhs);
}
/*****************************************************************************/

/*****************************************************************************/
 BasicPtr Real::subs( ConstBasicPtr const& old_exp, BasicPtr const& new_exp)
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
bool Symbolics::operator== (Real const& lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    if (rhs->getType() == Type_Real)
    {
        const Real *crhs = Util::getAsConstPtr<Real>(rhs);
        return lhs.getValue() == (*crhs).getValue();

    }
    return false;
}
/*****************************************************************************/
/*****************************************************************************/
bool Symbolics::operator== (BasicPtr const& lhs, Real  const& rhs)
/*****************************************************************************/
{
    if (lhs->getType() == Type_Real)
    {
        const Real *clhs = Util::getAsConstPtr<Real>(lhs);
        return (*clhs).getValue() == rhs.getValue();

    }
    return false;
}
/*****************************************************************************/


/*****************************************************************************/
bool Symbolics::operator!= (Real const& lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    if (rhs->getType() == Type_Real)
    {
        const Real *crhs = Util::getAsConstPtr<Real>(rhs);
        return lhs.getValue() != (*crhs).getValue();

    }
    return false;
}
/*****************************************************************************/
/*****************************************************************************/
bool Symbolics::operator!= (BasicPtr const& lhs, Real  const& rhs)
/*****************************************************************************/
{
    if (lhs->getType() == Type_Real)
    {
        const Real *clhs = Util::getAsConstPtr<Real>(lhs);
        return (*clhs).getValue() != rhs.getValue();

    }
    return false;
}
/*****************************************************************************/


/*****************************************************************************/
BasicPtr Symbolics::operator+ (Real lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    BasicPtr l( new Real(lhs) );
    return l + rhs;
}
/*****************************************************************************/
/*****************************************************************************/
BasicPtr Symbolics::operator+ (BasicPtr const& lhs, Real rhs)
/*****************************************************************************/
{
    BasicPtr r( new Real(rhs) );
    return lhs + r;
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Symbolics::operator- (Real lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    BasicPtr l( new Real(lhs) );
    return l - rhs;
}
/*****************************************************************************/
/*****************************************************************************/
BasicPtr Symbolics::operator- (BasicPtr const& lhs, Real rhs)
/*****************************************************************************/
{
    BasicPtr r( new Real(rhs) );
    return lhs - r;
}
/*****************************************************************************/


/*****************************************************************************/
BasicPtr Symbolics::operator* (Real lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    BasicPtr l( new Real(lhs) );
    return l * rhs;
}
/*****************************************************************************/
/*****************************************************************************/
BasicPtr Symbolics::operator* (BasicPtr const& lhs, Real rhs)
/*****************************************************************************/
{
    BasicPtr r( new Real(rhs) );
    return lhs * r;
}
/*****************************************************************************/