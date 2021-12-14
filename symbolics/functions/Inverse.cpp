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

#include "Inverse.h"
#include "Matrix.h"
#include "Util.h"
#include "Neg.h"
#include "Operators.h"

using namespace Symbolics;

/*****************************************************************************/
Inverse::Inverse( BasicPtrVec const& args ): UnaryOp(Type_Inverse, args)
/*****************************************************************************/
{
    m_shape = getArg()->getShape();
    if (m_shape.getNrDimensions() != 2)
		  throw InternalError("inv of " + getArg()->toString() + " is not definet");
}
/*****************************************************************************/


/*****************************************************************************/
Inverse::Inverse( BasicPtr const& arg ): UnaryOp(Type_Inverse, arg)
/*****************************************************************************/
{
    // Shape berechnen
  m_shape = getArg()->getShape();
	if (m_shape.getNrDimensions() != 2)
	 throw InternalError("inv of " + getArg()->toString() + " is not definet");
}
/*****************************************************************************/


/*****************************************************************************/
Inverse::~Inverse()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
std::string Inverse::toString() const
/*****************************************************************************/
{
#ifdef _DEBUG
    return "Inverse(" + getArg()->toString() + ")";
#else
    return "inv(" + getArg()->toString() + ")";
#endif
}
/*****************************************************************************/


/*****************************************************************************/
BasicPtr Inverse::simplify()
/*****************************************************************************/
{
    if (m_simplified)
        return BasicPtr(this);
    // Argument vereinfachen
    simplifyArg();
    
    // Ist es ein Skalar?
    if (getArg()->is_Scalar())
        return getArg();

    // simplify matrices
    switch(getArg()->getType())
    {
    case Type_Eye:
    case Type_Zero:
        return getArg();
    case Type_Neg:
        {
            const Neg* neg = Util::getAsConstPtr<Neg>(getArg());
            return Neg::New(New(neg->getArg()));
        }
    case Type_Inverse:
        {
            const Inverse *trans = Util::getAsConstPtr<Inverse>(getArg());
            return trans->getArg();
        }
    }
    m_simplified = true;
    return BasicPtr(this);
}
/*****************************************************************************/


/*****************************************************************************/
BasicPtr Inverse::New( BasicPtr const& arg )
/*****************************************************************************/
{
    // Ist es ein Skalar?
    if (arg->is_Scalar())
        return arg;

    // simplify matrices
    switch(arg->getType())
    {
    case Type_Zero:
		return Zero::getZero( arg->getShape());
	  case Type_Eye:
        return arg;
    case Type_Neg:
        {
            const Neg* neg = Util::getAsConstPtr<Neg>(arg);
			BasicPtr trans( new Inverse( neg->getArg() ) );
            BasicPtr negtrans( new Neg(trans) );
            return negtrans;
        }
    case Type_Inverse:
        {
            const Inverse *trans = Util::getAsConstPtr<Inverse>(arg);
            return trans->getArg();
        }
    }
    return BasicPtr( new Inverse(arg) );
}
/*****************************************************************************/