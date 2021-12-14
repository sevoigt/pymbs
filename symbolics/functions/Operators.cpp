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

#include "Operators.h"
#include "Basic.h"
#include "Int.h"
#include "Symbol.h"
#include "Add.h"
#include "Mul.h"
#include "Neg.h"
#include "Util.h"
#include "Pow.h"

#include <cassert>

using namespace Symbolics;

/*****************************************************************************/
bool Symbolics::operator<(BasicPtr const& lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    // zuerst den Typ vergleichen
    // Ausnamen fuer Zero
    if ((lhs->getType() == Type_Zero) &&
        (rhs->getType() == Type_Real))
    {
        const Real *cd = Util::getAsConstPtr<Real>(rhs);
        return 0.0 < cd->getValue();
    }
    if ((lhs->getType() == Type_Real) &&
        (rhs->getType() == Type_Zero))
    {
        const Real *cd = Util::getAsConstPtr<Real>(lhs);
        return cd->getValue() < 0.0;
    }
    if ((lhs->getType() == Type_Zero) &&
        (rhs->getType() == Type_Int))
    {
        const Int *ci = Util::getAsConstPtr<Int>(rhs);
        return 0 < ci->getValue();
    }
    if ((lhs->getType() == Type_Int) &&
        (rhs->getType() == Type_Zero))
    {
        const Int *ci = Util::getAsConstPtr<Int>(lhs);
        return ci->getValue() < 0;
    }
    // Ausnahme fuer Int==Dbl und andersherum
    if ((lhs->getType() == Type_Int) &&
        (rhs->getType() == Type_Real))
    {
        const Int *ci = Util::getAsConstPtr<Int>(lhs);
        const Real *cd = Util::getAsConstPtr<Real>(rhs);
        return (*ci) < (*cd);
    }
    if ((lhs->getType() == Type_Real) &&
        (rhs->getType() == Type_Int))
    {
        const Real *cd = Util::getAsConstPtr<Real>(lhs);
        const Int *ci = Util::getAsConstPtr<Int>(rhs);
        return (*ci) < (*cd);
    }
    // Vergleich Type
    if (lhs->getType() != rhs->getType())
        return (lhs->getType() < rhs->getType());

    // Vergleich Shape
    if (lhs->getShape() != rhs->getShape())
        return (lhs->getShape() < rhs->getShape());

    // Nun als Basic vergleichen
    return (*lhs.get() < *rhs.get());
}
/*****************************************************************************/

/*****************************************************************************/
bool Symbolics::operator>(BasicPtr const& lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    // Vergleich
    if (lhs == rhs) return false;
    return rhs < lhs;
}
/*****************************************************************************/

#pragma region op==
/*****************************************************************************/
bool Symbolics::operator== (BasicPtr const& lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    // zuerst den Typ vergleichen
    // Ausnamen fuer Zero
    if (lhs->getType() == Type_Zero)
    {
        const Zero *ci = Util::getAsConstPtr<Zero>(lhs);
        return (*ci) == rhs;
    }
    if (rhs->getType() == Type_Zero)
    {
        const Zero *ci = Util::getAsConstPtr<Zero>(rhs);
        return (*ci) == lhs;
    }
    if ((lhs->getType() == Type_Real) &&
        (rhs->getType() == Type_Int))
    {
        const Real *cd = Util::getAsConstPtr<Real>(lhs);
        const Int *ci = Util::getAsConstPtr<Int>(rhs);
        return (*ci) == (*cd);
    }
    // Ausnahme fuer Int==Dbl und andersherum
    if ((lhs->getType() == Type_Int) &&
        (rhs->getType() == Type_Real))
    {
        const Int *ci = Util::getAsConstPtr<Int>(lhs);
        const Real *cd = Util::getAsConstPtr<Real>(rhs);
        return (*ci) == (*cd);
    }
    if ((lhs->getType() == Type_Real) &&
        (rhs->getType() == Type_Int))
    {
        const Real *cd = Util::getAsConstPtr<Real>(lhs);
        const Int *ci = Util::getAsConstPtr<Int>(rhs);
        return (*ci) == (*cd);
    }

    // falls dieser verschieden ist, aufgeben
    if (lhs->getType() != rhs->getType()) return false;
    // Als naechstes den Shape vergleichen
    if (lhs->getShape() != rhs->getShape()) return false;

    // Nun als Basic vergleichen
    return (*lhs.get() == *rhs.get());
}
/*****************************************************************************/

/*****************************************************************************/
bool Symbolics::operator== (double const& lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    // Nun als Basic vergleichen
    return BasicPtr(new Real(lhs)) == rhs;
}
/*****************************************************************************/
/*****************************************************************************/
bool Symbolics::operator== (BasicPtr const& lhs, double  const& rhs)
/*****************************************************************************/
{
    // Nun als Basic vergleichen
    return lhs == BasicPtr(new Real(rhs));
}
/*****************************************************************************/

/*****************************************************************************/
bool Symbolics::operator== (int const& lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    // Nun als Basic vergleichen
    return BasicPtr(new Int(lhs)) == rhs;
}
/*****************************************************************************/
/*****************************************************************************/
bool Symbolics::operator== (BasicPtr const& lhs, int  const& rhs)
/*****************************************************************************/
{
    // Nun als Basic vergleichen
    return lhs == BasicPtr(new Int(rhs));
}
/*****************************************************************************/
#pragma endregion

#pragma region op!=
/*****************************************************************************/
bool Symbolics::operator!= (BasicPtr const& lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    // Einfach den Vergleich negieren
    return !(lhs == rhs);
}
/*****************************************************************************/

/*****************************************************************************/
bool Symbolics::operator!= (double const& lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    // Nun als Basic vergleichen
    return !(BasicPtr(new Real(lhs)) == rhs);
}
/*****************************************************************************/
/*****************************************************************************/
bool Symbolics::operator!= (BasicPtr const& lhs, double  const& rhs)
/*****************************************************************************/
{
    // Nun als Basic vergleichen
    return !(lhs == BasicPtr(new Real(rhs)));
}
/*****************************************************************************/

/*****************************************************************************/
bool Symbolics::operator!= (int const& lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    // Nun als Basic vergleichen
    return !(BasicPtr(new Int(lhs)) == rhs);
}
/*****************************************************************************/
/*****************************************************************************/
bool Symbolics::operator!= (BasicPtr const& lhs, int  const& rhs)
/*****************************************************************************/
{
    // Nun als Basic vergleichen
    return !(lhs == BasicPtr(new Int(rhs)));
}
/*****************************************************************************/
#pragma endregion

#pragma region op+
/*****************************************************************************/
BasicPtr Symbolics::operator+ (BasicPtr const& lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    // neues Add erstellen
    return Add::New(lhs,rhs);
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Symbolics::operator+ (double lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    BasicPtr l( new Real(lhs) );
    return l + rhs;
}
/*****************************************************************************/
/*****************************************************************************/
BasicPtr Symbolics::operator+ (BasicPtr const& lhs, double rhs)
/*****************************************************************************/
{
    BasicPtr r( new Real(rhs) );
    return lhs + r;
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Symbolics::operator+ (int lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    BasicPtr l( new Int(lhs) );
    return l + rhs;
}
/*****************************************************************************/
/*****************************************************************************/
BasicPtr Symbolics::operator+ (BasicPtr const& lhs, int rhs)
/*****************************************************************************/
{
    BasicPtr r( new Int(rhs) );
    return lhs + r;
}
/*****************************************************************************/

#pragma endregion

#pragma region op-
/*****************************************************************************/
BasicPtr Symbolics::operator- (BasicPtr const& op)
/*****************************************************************************/
{
    // neues Neg erstellen
    return Neg::New(op);
}
/*****************************************************************************/
/*****************************************************************************/
BasicPtr Symbolics::operator- (BasicPtr const& lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    return Add::New(lhs,Neg::New(rhs));
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Symbolics::operator- (double lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    BasicPtr l( new Real(lhs) );
    return l - rhs;
}
/*****************************************************************************/
/*****************************************************************************/
BasicPtr Symbolics::operator- (BasicPtr const& lhs, double rhs)
/*****************************************************************************/
{
    BasicPtr r( new Real(rhs) );
    return lhs - r;
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Symbolics::operator- (int lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    BasicPtr l( new Int(lhs) );
    return l - rhs;
}
/*****************************************************************************/
/*****************************************************************************/
BasicPtr Symbolics::operator- (BasicPtr const& lhs, int rhs)
/*****************************************************************************/
{
    BasicPtr r( new Int(rhs) );
    return lhs - r;
}
/*****************************************************************************/
#pragma endregion

#pragma region op*
/*****************************************************************************/
BasicPtr Symbolics::operator* (BasicPtr const& lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    // neues Mul erstellen
    return Mul::New(lhs,rhs);
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Symbolics::operator* (double lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    BasicPtr l( new Real(lhs) );
    return l * rhs;
}
/*****************************************************************************/
/*****************************************************************************/
BasicPtr Symbolics::operator* (BasicPtr const& lhs, double rhs)
/*****************************************************************************/
{
    BasicPtr r( new Real(rhs) );
    return lhs * r;
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Symbolics::operator* (int lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    BasicPtr l( new Int(lhs) );
    return l * rhs;
}
/*****************************************************************************/
/*****************************************************************************/
BasicPtr Symbolics::operator* (BasicPtr const& lhs, int rhs)
/*****************************************************************************/
{
    BasicPtr r( new Int(rhs) );
    return lhs * r;
}
/*****************************************************************************/
#pragma endregion
