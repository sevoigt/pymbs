#include "Neg.h"
#include "Int.h"
#include "Bool.h"
#include "Matrix.h"
#include "Util.h"

using namespace Symbolics;

/*****************************************************************************/
Neg::Neg( BasicPtrVec const& args ): UnaryOp(Type_Neg, args)
/*****************************************************************************/
{
    // Shape wird in UnaryOp einfach uebernommen
}
/*****************************************************************************/


/*****************************************************************************/
Neg::Neg( BasicPtr const& arg ): UnaryOp(Type_Neg, arg)
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
Neg::~Neg()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
std::string Neg::toString() const
/*****************************************************************************/
{
#ifdef _DEBUG
    return "Neg(" + getArg()->toString() + ")";
#else
    return "-(" + getArg()->toString() + ")";
#endif
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Neg::simplify()
/*****************************************************************************/
{
    if (m_simplified)
        return BasicPtr(this);
    // Argument vereinfachen
    simplifyArg();

    // simplify constants
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
            return Int::New(-i);
        }
    case Type_Real:
        {
            const Real *c = Util::getAsConstPtr<Real>(getArg());
            return BasicPtr( new Real(-(c->getValue())) );
        }
    case Type_Bool:
        {
            const Bool *c = Util::getAsConstPtr<Bool>(getArg());
            bool b = c->getValue();
            return BasicPtr(new Bool(!b));
        }
    case Type_Matrix:
        {
             Matrix::UnaryFunctor<Neg> f;
             const Matrix *mat = Util::getAsConstPtr<Matrix>(getArg());
             return mat->applyFunctor(f);
        }
    case Type_Neg:
        {
            const Neg *c = Util::getAsConstPtr<Neg>(getArg());
            return c->getArg();
        }
    }
    // simplify Neg
    m_simplified = true;
    return BasicPtr(this);
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Neg::New(BasicPtr const& e)
/*****************************************************************************/
{
    // if constant move sign into value
    switch (e->getType())
    {
    case Type_Zero:
        {
          return e;
        }
    case Type_Int:
        {
            const Int *c = Util::getAsConstPtr<Int>(e);
            return BasicPtr( new Int(-(*c).getValue()) );
        }
    case Type_Bool:
        {
            const Bool *c = Util::getAsConstPtr<Bool>(e);
            bool b = c->getValue();
            return BasicPtr(new Bool(!b));
        }
    case Type_Real:
        {
            const Real *c = Util::getAsConstPtr<Real>(e);
            return BasicPtr( new Real(-(*c).getValue()) );
        }
        // -(-(a)) = a
    case Type_Neg:
        {
            const Neg *c = Util::getAsConstPtr<Neg>(e);
            return c->getArg();
        }
    case Type_Matrix:
        {
            BasicSet atoms = e->getAtoms();
            if (atoms.size() == 0)
            {
                const Matrix *c = Util::getAsConstPtr<Matrix>(e);
                return Int::getMinusOne()*(*c);
            }
            break;
        }
    }
    return BasicPtr(new Neg(e));
}
/*****************************************************************************/
