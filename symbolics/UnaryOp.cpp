#include "UnaryOp.h"
#include "str.h"
#include "Factory.h"

using namespace Symbolics;


/*****************************************************************************/
UnaryOp::UnaryOp( Basic_Type type, BasicPtrVec const& args): Basic(type)
/*****************************************************************************/
{
    m_arg.registerParent(this);
    // args pruefen
    if (args.size() != 1) throw InternalError("BinaryOp: Args must be of length 1, not " + str(args.size()) + "!");
    // args uebernehmen
    setArg(args[0]);
	  // erst mal shape uerbernehmen, kann spaeter wieder geaendert werden
    m_shape = getArg()->getShape();
}
/*****************************************************************************/


/*****************************************************************************/
UnaryOp::UnaryOp( Basic_Type type, BasicPtr const& arg): Basic(type)
/*****************************************************************************/
{
   m_arg.registerParent(this);
	// args uebernehmen
	setArg(arg);
	// erst mal shape uerbernehmen, kann spaeter wieder geaendert werden
  m_shape = getArg()->getShape();
}
/*****************************************************************************/


/*****************************************************************************/
UnaryOp::~UnaryOp()
/*****************************************************************************/
{
   m_arg.unregisterParent();
}
/*****************************************************************************/


/*****************************************************************************/
bool UnaryOp::operator ==( Basic const& rhs) const
/*****************************************************************************/
{
    // Typen vergleichen
    if (getType() != rhs.getType()) 
      return false;

    // Vergleich Shape
    if (getShape() != rhs.getShape())
      return false;

    // Versuchen zu casten
    const UnaryOp *unary = dynamic_cast<const UnaryOp*>(&rhs);
    if (unary == NULL) return false;

    // Vergleich
    return (getArg() == (unary->getArg()));
}
/*****************************************************************************/

/*****************************************************************************/
bool UnaryOp::operator<(Basic const& rhs) const
/*****************************************************************************/
{
    // Typen vergleichen
    if (getType() != rhs.getType())
        return getType() < rhs.getType();

    // Vergleich Shape
    if (getShape() != rhs.getShape())
        return (getShape() < rhs.getShape());

    // UnaryOp
    const UnaryOp &unary = dynamic_cast<const UnaryOp &>(rhs);
    BasicPtr rhsArg(unary.getArg()); 

    // Args vergleichen
    return (*(getArg().get())) < (*rhsArg.get());

}
/*****************************************************************************/

/*****************************************************************************/
 BasicPtr UnaryOp::subs( ConstBasicPtr const& old_exp, 
                       BasicPtr const& new_exp)
/*****************************************************************************/
{
    if (*this == *old_exp.get())
        return new_exp;

    // subs args
    BasicPtr subs_arg = getArg()->subs(old_exp,new_exp);
    return Factory::newBasic( getType(), subs_arg, getShape() );
}
/*****************************************************************************/
