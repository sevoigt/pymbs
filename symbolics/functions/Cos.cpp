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
#include "Cos.h"
#include "Sin.h"
#include "Acos.h"
#include "Neg.h"
#include "Util.h"
#include "Mul.h"
#include "Matrix.h"
#include <math.h>


using namespace Symbolics;

ScopePtr  Cos::m_scope = ScopePtr(new Scope()); 

/*****************************************************************************/
Cos::Cos( BasicPtrVec const& args ): UnaryOp(Type_Cos, args), List(m_scope)
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
Cos::Cos( BasicPtr const& arg ): UnaryOp(Type_Cos, arg), List(m_scope)
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
Cos::~Cos()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
std::string Cos::toString() const
/*****************************************************************************/
{
#ifdef _DEBUG
    return "cos(" + getArg()->toString() + ")";
#else
    return "cos(" + getArg()->toString() + ")";
#endif
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Cos::simplify()
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
            return Int::getOne();
          else
          {
            Matrix *mat = new Matrix(m_shape);
            for(size_t i=0;i<mat->getNumEl();++i)
              mat->set(i,Int::getOne());
            return BasicPtr(mat);
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
            return BasicPtr( new Real(::cos( static_cast<double>(src->getValue()))) );
        }
    case Type_Real:
        {
            const Real *src = Util::getAsConstPtr<Real>(getArg());
            return BasicPtr( new Real(::cos(src->getValue())) );
        }
    case Type_Acos:
        {
            const Acos *acos = Util::getAsConstPtr<Acos>(getArg());
            return acos->getArg();
        }
    case Type_Neg:
        {
            const Neg *neg = Util::getAsConstPtr<Neg>(getArg());
            setArg(neg->getArg());
            m_simplified = false;
            return simplify();
        }
    case Type_Matrix:
        {
            Matrix::UnaryFunctor<Cos> f;
            const Matrix *mat = Util::getAsConstPtr<Matrix>(getArg());
            return mat->applyFunctor(f);
        }
    }
    m_simplified = true;
    return BasicPtr(this);
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Cos::New( BasicPtr const& arg)
/*****************************************************************************/
{
    // fuer Konstanten einfach die Operation ausfuehren
    // Was haben wir denn als Argument?
    switch(arg->getType())
    {
    case Type_Zero:
        {
          if (arg->is_Scalar())
            return Int::getOne();
          else
          {
            Matrix *mat = new Matrix(arg->getShape());
            for(size_t i=0;i<mat->getNumEl();++i)
              mat->set(i,Int::getOne());
            return BasicPtr(mat);
          }
        }
    case Type_Eye:
        {
          if (arg->is_Scalar())
            return BasicPtr( new Real(cos(1.0)) );
        }
    case Type_Int:
        {
            const Int *src = Util::getAsConstPtr<Int>(arg);
            return BasicPtr( new Real(::cos( static_cast<double>(src->getValue()))) );
        }
    case Type_Real:
        {
            const Real *src = Util::getAsConstPtr<Real>(arg);
            return BasicPtr( new Real(::cos(src->getValue())) );
        }
    case Type_Acos:
        {
            const Acos *acos = Util::getAsConstPtr<Acos>(arg);
            return acos->getArg();
        }
    case Type_Neg:
        {
            const Neg *neg = Util::getAsConstPtr<Neg>(arg);
            return New(neg->getArg());
        }
    case Type_Matrix:
        {
            Matrix::UnaryFunctor<Cos> f;
            const Matrix *mat = Util::getAsConstPtr<Matrix>(arg);
            BasicPtr negmat = mat->applyFunctor(f);
            return negmat;
        }
    }
    return BasicPtr(new Cos(arg));
}
/*****************************************************************************/

/*****************************************************************************/
inline BasicPtr Cos::der()
{ 
  // der(cos(x)) = -der(x)*sin(x)
    return Neg::New(Mul::New(getArg()->der(),Sin::New(getArg()))); 
}
/*****************************************************************************/

/*****************************************************************************/
inline BasicPtr Cos::der(BasicPtr const& symbol)
{ 
  // der(cos(x)) = -der(x)*sin(x)
    return Neg::New(Mul::New(getArg()->der(symbol),Sin::New(getArg()))); 
}
/*****************************************************************************/