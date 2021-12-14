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
#include "Scalar.h"
#include "Neg.h"
#include "Util.h"
#include "Matrix.h"
#include "Int.h"

using namespace Symbolics;


/*****************************************************************************/
Scalar::Scalar( BasicPtrVec const& args ): UnaryOp(Type_Scalar, args)
/*****************************************************************************/
{
	validate();
}
/*****************************************************************************/


/*****************************************************************************/
Scalar::Scalar( BasicPtr const& arg ): UnaryOp(Type_Scalar, arg)
/*****************************************************************************/
{
	validate();
}
/*****************************************************************************/


/*****************************************************************************/
Scalar::~Scalar()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
void Scalar::validate()
/*****************************************************************************/
{
	// Scalar ist immer ein Skalar
    m_shape = Shape();

    // Argument muss skalar sein!
    if ((getArg()->getShape().getDimension(1) != 1) ||
        (getArg()->getShape().getDimension(2) != 1)) 
    {
         std::string msg = "Internal Error: Scalar - Argument must be (1,1) but is " + getArg()->getShape().toString() + "!";
        throw ShapeError(msg.c_str());
    }
}
/*****************************************************************************/


/*****************************************************************************/
std::string Scalar::toString() const
/*****************************************************************************/
{
#ifdef _DEBUG
    return "Scalar(" + getArg()->toString() + ")";
#else
    return "Scalar(" + getArg()->toString() + ")";
#endif
}
/*****************************************************************************/


/*****************************************************************************/
BasicPtr Scalar::simplify()
/*****************************************************************************/
{
    if (m_simplified)
        return BasicPtr(this);
    // Argument vereinfachen
    simplifyArg();

    // Matrix, dann 1. Element holen
    switch (getArg()->getType())
    {
    case Type_Matrix:
        {
            const Matrix *mat = Util::getAsConstPtr<Matrix>(getArg());
            return (*mat)(0,0);
        }
    case Type_Neg:
        {
            const Neg *neg = Util::getAsConstPtr<Neg>(getArg());
            return Neg::New( New(neg->getArg()) );
        }
    }

    // Skalar, dann nichts tun
    if (getArg()->is_Scalar())
        return getArg();

    m_simplified = true;
    return BasicPtr(this);
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Scalar::New(BasicPtr const& arg)
/*****************************************************************************/
{
    // if constant move sign into value
    switch (arg->getType())
    {
    case Type_Matrix:
        {
            const Matrix *mat = Util::getAsConstPtr<Matrix>(arg);
            return (*mat)(0,0);
        }
    case Type_Neg:
        {
            const Neg *neg = Util::getAsConstPtr<Neg>(arg);
            return Neg::New(New(neg->getArg()));
        }
    }
    return BasicPtr(new Scalar(arg));
}
/*****************************************************************************/