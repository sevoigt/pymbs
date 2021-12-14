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

#include <stdarg.h>
#include "Solve.h"
#include "Pow.h"
#include "Int.h"
#include "Neg.h"
#include "Mul.h"
#include "Util.h"

using namespace Symbolics;

/*****************************************************************************/
Solve::Solve( BasicPtrVec const& args ): BinaryOp(Type_Solve, args)
/*****************************************************************************/
{
	validate();
}
/*****************************************************************************/


/*****************************************************************************/
Solve::Solve( BasicPtr const& arg1, BasicPtr const& arg2 ): BinaryOp(Type_Solve, arg1, arg2)
/*****************************************************************************/
{
	validate();
}
/*****************************************************************************/


/*****************************************************************************/
Solve::~Solve()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
void Solve::validate()
/*****************************************************************************/
{
    Shape s1 = getArg1()->getShape();
    Shape s2 = getArg2()->getShape();
    // arg1 Matrix or Scalar
    if (s1.getDimension(1) != s1.getDimension(2))
        throw ShapeError("Solve is only defined for square matrix as first argument!");
    // arg2 Vector or Scalar
    if (s2.getDimension(2) != 1)
    {
         std::string msg = "Solve is only defined for vectors as second argument! Got " + s2.toString() + " instead.";
        throw ShapeError(msg.c_str());
    }
    // arg1 equal dim arg2
    if (s1.getDimension(1) != s2.getDimension(1))
    {
         std::string msg = "Solve: Dimension of Matrix " + s1.toString() + "does not fit dimension of Vector" + s2.toString() + "!" + getArg1()->toString() + "  " + getArg2()->toString();
        throw ShapeError(msg.c_str());
    }
    // Shape berechnen
    m_shape = Shape(s1.getDimension(2));
}
/*****************************************************************************/


/*****************************************************************************/
std::string Solve::toString() const
/*****************************************************************************/
{
#ifdef _DEBUG
    return "Solve(" + getArg1()->toString() + "," + getArg2()->toString() + ")";
#else
    return "(" + getArg1()->toString() + "\\" + getArg2()->toString() + ")";
#endif
}
/*****************************************************************************/


/*****************************************************************************/
BasicPtr Solve::simplify()
/*****************************************************************************/
{
    if (m_simplified)
        return BasicPtr(this);
    // Argument vereinfachen
    simplifyArgs();

    // b is zero
    if (Util::is_Zero(getArg2()))
    {
        return getArg2();
    }
    // A is One
    if (Util::is_One(getArg1()))
    {
        return getArg2();
    }
    // Scalar
    if ((getArg1()->is_Scalar()) && (getArg2()->is_Scalar()))
    {
        return Util::div(getArg2(),getArg1());
    }
    m_simplified = true;
    return BasicPtr(this);
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Solve::New( BasicPtr const& arg1, BasicPtr const& arg2)
/*****************************************************************************/
{
    // b is zero
    if (Util::is_Zero(arg2))
    {
        return arg2;
    }
    // A is One
    if (Util::is_One(arg1))
    {
        return arg2;
    }
    // Scalar
    if ((arg1->is_Scalar()) && (arg2->is_Scalar()))
    {
        return Util::div(arg2,arg1);
    }
    return BasicPtr(new Solve(arg1,arg2));
}
/*****************************************************************************/

/*****************************************************************************/
inline BasicPtr Solve::der()
/*****************************************************************************/
{
  // der(solve(A,b)) = solve(A,der(b)-der(A)*solve(A,b))
  return New(getArg1(),getArg2()->der()-Mul::New(getArg1()->der(),BasicPtr(this)));
}
/*****************************************************************************/

/*****************************************************************************/
inline BasicPtr Solve::der(BasicPtr const& symbol)
/*****************************************************************************/
{
  // der(solve(A,b)) = solve(A,der(b)-der(A)*solve(A,b))
  return New(getArg1(),getArg2()->der(symbol)-Mul::New(getArg1()->der(symbol),BasicPtr(this)));
}
/*****************************************************************************/