#include "Neg.h"
#include "Constant.h"
#include "Util.h"

using namespace Symbolics;

/*****************************************************************************/
Neg::Neg( ConstBasicPtr exp ): UnaryOp(Type_Neg)
/*****************************************************************************/
{
	// Argument speichern
	m_arg = exp;

	// Shape berechnen
	m_shape = exp->getShape();
}
/*****************************************************************************/


/*****************************************************************************/
Neg::Neg( const Basic *exp ): UnaryOp(Type_Neg)
/*****************************************************************************/
{
	// Argument speichern
	m_arg = ConstBasicPtr(exp);

	// Shape berechnen
	m_shape = exp->getShape();
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
	return "Neg(" + m_arg->toString() + ")";
#else
	return "-(" + m_arg->toString() + ")";
#endif
}
/*****************************************************************************/


/*****************************************************************************/
ConstBasicPtr Neg::simplify() const
/*****************************************************************************/
{
    bool simplified = false;
    // call arg
    ConstBasicPtr simply_arg = m_arg->simplify();
    if (simply_arg.get() == NULL) 
        simply_arg = m_arg;
    else
        simplified = true;

	// simplify constants
    if (simply_arg->getType() == Type_Constant_Int)
    {
        const Constant_Int *c = Util::getAsPtr<Constant_Int>(simply_arg);
        Constant_Int *negc = new Constant_Int(0);
        *negc = -(*c);
        return ConstBasicPtr(negc);
    }
    if (simply_arg->getType() == Type_Constant_Dbl)
    {
        const Constant_Dbl *c = Util::getAsPtr<Constant_Dbl>(simply_arg);
        Constant_Dbl *negc = new Constant_Dbl(0);
        *negc = -(*c);
        return ConstBasicPtr(negc);
    }
    // simplify Neg
    if (simply_arg->getType() == Type_Neg)
    {
        const Neg *c = Util::getAsPtr<Neg>(simply_arg);
        return c->getArg();
    }
    if (simplified)
    {
        ConstBasicPtr arg(new Neg(simply_arg));
	    return arg;
    }
    return ConstBasicPtr();
}
/*****************************************************************************/

/*****************************************************************************/
ConstBasicPtr Neg::subs(const ConstBasicPtr old_exp, 
                           const ConstBasicPtr new_exp) const
/*****************************************************************************/
{
    if (this == old_exp.get())
        return new_exp;
    // call arg
    ConstBasicPtr subs_arg = m_arg->subs(old_exp,new_exp);
    return Util::negate(subs_arg);
}
/*****************************************************************************/
