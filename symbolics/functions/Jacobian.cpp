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

#include "Jacobian.h"
#include "Util.h"
#include "Matrix.h"
#include "Der.h"
#include "Matrix.h"

using namespace Symbolics;

/*****************************************************************************/
Jacobian::Jacobian( BasicPtr const& exp, BasicPtr const& symbols ): BinaryOp(Type_Jacobian, exp, symbols)
/*****************************************************************************/
{
    validate();
}
/*****************************************************************************/


/*****************************************************************************/
Jacobian::Jacobian( BasicPtrVec const& args ): BinaryOp(Type_Jacobian, args)
/*****************************************************************************/
{
	validate();
}
/*****************************************************************************/


/*****************************************************************************/
Jacobian::~Jacobian()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
void Jacobian::validate()
/*****************************************************************************/
{
  if (getArg1()->is_Matrix())
    throw InternalError("Jacobian could only be uses with Scalar or Vector as first argument!");
  if (!getArg2()->is_Vector())
    throw InternalError("Jacobian could only be uses with Vector as second argument!");

  m_shape = Shape(getArg1()->getShape().getNumEl(),getArg2()->getShape().getNumEl());

}
/*****************************************************************************/


/*****************************************************************************/
std::string Jacobian::toString() const
/*****************************************************************************/
{
#ifdef _DEBUG
    return "jacobian(" + getArg1()->toString() + "," + getArg2()->toString() + ")";
#else
    return "jacobian(" + getArg1()->toString() + "," + getArg2()->toString() + ")";
#endif
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Jacobian::simplify()
/*****************************************************************************/
{
  if (m_simplified)
    return BasicPtr(this);
  // Argument vereinfachen
  simplifyArgs();

  // 
  if (getArg1()->is_Scalar())
  {
    BasicPtrVec vec;
    size_t numelem = getArg2()->getShape().getNumEl();
    vec.reserve(numelem);
    for (size_t i=0;i<numelem;++i)
      vec.push_back(getArg1()->der(getArg2()->getArg(i))->simplify());
    return BasicPtr(new Matrix(vec,Shape(numelem)));
  }
  else if (Util::is_Const(getArg1()))
  {
    return Zero::getZero(m_shape);
  }
  else if (getArg1()->getType() == Type_Matrix)
  {
    Matrix *mat = new Matrix(m_shape);
    for (size_t i=0;i<getArg2()->getShape().getNumEl();++i)
      for (size_t j=0;j<getArg1()->getShape().getNumEl();++j)
        mat->set(i,j,getArg1()->getArg(j)->der(getArg2()->getArg(i))->simplify());
    return BasicPtr(mat);
  }
  m_simplified = true;
  return BasicPtr(this);
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Jacobian::New( BasicPtr const& exp, BasicPtr const& symbols)
/*****************************************************************************/
{
  if (exp->is_Matrix())
    throw InternalError("Jacobian could only be used with Scalar or Vector as first argument!");
  if (!symbols->is_Vector())
    throw InternalError("Jacobian could only be used with Vector as second argument!");

  Shape s(exp->getShape().getNumEl(), symbols->getShape().getNumEl());

  BasicPtr exp1 = exp->simplify();

  if (exp1->is_Scalar()) // Wir leiten einen Skalar ab => Vektor mit Shape(1,n)
  {
    BasicPtrVec vec;
    size_t numelem = symbols->getShape().getNumEl();
    vec.reserve(numelem);
    for (size_t i=0;i<numelem;++i)
      vec.push_back(exp1->der(symbols->getArg(i))->simplify());
    return BasicPtr(new Matrix(vec,Shape(1,1,numelem)));
  }
  else if (Util::is_Const(exp1))
  {
    return Zero::getZero(s);
  }
  else if (exp1->getType() == Type_Matrix)	// Wir leiten einen Vektor ab => Matrix
  {
	  // Neue Matrix anlegen
	  Matrix *mat = new Matrix(s);

	  // i -> Symbole (=Spalten)
    for (size_t i=0;i<symbols->getShape().getNumEl();++i)
		// j - Expression (=Zeilen)
		for (size_t j=0;j<exp1->getShape().getNumEl();++j)
			// Matrixelement
			mat->set(j,i, exp1->getArg(j)->der(symbols->getArg(i))->simplify());

    return BasicPtr(mat);
  }
  return BasicPtr( new Jacobian(exp1,symbols));
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Jacobian::der()
/*****************************************************************************/
{
  return BasicPtr(new Der(BasicPtr(this)));
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Jacobian::der(BasicPtr const& symbol)
/*****************************************************************************/
{
  throw InternalError("Partial derivative of Jacobian is not supported!");
  return BasicPtr();
}
/*****************************************************************************/