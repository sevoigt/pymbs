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

#include "Acos.h"
#include "Cos.h"
#include "Util.h"
#include "Neg.h"
#include "Pow.h"
#include "Real.h"
#include "Add.h"
#include "Mul.h"
#include "Matrix.h"
#include <math.h>

using namespace Symbolics;

ScopePtr  Acos::m_scope = ScopePtr(new Scope()); 

/*****************************************************************************/
Acos::Acos( BasicPtrVec const& args ): UnaryOp(Type_Acos, args), List(m_scope)
/*****************************************************************************/
{
    // Shape wird in UnaryOp einfach uebernommen
}
/*****************************************************************************/


/*****************************************************************************/
Acos::Acos( BasicPtr const& arg ): UnaryOp(Type_Acos, arg), List(m_scope)
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
Acos::~Acos()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
std::string Acos::toString() const
/*****************************************************************************/
{
#ifdef _DEBUG
    return "Acos(" + getArg()->toString() + ")";
#else
    return "Acos(" + getArg()->toString() + ")";
#endif
}
/*****************************************************************************/


/*****************************************************************************/
BasicPtr Acos::simplify()
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
            return BasicPtr( new Real(acos(0.0)) );
          else
          {
            Matrix::UnaryFunctor<Acos> f;
            Matrix *mat = new Matrix(m_shape);
            return mat->applyFunctor(f);
          }
        }
    case Type_Eye:
        {
          if (getArg()->is_Scalar())
            return BasicPtr( new Real(acos(1.0)) );
        }
    case Type_Int:
        {
            const Int *src = Util::getAsConstPtr<const Int>(getArg());
            return BasicPtr( new Real(acos( static_cast<double>(src->getValue()))) );
        }
    case Type_Real:
        {
            const Real *src = Util::getAsConstPtr<Real>(getArg());
            return BasicPtr( new Real(acos(src->getValue())) );
        }
    case Type_Cos:
        {
            const Cos *c = Util::getAsConstPtr<Cos>(getArg());
            return c->getArg();
        }
    case Type_Matrix:
        {
            Matrix::UnaryFunctor<Acos> f;
            const Matrix *mat = Util::getAsConstPtr<Matrix>(getArg());
            return mat->applyFunctor(f);
        }
    }
    m_simplified = true;
    return BasicPtr(this);
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Acos::New( BasicPtr const& arg)
/*****************************************************************************/
{
    // fuer Konstanten einfach die Operation ausfuehren
    // Was haben wir denn als Argument?
    switch(arg->getType())
    {
    case Type_Zero:
        {
          if (arg->is_Scalar())
            return BasicPtr( new Real(acos(0.0)) );
          else
          {
            Matrix::UnaryFunctor<Acos> f;
            Matrix *mat = new Matrix(arg->getShape());
            return mat->applyFunctor(f);
          }
        }
    case Type_Eye:
        {
          if (arg->is_Scalar())
            return BasicPtr( new Real(acos(1.0)) );
        }
    case Type_Int:
        {
            const Int *src = Util::getAsConstPtr<const Int>(arg);
            return BasicPtr( new Real(acos( static_cast<double>(src->getValue()))) );
        }
    case Type_Real:
        {
            const Real *src = Util::getAsConstPtr<Real>(arg);
            return BasicPtr( new Real(acos(src->getValue())) );
        }
    case Type_Cos:
        {
            const Cos *c = Util::getAsConstPtr<Cos>(arg);
            return c->getArg();
        }
    case Type_Matrix:
        {
            Matrix::UnaryFunctor<Acos> f;
            const Matrix *mat = Util::getAsConstPtr<Matrix>(arg);
            return mat->applyFunctor(f);
        }
    }
    return BasicPtr(new Acos(arg));
}
/*****************************************************************************/

/*****************************************************************************/
inline BasicPtr Acos::der()
/*****************************************************************************/
{ 
  // der(acos(x)) = -der(x)/sqrt(1-x^2)
  return Neg::New(Util::div(getArg()->der(),Util::sqrt(Add::New(Int::getOne(),Neg::New(Pow::New(getArg(),Int::New(2)))))));
}
/*****************************************************************************/

/*****************************************************************************/
inline BasicPtr Acos::der(BasicPtr const& symbol)
/*****************************************************************************/
{ 
  // der(acos(x)) = -der(x)/sqrt(1-x^2)
  return Neg::New(Util::div(getArg()->der(symbol),Util::sqrt(Add::New(Int::getOne(),Neg::New(Pow::New(getArg(),Int::New(2)))))));
}
/*****************************************************************************/