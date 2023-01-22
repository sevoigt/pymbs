#include "Sign.h"
#include "Der.h"
#include "Int.h"
#include "Bool.h"
#include "Matrix.h"
#include "Neg.h"
#include "Util.h"
#include "Operators.h"

using namespace Symbolics;

/*****************************************************************************/
Sign::Sign( BasicPtrVec const& args ): UnaryOp(Type_Sign, args)
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
Sign::Sign( BasicPtr const& arg ): UnaryOp(Type_Sign, arg)
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
Sign::~Sign()
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
std::string Sign::toString() const
/*****************************************************************************/
{
#ifdef _DEBUG
    return "Sign(" + getArg()->toString() + ")";
#else
    return "Sign(" + getArg()->toString() + ")";
#endif
}
/*****************************************************************************/


/*****************************************************************************/
BasicPtr Sign::simplify()
/*****************************************************************************/
{
    if (m_simplified)
        return BasicPtr(this);
    // Argument vereinfachen
    simplifyArg();

    // if zero, return zero
    if (getArg() == Zero::getZero()) return getArg();

    // simplify matrices
    switch(getArg()->getType())
    {
    case Type_Zero:
        {
          return getArg();
        }
    case Type_Int:
        {
            const Int *c = Util::getAsConstPtr<Int>(getArg());
            int i = c->getValue();
            return i>0?Int::getOne():Int::getMinusOne();
        }
    case Type_Real:
        {
            const Real *c = Util::getAsConstPtr<Real>(getArg());
            return c->getValue()>0?Int::getOne():Int::getMinusOne();
        }
    case Type_Bool:
        {
            const Bool *c = Util::getAsConstPtr<Bool>(getArg());
            bool b = c->getValue();
            return c->getValue()?Int::getOne():Int::getMinusOne();
        }
    case Type_Matrix:
        {
            Matrix::UnaryFunctor<Sign> f;
            const Matrix *mat = Util::getAsConstPtr<Matrix>(getArg());
            return mat->applyFunctor(f);
        }
    case Type_Neg:
        {
            const Neg* neg = Util::getAsConstPtr<Neg>(getArg());
            return Neg::New(New(neg->getArg()));
        }
    }
    m_simplified = true;
    return BasicPtr(this);
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Sign::New( BasicPtr const& arg)
/*****************************************************************************/
{
    // if zero, return zero
    if (arg == Zero::getZero()) return arg;

    // simplify matrices
    switch(arg->getType())
    {
    case Type_Zero:
        {
          return arg;
        }
    case Type_Int:
        {
            const Int *c = Util::getAsConstPtr<Int>(arg);
            int i = c->getValue();
            return i>0?Int::getOne():Int::getMinusOne();
        }
    case Type_Real:
        {
            const Real *c = Util::getAsConstPtr<Real>(arg);
            return c->getValue()>0?Int::getOne():Int::getMinusOne();
        }
    case Type_Bool:
        {
            const Bool *c = Util::getAsConstPtr<Bool>(arg);
            bool b = c->getValue();
            return c->getValue()?Int::getOne():Int::getMinusOne();
        }
    case Type_Matrix:
        {
            Matrix::UnaryFunctor<Sign> f;
            const Matrix *mat = Util::getAsConstPtr<Matrix>(arg);
            return mat->applyFunctor(f);
        }
    case Type_Neg:
        {
            const Neg* neg = Util::getAsConstPtr<Neg>(arg);
            return Neg::New(New(neg->getArg()) );
        }
    }
    return BasicPtr(new Sign(arg));
}
/*****************************************************************************/

/*****************************************************************************/
inline BasicPtr Sign::der()
/*****************************************************************************/
{
  return BasicPtr(new Der(BasicPtr(this)));
}
/*****************************************************************************/

/*****************************************************************************/
inline BasicPtr Sign::der(BasicPtr const& symbol)
/*****************************************************************************/
{
  return New(getArg()->der(symbol));
}
/*****************************************************************************/