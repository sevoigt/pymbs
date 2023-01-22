#include "Greater.h"
#include "Bool.h"
#include "Util.h"

using namespace Symbolics;

/*****************************************************************************/
Greater::Greater( BasicPtrVec const& args ): BinaryOp(Type_Greater, args)
/*****************************************************************************/
{
    // Shape berechnen
    if (getArg1()->getShape() != getArg2()->getShape())
        throw InternalError("Greater shapes not equal!");

    m_shape = getArg1()->getShape();
}
/*****************************************************************************/


/*****************************************************************************/
Greater::Greater( BasicPtr const& arg1, BasicPtr const& arg2 ): BinaryOp(Type_Greater, arg1, arg2)
/*****************************************************************************/
{
    // Shape berechnen
    if (getArg1()->getShape() != getArg2()->getShape())
        throw InternalError("Greater shapes not equal!");

    m_shape = getArg1()->getShape();
}
/*****************************************************************************/

/*****************************************************************************/
Greater::~Greater()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
std::string Greater::toString() const
/*****************************************************************************/
{
#ifdef _DEBUG
    return "greater(" + getArg1()->toString() + "," + getArg2()->toString() + ")";
#else
    return getArg1()->toString() + ">" + getArg2()->toString();
#endif
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Greater::simplify()
/*****************************************************************************/
{
    if (m_simplified)
        return BasicPtr(this);
    // Argument vereinfachen
    simplifyArgs();

    // constants
    if (getArg1()->getType() == Type_Bool) 
    {
        const Bool *clhs = Util::getAsConstPtr<Bool>(getArg1());
        if (getArg2()->getType() == Type_Bool) 
        {
            const Bool *crhs = Util::getAsConstPtr<Bool>(getArg2());
            return Bool::New(crhs->getValue()<clhs->getValue());
        }
        if (getArg2()->getType() == Type_Int) 
        {
            const Int *crhs = Util::getAsConstPtr<Int>(getArg2());
            return Bool::New(crhs->getValue()<(clhs->getValue()?1:0));
        }
        else if (getArg2()->getType() == Type_Real) 
        {
            const Real *crhs = Util::getAsConstPtr<Real>(getArg2());
            return BasicPtr(new Real(crhs->getValue()<(clhs->getValue()?1.0:0.0)));
        }
    }
    if (getArg1()->getType() == Type_Int) 
    {
        const Int *clhs = Util::getAsConstPtr<Int>(getArg1());
        if (getArg2()->getType() == Type_Bool) 
        {
            const Bool *crhs = Util::getAsConstPtr<Bool>(getArg2());
            return Bool::New((crhs->getValue()?1:0)<clhs->getValue());
        }
        if (getArg2()->getType() == Type_Int) 
        {
            const Int *crhs = Util::getAsConstPtr<Int>(getArg2());
            return Bool::New(crhs->getValue()<clhs->getValue());
        }
        else if (getArg2()->getType() == Type_Real) 
        {
            const Real *crhs = Util::getAsConstPtr<Real>(getArg2());
            return BasicPtr(new Real(crhs->getValue()<clhs->getValue()));
        }
    }
    if (getArg1()->getType() == Type_Real) 
    {
        const Real *clhs = Util::getAsConstPtr<Real>(getArg1());
        if (getArg2()->getType() == Type_Bool) 
        {
            const Bool *crhs = Util::getAsConstPtr<Bool>(getArg2());
            return Bool::New((crhs->getValue()?1:0)<clhs->getValue());
        }
        if (getArg2()->getType() == Type_Int) 
        {
            const Int *crhs = Util::getAsConstPtr<Int>(getArg2());
            return Bool::New(crhs->getValue()<clhs->getValue());
        }
        else if (getArg2()->getType() == Type_Real) 
        {
            const Real *crhs = Util::getAsConstPtr<Real>(getArg2());
            return BasicPtr(new Real(crhs->getValue()<clhs->getValue()));
        }
    }
    m_simplified = true;
    return BasicPtr(this);
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Greater::New( BasicPtr const& arg1, BasicPtr const& arg2)
/*****************************************************************************/
{
    // fuer Konstanten einfach die Operation ausfuehren
    // Was haben wir denn als Argument?
    if (arg1->getType() == Type_Bool) 
    {
        const Bool *clhs = Util::getAsConstPtr<Bool>(arg1);
        if (arg2->getType() == Type_Bool) 
        {
            const Bool *crhs = Util::getAsConstPtr<Bool>(arg2);
            return BasicPtr(new Bool(crhs->getValue()<clhs->getValue()));
        }
        if (arg2->getType() == Type_Int) 
        {
            const Int *crhs = Util::getAsConstPtr<Int>(arg2);
            return BasicPtr(new Bool(crhs->getValue()<(clhs->getValue()?1:0)));
        }
        else if (arg2->getType() == Type_Real) 
        {
            const Real *crhs = Util::getAsConstPtr<Real>(arg2);
            return BasicPtr(new Bool(crhs->getValue()<(clhs->getValue()?1:0)));
        }
    }
    if (arg1->getType() == Type_Int) 
    {
        const Int *clhs = Util::getAsConstPtr<Int>(arg1);
        if (arg2->getType() == Type_Bool) 
        {
            const Bool *crhs = Util::getAsConstPtr<Bool>(arg2);
            return BasicPtr(new Bool((crhs->getValue()?1:0)<clhs->getValue()));
        }
        if (arg2->getType() == Type_Int) 
        {
            const Int *crhs = Util::getAsConstPtr<Int>(arg2);
            return BasicPtr(new Bool(crhs->getValue()<clhs->getValue()));
        }
        else if (arg2->getType() == Type_Real) 
        {
            const Real *crhs = Util::getAsConstPtr<Real>(arg2);
            return BasicPtr(new Bool(crhs->getValue()<clhs->getValue()));
        }
    }
    if (arg1->getType() == Type_Real) 
    {
        const Real *clhs = Util::getAsConstPtr<Real>(arg1);
        if (arg2->getType() == Type_Bool) 
        {
            const Bool *crhs = Util::getAsConstPtr<Bool>(arg2);
            return BasicPtr(new Bool((crhs->getValue()?1:0)<clhs->getValue()));
        }
        if (arg2->getType() == Type_Int) 
        {
            const Int *crhs = Util::getAsConstPtr<Int>(arg2);
            return BasicPtr(new Bool(crhs->getValue()<clhs->getValue()));
        }
        else if (arg2->getType() == Type_Real) 
        {
            const Real *crhs = Util::getAsConstPtr<Real>(arg2);
            return BasicPtr(new Bool(crhs->getValue()<clhs->getValue()));
        }
    }
    return BasicPtr(new Greater(arg1,arg2));
}
/*****************************************************************************/