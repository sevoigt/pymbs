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
#include "Tan.h"
#include "Atan.h"
#include "Neg.h"
#include "Util.h"
#include "Matrix.h"
#include "Cos.h"
#include "Pow.h"
#include "Mul.h"
#include <math.h>

using namespace Symbolics;

ScopePtr  Tan::m_scope = ScopePtr(new Scope()); 

/*****************************************************************************/
Tan::Tan( BasicPtrVec const& args ): UnaryOp(Type_Tan, args), List(m_scope)
/*****************************************************************************/
{
    // Shape wird in UnaryOp einfach uebernommen
}
/*****************************************************************************/


/*****************************************************************************/
Tan::Tan( BasicPtr const& arg ): UnaryOp(Type_Tan, arg), List(m_scope)
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
Tan::~Tan()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
std::string Tan::toString() const
/*****************************************************************************/
{
#ifdef _DEBUG
    return "tan(" + getArg()->toString() + ")";
#else
    return "tan(" + getArg()->toString() + ")";
#endif
}
/*****************************************************************************/


/*****************************************************************************/
BasicPtr Tan::simplify()
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
          return getArg();
        }
    case Type_Eye:
        {
          if (getArg()->is_Scalar())
            return BasicPtr( new Real(tan(1.0)) );
        }
    case Type_Int:
        {
            const Int *src = Util::getAsConstPtr<Int>(getArg());
            return BasicPtr( new Real(tan( static_cast<double>(src->getValue()))) );
        }
    case Type_Real:
        {
            const Real *src = Util::getAsConstPtr<Real>(getArg());
            return BasicPtr( new Real(tan(src->getValue())) );
        }
    case Type_Atan:
        {
            const Atan *t = Util::getAsConstPtr<Atan>(getArg());
            return t->getArg();
        }
    case Type_Neg:
        {
            const Neg *neg = Util::getAsConstPtr<Neg>(getArg());
            return Neg::New(New(neg->getArg()));
        }
    case Type_Matrix:
        {
            Matrix::UnaryFunctor<Tan> f;
            const Matrix *mat = Util::getAsConstPtr<Matrix>(getArg());
            return mat->applyFunctor(f);
        }
    }
    m_simplified = true;
    return BasicPtr(this);
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Tan::New( BasicPtr const& arg)
/*****************************************************************************/
{
    // if zero, return zero
    if (arg == Zero::getZero()) return Zero::getZero();

    // simplify matrices
    switch(arg->getType())
    {
    case Type_Zero:
        {
          return arg;
        }
    case Type_Eye:
        {
          if (arg->is_Scalar())
            return BasicPtr( new Real(tan(1.0)) );
        }
    case Type_Int:
        {
            const Int *src = Util::getAsConstPtr<Int>(arg);
            return BasicPtr( new Real(tan( static_cast<double>(src->getValue()))) );
        }
    case Type_Real:
        {
            const Real *src = Util::getAsConstPtr<Real>(arg);
            return BasicPtr( new Real(tan(src->getValue())) );
        }
    case Type_Atan:
        {
            const Atan *t = Util::getAsConstPtr<Atan>(arg);
            return t->getArg();
        }
    case Type_Neg:
        {
            const Neg *neg = Util::getAsConstPtr<Neg>(arg);
            return Neg::New(New(neg->getArg()));
        }
    case Type_Matrix:
        {
            Matrix::UnaryFunctor<Tan> f;
            const Matrix *mat = Util::getAsConstPtr<Matrix>(arg);
            return mat->applyFunctor(f);
        }
    }
    return BasicPtr(new Tan(arg));
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Tan::der()
/*****************************************************************************/
{
  // der(tan(x)) = der(x)/(cos(x)^2)
  return Util::div(getArg()->der(),Pow::New(Cos::New(getArg()),Int::New(2)));
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Tan::der(BasicPtr const& symbol)
/*****************************************************************************/
{
  // der(tan(x)) = der(x)/(cos(x)^2)
  return Util::div(getArg()->der(symbol),Pow::New(Cos::New(getArg()),Int::New(2)));
}
/*****************************************************************************/