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

#include "Transpose.h"
#include "Int.h"
#include "Matrix.h"
#include "Util.h"
#include "Neg.h"
#include "Operators.h"

using namespace Symbolics;

/*****************************************************************************/
Transpose::Transpose( BasicPtrVec const& args ): UnaryOp(Type_Transpose, args)
/*****************************************************************************/
{
    m_shape = getArg()->getShape().transpose();
}
/*****************************************************************************/


/*****************************************************************************/
Transpose::Transpose( BasicPtr const& arg ): UnaryOp(Type_Transpose, arg)
/*****************************************************************************/
{
    // Shape berechnen
    m_shape = getArg()->getShape().transpose();
}
/*****************************************************************************/


/*****************************************************************************/
Transpose::~Transpose()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
std::string Transpose::toString() const
/*****************************************************************************/
{
#ifdef _DEBUG
    return "Transpose(" + getArg()->toString() + ")";
#else
    return "Transpose(" + getArg()->toString() + ")";
#endif
}
/*****************************************************************************/


/*****************************************************************************/
BasicPtr Transpose::simplify()
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
    case Type_Int:
    case Type_Real:
        return getArg();
    case Type_Matrix:
        {
            const Matrix *mat = Util::getAsConstPtr<Matrix>(getArg());
            return BasicPtr(new Matrix(mat->transpose()));
        }
    case Type_Neg:
        {
            const Neg* neg = Util::getAsConstPtr<Neg>(getArg());
            return Neg::New(New(neg->getArg()));
        }
    case Type_Transpose:
        {
            const Transpose *trans = Util::getAsConstPtr<Transpose>(getArg());
            return trans->getArg();
        }
    }
    m_simplified = true;
    return BasicPtr(this);
}
/*****************************************************************************/


/*****************************************************************************/
BasicPtr Transpose::New( BasicPtr const& arg )
/*****************************************************************************/
{
    // Ist es ein Skalar?
    if (arg->is_Scalar())
        return arg;

    // simplify matrices
    switch(arg->getType())
    {
    case Type_Zero:
		return Zero::getZero( arg->getShape().transpose() );
	case Type_Eye:
    case Type_Int:
    case Type_Real:
        return arg;
    case Type_Matrix:
        {
            const Matrix *mat = Util::getAsConstPtr<Matrix>(arg);
            return BasicPtr(new Matrix(mat->transpose()));
        }
    case Type_Neg:
        {
            const Neg* neg = Util::getAsConstPtr<Neg>(arg);
			BasicPtr trans( new Transpose( neg->getArg() ) );
            BasicPtr negtrans( new Neg(trans) );
            return negtrans;
        }
    case Type_Transpose:
        {
            const Transpose *trans = Util::getAsConstPtr<Transpose>(arg);
            return trans->getArg();
        }
    }
    return BasicPtr( new Transpose(arg) );
}
/*****************************************************************************/