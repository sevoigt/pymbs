#include "Abs.h"
#include "Neg.h"
#include "Pow.h"
#include "Util.h"
#include "Sign.h"
#include "Zero.h"
#include "Matrix.h"
#include "Mul.h"

#include <math.h>

using namespace Symbolics;

/*****************************************************************************/
Abs::Abs( BasicPtrVec const& args ): UnaryOp(Type_Abs, args)
/*****************************************************************************/
{
    // Shape wird in UnaryOp einfach uebernommen
}
/*****************************************************************************/


/*****************************************************************************/
Abs::Abs( BasicPtr const& arg ): UnaryOp(Type_Abs, arg)
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
Abs::~Abs()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
std::string Abs::toString() const
/*****************************************************************************/
{
#ifdef _DEBUG
    return "abs(" + getArg()->toString() + ")";
#else
    return "abs(" + getArg()->toString() + ")";
#endif
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Abs::simplify()
/*****************************************************************************/
{
    if (m_simplified)
        return BasicPtr(this);
    // Argument vereinfachen
    simplifyArg();

    // Was haben wir denn als Argument?
    switch(getArg()->getType())
    {
    case Type_Zero:
      return getArg();
    case Type_Eye:
      return getArg();
    // Negs werden geloescht
    case Type_Neg:
        {
            const Neg *n = Util::getAsConstPtr<Neg>(getArg());
            return New(n->getArg());
        }
    // fuer Konstanten einfach die Operation ausfuehren
    case Type_Int:
        {
            const Int *src = Util::getAsConstPtr<Int>(getArg());
            int i = src->getValue();
            if ( i >= 0 )
                return getArg();
            else
                return BasicPtr( new Int(-i) );
        }
    case Type_Real:
        {
            const  Real *src = Util::getAsConstPtr<Real>(getArg());
            double r = src->getValue();
            if ( r >= 0 )
                return getArg();
            else
                return BasicPtr( new Real(-r) );
        }
    // fuer Powers mit geraden Exponenten, brauchen wir kein Abs
    case Type_Pow:
        {
            const Pow *p = Util::getAsConstPtr<Pow>(getArg());
            int exp;
            if (Util::is_Int(p->getArg2(), exp))
                // Ist der Exponent auch gerade?
                if ((exp%2)==0)
                    return getArg();
            break;
        }
    case Type_Matrix:
        {
            Matrix::UnaryFunctor<Abs> f;
            const Matrix *mat = Util::getAsConstPtr<Matrix>(getArg());
            return mat->applyFunctor(f);
        }    
    }
    m_simplified = true;
    return BasicPtr(this);
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Abs::New( BasicPtr const& arg)
/*****************************************************************************/
{
    // fuer Konstanten einfach die Operation ausfuehren
    // Was haben wir denn als Argument?
    switch(arg->getType())
    {
    case Type_Zero:
      return arg;
    case Type_Eye:
      return arg;
    // Negs werden geloescht
    case Type_Neg:
        {
            const Neg *n = Util::getAsConstPtr<Neg>(arg);
            return New(n->getArg());
        }
    // fuer Konstanten einfach die Operation ausfuehren
    case Type_Int:
        {
            const Int *src = Util::getAsConstPtr<Int>(arg);
            int i = src->getValue();
            if ( i >= 0 )
                return arg;
            else
                return BasicPtr( new Int(-i) );
        }
    case Type_Real:
        {
            const  Real *src = Util::getAsConstPtr<Real>(arg);
            double r = src->getValue();
            if ( r >= 0 )
                return arg;
            else
                return BasicPtr( new Real(-r) );
        }
    // fuer Powers mit geraden Exponenten, brauchen wir kein Abs
    case Type_Pow:
        {
            const Pow *p = Util::getAsConstPtr<Pow>(arg);
            int exp;
            if (Util::is_Int(p->getArg2(), exp))
                // Ist der Exponent auch gerade?
                if ((exp%2)==0)
                    return arg;
            break;
        }
    case Type_Matrix:
        {
            Matrix::UnaryFunctor<Abs> f;
            const Matrix *mat = Util::getAsConstPtr<Matrix>(arg);
            return mat->applyFunctor(f);
        }    
    }
    return BasicPtr(new Abs(arg));
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Abs::der()
/*****************************************************************************/
{ 
  // der(abs(x)) = sign(x)*der(x)
  BasicPtr derarg = getArg()->der();
  return Mul::New(Sign::New(getArg()),getArg()->der()); 
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Abs::der(BasicPtr const& symbol)
/*****************************************************************************/
{ 
  // der(abs(x)) = sign(x)*der(x)
  BasicPtr derarg = getArg()->der(symbol);
  return Mul::New(Sign::New(getArg()),getArg()->der(symbol)); 
}
/*****************************************************************************/