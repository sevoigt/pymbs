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

#include "Outer.h"
#include "Matrix.h"
#include "Neg.h"
#include "Util.h"
#include "Mul.h"
#include "Operators.h"

using namespace Symbolics;

/*****************************************************************************/
Outer::Outer( BasicPtrVec const& args ): BinaryOp(Type_Outer, args)
/*****************************************************************************/
{
	validate();
}
/*****************************************************************************/


/*****************************************************************************/
Outer::Outer( BasicPtr const& arg1, BasicPtr const& arg2 ): BinaryOp(Type_Outer, arg1, arg2)
/*****************************************************************************/
{
	validate();
}
/*****************************************************************************/


/*****************************************************************************/
Outer::~Outer()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
void Outer::validate()
/*****************************************************************************/
{
	// Die Argumente muessen 2 nx1 Vektoren sein

    // Outer ist für 0 und einen nx1 Vektor definiert
	Shape const& s = getArg1()->getShape();
	const int n = s.getDimension(1);
    if (!getArg1()->is_Vector())
    {
        const std::string msg = "First Argument of Outer must be a nx1 Vector! Shape of argument is " + getArg1()->getShape().toString() + ".";
        throw ShapeError( msg.c_str() );
    }
    if (!getArg2()->is_Vector())
    {
        const std::string msg = "Second Argument of Outer must be a nx1 Vector! Shape of argument is " + getArg2()->getShape().toString() + ".";
        throw ShapeError( msg.c_str() );
    }
	const int m = getArg2()->getShape().getDimension(1);
	if (m != n)
	{
        const std::string msg = "First and second argument must have same length (" + str(n) + "!=" + str(n) + ")!";
        throw ShapeError( msg.c_str() );
	}
    // Shape auf nxn festsetzen
    m_shape = Shape(n,n);
}
/*****************************************************************************/


/*****************************************************************************/
std::string Outer::toString() const
/*****************************************************************************/
{
#ifdef _DEBUG
    return "Outer(" + getArg1()->toString() + "," + getArg1()->toString() + ")";
#else
    return "Outer(" + getArg1()->toString() + "," + getArg1()->toString() + ")";
#endif
}
/*****************************************************************************/


/*****************************************************************************/
BasicPtr Outer::simplify()
/*****************************************************************************/
{
    if (m_simplified)
        return BasicPtr(this);
    // Argument vereinfachen
    simplifyArgs();

    // if zero, return zero
    if (Util::is_Zero(getArg1())) return Zero::getZero(m_shape);
    if (Util::is_Zero(getArg2())) return Zero::getZero(m_shape);

	  // simplify matrices
	  if ( (getArg1()->getType() == Type_Matrix) && (getArg2()->getType() == Type_Matrix) )
	  {
		  const Matrix *mat1 = Util::getAsConstPtr<Matrix>(getArg1().get());
		  const Matrix *mat2 = Util::getAsConstPtr<Matrix>(getArg2().get());
		  Matrix *mat3 = new Matrix( (*mat1)*mat2->transpose() );
		  return BasicPtr(mat3);
	  }
	  // do not simplify
    m_simplified = true;
    return BasicPtr(this);
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Outer::New( BasicPtr const& arg1, BasicPtr const& arg2 )
/*****************************************************************************/
{
    // if zero, return zero
    if (Util::is_Zero(arg1)) return Zero::getZero();
    if (Util::is_Zero(arg2)) return Zero::getZero();

	// simplify matrices
	if ( (arg1->getType() == Type_Matrix) && (arg2->getType() == Type_Matrix) )
	{
		const Matrix *mat1 = Util::getAsConstPtr<Matrix>(arg1.get());
		const Matrix *mat2 = Util::getAsConstPtr<Matrix>(arg2.get());
		Matrix *mat3 = new Matrix( (*mat1)*mat2->transpose() );
		return BasicPtr(mat3);
	}

    return BasicPtr(new Outer(arg1, arg2));
}
/*****************************************************************************/