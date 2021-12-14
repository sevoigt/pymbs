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

#include "Asin.h"
#include "Sin.h"
#include "Pow.h"
#include "Util.h"
#include "Matrix.h"
#include "Add.h"
#include "Mul.h"
#include "Neg.h"
#include <math.h>

using namespace Symbolics;

ScopePtr  Asin::m_scope = ScopePtr(new Scope()); 

/*****************************************************************************/
Asin::Asin( BasicPtrVec const& args ): UnaryOp(Type_Asin, args), List(m_scope)
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
Asin::Asin( BasicPtr const& arg ): UnaryOp(Type_Asin, arg), List(m_scope)
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
Asin::~Asin()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
std::string Asin::toString() const
/*****************************************************************************/
{
#ifdef _DEBUG
    return "Asin(" + getArg()->toString() + ")";
#else
    return "Asin(" + getArg()->toString() + ")";
#endif
}
/*****************************************************************************/


/*****************************************************************************/
BasicPtr Asin::simplify()
/*****************************************************************************/
{
    if (m_simplified)
        return BasicPtr(this);
    // Argument vereinfachen
    simplifyArg();

    // fuer Konstanten einfach die Operation ausfuehren
    // Was haben wir denn als Argument?
    switch(getArg()->getType())
    {
    case Type_Zero:
        {
          if (getArg()->is_Scalar())
            return BasicPtr( new Real(asin(0.0)) );
          else
          {
            Matrix::UnaryFunctor<Asin> f;
            Matrix *mat = new Matrix(m_shape);
            return mat->applyFunctor(f);
          }
        }
    case Type_Eye:
        {
          if (getArg()->is_Scalar())
            return BasicPtr( new Real(asin(1.0)) );
        }
    case Type_Int:
        {
            const Int *src = Util::getAsConstPtr<Int>(getArg());
            return BasicPtr( new Real(asin( static_cast<double>(src->getValue()))) );
        }
    case Type_Real:
        {
            const Real *src = Util::getAsConstPtr<Real>(getArg());
            return BasicPtr( new Real(asin(src->getValue())) );
        }
    case Type_Sin:
        {
            const Sin *s = Util::getAsConstPtr<Sin>(getArg());
            return s->getArg();
        }

    case Type_Matrix:
        {
            Matrix::UnaryFunctor<Asin> f;
            const Matrix *mat = Util::getAsConstPtr<Matrix>(getArg());
            return mat->applyFunctor(f);
        }
    }
    m_simplified = true;
    return BasicPtr(this);
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Asin::New( BasicPtr const& arg)
/*****************************************************************************/
{
    // fuer Konstanten einfach die Operation ausfuehren
    // Was haben wir denn als Argument?
    switch(arg->getType())
    {
    case Type_Zero:
        {
          if (arg->is_Scalar())
            return BasicPtr( new Real(asin(0.0)) );
          else
          {
            Matrix::UnaryFunctor<Asin> f;
            Matrix *mat = new Matrix(arg->getShape());
            return mat->applyFunctor(f);
          }
        }
    case Type_Eye:
        {
          if (arg->is_Scalar())
            return BasicPtr( new Real(asin(1.0)) );
        }
    case Type_Int:
        {
            const Int *src = Util::getAsConstPtr<Int>(arg);
            return BasicPtr( new Real(asin( static_cast<double>(src->getValue()))) );
        }
    case Type_Real:
        {
            const Real *src = Util::getAsConstPtr<Real>(arg);
            return BasicPtr( new Real(asin(src->getValue())) );
        }
    case Type_Sin:
        {
            const Sin *s = Util::getAsConstPtr<Sin>(arg);
            return s->getArg();
        }

    case Type_Matrix:
        {
            Matrix::UnaryFunctor<Asin> f;
            const Matrix *mat = Util::getAsConstPtr<Matrix>(arg);
            return mat->applyFunctor(f);
        }
    }
    return BasicPtr(new Asin(arg));
}
/*****************************************************************************/

/*****************************************************************************/
inline BasicPtr Asin::der()
/*****************************************************************************/
{ 
  // der(asin(x)) = der(x)/sqrt(1-x^2)
  return Util::div(getArg()->der(),Util::sqrt(Add::New(Int::getOne(),Neg::New(Pow::New(getArg(),Int::New(2))))));
}
/*****************************************************************************/

/*****************************************************************************/
inline BasicPtr Asin::der(BasicPtr const& symbol)
/*****************************************************************************/
{ 
  // der(asin(x)) = der(x)/sqrt(1-x^2)
  return Util::div(getArg()->der(symbol),Util::sqrt(Add::New(Int::getOne(),Neg::New(Pow::New(getArg(),Int::New(2))))));
}
/*****************************************************************************/