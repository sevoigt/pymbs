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

#include "Der.h"
#include "Util.h"
#include "Symbol.h"
#include "Int.h"
#include "Matrix.h"

using namespace Symbolics;

/*****************************************************************************/
Der::Der( BasicPtrVec const& args ): UnaryOp(Type_Der, args)
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
Der::Der( BasicPtr const& arg ): UnaryOp(Type_Der, arg)
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
Der::~Der()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
std::string Der::toString() const
/*****************************************************************************/
{
#ifdef _DEBUG
    return "der(" + getArg()->toString() + ")";
#else
    return "der(" + getArg()->toString() + ")";
#endif
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Der::simplify()
/*****************************************************************************/
{
    if (m_simplified)
        return BasicPtr(this);
    // Argument vereinfachen
    simplifyArg();

    // simplify constants
  switch( getArg()->getType())
  {
  case Type_Zero:
  case Type_Eye:
        return getArg();
  case Type_Int:
  case Type_Real:
        return Zero::getZero();
  case Type_Matrix:
    {
        Matrix::UnaryFunctor<Der> f;
        const Matrix *mat = Util::getAsConstPtr<Matrix>(getArg());
        return mat->applyFunctor(f);
    }
  }
  m_simplified = true;
  return BasicPtr(this);
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Der::New( BasicPtr const& arg)
/*****************************************************************************/
{
    // simplify constants
  switch( arg->getType())
  {
  case Type_Zero:
  case Type_Eye:
        return arg;
  case Type_Int:
  case Type_Real:
        return Zero::getZero();
  case Type_Der:
     return BasicPtr(new Der(arg));
  case Type_Matrix:
    {
        Matrix::UnaryFunctor<Der> f;
        const Matrix *mat = Util::getAsConstPtr<Matrix>(arg);
        return mat->applyFunctor(f);
    }
  }
  return arg->der();
}
/*****************************************************************************/

