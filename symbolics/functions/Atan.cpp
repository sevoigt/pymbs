#include "Atan.h"
#include "Tan.h"
#include "Pow.h"
#include "Add.h"
#include "Util.h"
#include "Matrix.h"
#include <math.h>

using namespace Symbolics;

ScopePtr  Atan::m_scope = ScopePtr(new Scope()); 

/*****************************************************************************/
Atan::Atan( BasicPtrVec const& args ): UnaryOp(Type_Atan, args), List(m_scope)
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
Atan::Atan( BasicPtr const& arg ): UnaryOp(Type_Atan, arg), List(m_scope)
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
Atan::~Atan()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
std::string Atan::toString() const
/*****************************************************************************/
{
#ifdef _DEBUG
    return "Atan(" + getArg()->toString() + ")";
#else
    return "Atan(" + getArg()->toString() + ")";
#endif
}
/*****************************************************************************/


/*****************************************************************************/
BasicPtr Atan::simplify()
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
            return BasicPtr( new Real(atan(0.0)) );
          else
          {
            Matrix::UnaryFunctor<Atan> f;
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
            return BasicPtr( new Real(atan( static_cast<double>(src->getValue()))) );
        }
    case Type_Real:
        {
            const Real *src = Util::getAsConstPtr<Real>(getArg());
            return BasicPtr( new Real(atan(src->getValue())) );
        }
    case Type_Tan:
        {
            const Tan *t = Util::getAsConstPtr<Tan>(getArg());
            return t->getArg();
        }
    case Type_Matrix:
        {
            Matrix::UnaryFunctor<Atan> f;
            const Matrix *mat = Util::getAsConstPtr<Matrix>(getArg());
            return mat->applyFunctor(f);
        }
    }
    m_simplified = true;
    return BasicPtr(this);
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Atan::New( BasicPtr const& arg)
/*****************************************************************************/
{
    // fuer Konstanten einfach die Operation ausfuehren
    // Was haben wir denn als Argument?
    switch(arg->getType())
    {
    case Type_Zero:
        {
          if (arg->is_Scalar())
            return BasicPtr( new Real(atan(0.0)) );
          else
          {
            Matrix::UnaryFunctor<Atan> f;
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
            return BasicPtr( new Real(atan( static_cast<double>(src->getValue()))) );
        }
    case Type_Real:
        {
            const Real *src = Util::getAsConstPtr<Real>(arg);
            return BasicPtr( new Real(atan(src->getValue())) );
        }
    case Type_Tan:
        {
            const Tan *t = Util::getAsConstPtr<Tan>(arg);
            return t->getArg();
        }
    case Type_Matrix:
        {
            Matrix::UnaryFunctor<Atan> f;
            const Matrix *mat = Util::getAsConstPtr<Matrix>(arg);
            return mat->applyFunctor(f);
        }
    }
    return BasicPtr(new Atan(arg));
}
/*****************************************************************************/

/*****************************************************************************/
inline BasicPtr Atan::der()
/*****************************************************************************/
{ 
  // der(atan(x)) = der(x)/(1+x^2)
    return Util::div(getArg()->der(),Add::New(Int::getOne(),Pow::New(getArg(),BasicPtr(new Int(2))))); 
}
/*****************************************************************************/

/*****************************************************************************/
inline BasicPtr Atan::der(BasicPtr const& symbol)
/*****************************************************************************/
{ 
  // der(atan(x)) = der(x)/(1+x^2)
    return Util::div(getArg()->der(symbol),Add::New(Int::getOne(),Pow::New(getArg(),BasicPtr(new Int(2))))); 
}
/*****************************************************************************/