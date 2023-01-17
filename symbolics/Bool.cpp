#include "Bool.h"
#include "Int.h"
#include "Bool.h"
#include "Util.h"
#include "Operators.h"

using namespace Symbolics;

/*****************************************************************************/
BasicPtr Bool::getArgres;
/*****************************************************************************/

/*****************************************************************************/
Bool::Bool(  const bool value ): Basic(Type_Bool), m_value(value)
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
Bool::~Bool()
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
 BasicPtr Bool::iterateExp(Symbolics::Basic::Iterator &v)
/*****************************************************************************/
{
    return v.process_Arg( BasicPtr(this) );
}
/*****************************************************************************/

/*****************************************************************************/
bool Bool::operator==(  bool const& rhs ) const
/*****************************************************************************/
{
    return m_value == rhs;
}
/*****************************************************************************/

/*****************************************************************************/
bool Bool::operator==(  Bool const& rhs ) const
/*****************************************************************************/
{
    return m_value == rhs.getValue();
}
/*****************************************************************************/

/*****************************************************************************/
bool Bool::operator==(  Basic const& rhs ) const
/*****************************************************************************/
{
    // Typ pruefen
    switch (rhs.getType())
    {
    case Type_Bool:
        {
            const Bool &b = dynamic_cast<const Bool&>(rhs);
            return m_value == b.getValue();
        }
    case Type_Int:
        {
            const Int &i = dynamic_cast<const Int&>(rhs);
            return m_value == (i.getValue()==0?false:true);
        }
    case Type_Real:
        {
            const Real &r = dynamic_cast<const Real&>(rhs);
            return m_value == (r.getValue()==0.0?false:true);
        }
    default:
        return false;
    }
}
/*****************************************************************************/

/*****************************************************************************/
bool Bool::operator!=(  bool const& rhs ) const
/*****************************************************************************/
{
    return m_value != rhs;
}
/*****************************************************************************/

/*****************************************************************************/
bool Bool::operator!=(  Bool const& rhs ) const
/*****************************************************************************/
{
    return m_value != rhs.getValue();
}
/*****************************************************************************/

/*****************************************************************************/
bool Bool::operator!=(  Basic const& rhs ) const
/*****************************************************************************/
{
    // Typ pruefen
    switch (rhs.getType())
    {
    case Type_Zero:
        {
            return m_value != false;
        }
    case Type_Bool:
        {
            const Bool &b = dynamic_cast<const Bool&>(rhs);
            return m_value != b.getValue();
        }
    case Type_Int:
        {
            const Int &i = dynamic_cast<const Int&>(rhs);
            return m_value != (i.getValue()==0?false:true);
        }
    case Type_Real:
        {
            const Real &r = dynamic_cast<const Real&>(rhs);
            return m_value != (r.getValue()==0.0?false:true);
        }
    default:
        return true;
    }
}
/*****************************************************************************/

/*****************************************************************************/
bool Bool::operator<(  Basic const& rhs ) const
/*****************************************************************************/
{
    // Typ pruefen
    switch (rhs.getType())
    {
    case Type_Zero:
        {
            return (m_value?1:0) < 0;
        }
    case Type_Int:
        {
            const Int &i = dynamic_cast<const Int&>(rhs);
            return m_value < (i.getValue()==0?false:true);
        }
    case Type_Bool:
        {
            const Bool &r = dynamic_cast<const Bool&>(rhs);
            return m_value < r.getValue();
        }
    case Type_Real:
        {
            const Real &r = dynamic_cast<const Real&>(rhs);
            return m_value < (r.getValue()==0.0?false:true);
        }
    default:
        return getType() < rhs.getType();
    }
}
/*****************************************************************************/

 
/*****************************************************************************/
 BasicPtr Bool::subs( ConstBasicPtr const& old_exp, BasicPtr const& new_exp)
/*****************************************************************************/
{
    // Sind wir es selbst, die ersetzt werden sollen?
    if (*this == *old_exp.get())
        return new_exp;

    // uns selbs zurueckgeben
    return BasicPtr(this);
}
/*****************************************************************************/


/*****************************************************************************/
bool Symbolics::operator== (Bool const& lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    if (rhs->getType() == Type_Bool)
    {
        const Bool *crhs = Util::getAsConstPtr<Bool>(rhs);
        return lhs.getValue() == (*crhs).getValue();

    }
    return false;
}
/*****************************************************************************/
/*****************************************************************************/
bool Symbolics::operator== (BasicPtr const& lhs, Bool  const& rhs)
/*****************************************************************************/
{
    if (lhs->getType() == Type_Bool)
    {
        const Bool *clhs = Util::getAsConstPtr<Bool>(lhs);
        return rhs.getValue() == (*clhs).getValue();

    }
    return false;
}
/*****************************************************************************/

/*****************************************************************************/
bool Symbolics::operator!= (Bool const& lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    if (rhs->getType() == Type_Bool)
    {
        const Bool *crhs = Util::getAsConstPtr<Bool>(rhs);
        return lhs.getValue() != (*crhs).getValue();

    }
    return true;
}
/*****************************************************************************/
/*****************************************************************************/
bool Symbolics::operator!= (BasicPtr const& lhs, Bool  const& rhs)
/*****************************************************************************/
{
    if (lhs->getType() == Type_Bool)
    {
        const Bool *clhs = Util::getAsConstPtr<Bool>(lhs);
        return rhs.getValue() != (*clhs).getValue();

    }
    return true;
}
/*****************************************************************************/
