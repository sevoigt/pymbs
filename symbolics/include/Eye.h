#ifndef __EYE_H_
#define __EYE_H_

#include "Basic.h"
#include "Zero.h"
#include "str.h"

namespace Symbolics
{
/*****************************************************************************/
    class Eye: public Basic
    {
    public:
        Eye();
        Eye(Shape const& s);

        ~Eye();

        inline std::string toString() const { return "1"; };

        inline BasicPtr simplify() { return BasicPtr(this); };

        bool operator==(  Basic const& rhs ) const;
        bool operator!=(  Basic const& rhs ) const { return m_type != rhs.getType(); };
        bool operator<(Basic const& rhs) const { return m_type < rhs.getType(); };

        template< typename T >
        BasicPtr operator* ( T const& t) { return BasicPtr(this); };

        template< typename T >
        T operator+ ( T const& t) { return t; };

        // Ersetzen
        BasicPtr subs( ConstBasicPtr const& old_exp,  BasicPtr const& new_exp);
        BasicPtr iterateExp(Symbolics::Basic::Iterator &v);

        // atoms
        inline void getAtoms(BasicSet &atoms) {};
        inline void getAtoms(BasicSizeTMap &atoms) {};

        // args
        inline size_t getArgsSize() const { return 0; };
        inline BasicPtr const& getArg(size_t i) const { return getArgres; };
    
        inline static BasicPtr & getEye() { return eye; };
        static BasicPtr getEye(Shape const& s);

        // derivative
        inline BasicPtr der() { return Zero::getZero(m_shape); };
        inline BasicPtr der(BasicPtr const& symbol) { return Zero::getZero(m_shape); };
    protected:
        static BasicPtr eye;

        static BasicPtr getArgres;

    };
/*****************************************************************************/

    // Operatoren mit Eye
    bool operator== (Eye const& lhs, BasicPtr  const& rhs);
    bool operator!= (Eye const& lhs, BasicPtr  const& rhs);
    bool operator== (BasicPtr const& lhs, Eye  const& rhs);
    bool operator!= (BasicPtr const& lhs, Eye  const& rhs);

/*****************************************************************************/

};

#endif // __EYE_H_
