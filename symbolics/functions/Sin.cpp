#include <stdarg.h>
#include "Sin.h"
#include "Asin.h"
#include "Cos.h"
#include "Neg.h"
#include "Util.h"
#include "Mul.h"
#include <math.h>
#include "Matrix.h"

using namespace Symbolics;

ScopePtr  Sin::m_scope = ScopePtr(new Scope()); 

/*****************************************************************************/
Sin::Sin( BasicPtrVec const& args ): UnaryOp(Type_Sin, args), List(m_scope)
/*****************************************************************************/
{
    // Shape wird in UnaryOp einfach uebernommen
}
/*****************************************************************************/


/*****************************************************************************/
Sin::Sin( BasicPtr const& arg ): UnaryOp(Type_Sin, arg), List(m_scope)
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
Sin::~Sin()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
std::string Sin::toString() const
/*****************************************************************************/
{
#ifdef _DEBUG
    return "sin(" + getArg()->toString() + ")";
#else
    return "sin(" + getArg()->toString() + ")";
#endif
}
/*****************************************************************************/


/*****************************************************************************/
BasicPtr Sin::simplify()
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
            return BasicPtr( new Real(sin(1.0)) );
        }
    case Type_Int:
        {
            const Int *src = Util::getAsConstPtr<Int>(getArg());
            return BasicPtr( new Real(::sin( static_cast<double>(src->getValue()))) );
        }
    case Type_Real:
        {
            const Real *src = Util::getAsConstPtr<Real>(getArg());
            return BasicPtr( new Real(::sin(src->getValue())) );
        }
    case Type_Asin:
        {
            const Asin *asin = Util::getAsConstPtr<Asin>(getArg());
            return asin->getArg();
        }
    case Type_Neg:
        {
            const Neg *neg = Util::getAsConstPtr<Neg>(getArg());
            return Neg::New(New(neg->getArg()));
        }
    case Type_Matrix:
        {
            Matrix::UnaryFunctor<Sin> f;
            const Matrix *mat = Util::getAsConstPtr<Matrix>(getArg());
            return mat->applyFunctor(f);
        }
    }
    m_simplified = true;
    return BasicPtr(this);
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Sin::New( BasicPtr const& arg)
/*****************************************************************************/
{
    // fuer Konstanten einfach die Operation ausfuehren
    // Was haben wir denn als Argument?
    switch(arg->getType())
    {
    case Type_Zero:
        {
          return arg;
        }
    case Type_Eye:
        {
          if (arg->is_Scalar())
            return BasicPtr( new Real(sin(1.0)) );
        }
    case Type_Int:
        {
            const Int *src = Util::getAsConstPtr<Int>(arg);
            return BasicPtr( new Real(::sin( static_cast<double>(src->getValue()))) );
        }
    case Type_Real:
        {
            const Real *src = Util::getAsConstPtr<Real>(arg);
            return BasicPtr( new Real(::sin(src->getValue())) );
        }
    case Type_Asin:
        {
            const Asin *asin = Util::getAsConstPtr<Asin>(arg);
            return asin->getArg();
        }
    case Type_Neg:
        {
            const Neg *neg = Util::getAsConstPtr<Neg>(arg);
            return Neg::New(New(neg->getArg()));
        }
    case Type_Matrix:
        {
            Matrix::UnaryFunctor<Sin> f;
            const Matrix *mat = Util::getAsConstPtr<Matrix>(arg);
            BasicPtr negmat = mat->applyFunctor(f);
            return negmat;
        }
    }
    return BasicPtr(new Sin(arg));
}
/*****************************************************************************/

/*****************************************************************************/
inline BasicPtr Sin::der()
/*****************************************************************************/
{ 
  // der(sin(x)) = der(x)*cos(x); 
  return Mul::New(getArg()->der(),Cos::New(getArg())); 
}
/*****************************************************************************/

/*****************************************************************************/
inline BasicPtr Sin::der(BasicPtr const& symbol)
/*****************************************************************************/
{ 
  // der(sin(x)) = der(x)*cos(x); 
  return Mul::New(getArg()->der(symbol),Cos::New(getArg())); 
}
/*****************************************************************************/

void Sin::tricksimplify()
/*****************************************************************************/
{ 

}
/*****************************************************************************/
