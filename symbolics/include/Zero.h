#ifndef __ZERO_H_
#define __ZERO_H_

#include "Basic.h"
#include "str.h"

namespace Symbolics
{
/*****************************************************************************/
    class Zero: public Basic
    {
    public:
        Zero();
        Zero(Shape const& s);

        ~Zero();

        inline std::string toString() const { return "0"; };

        inline BasicPtr simplify() { return BasicPtr(this); };

        bool operator==(  Basic const& rhs ) const;
        bool operator!=(  Basic const& rhs ) const { return m_type != rhs.getType(); };
        bool operator<( Basic const& rhs ) const { return m_type < rhs.getType(); };

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
    
        inline static BasicPtr & getZero() { return zero; };
        static BasicPtr getZero(Shape const& s);
        static BasicPtr getExactZero(Shape const& s);

        // derivative
        inline BasicPtr der() { return BasicPtr(this); };
        inline BasicPtr der(BasicPtr const& symbol) { return BasicPtr(this); };

    protected:
        static BasicPtr zero;

        static BasicPtr getArgres;

    };
/*****************************************************************************/
    // Operatoren mit Zero
    bool operator== (Zero const& lhs, BasicPtr  const& rhs);
    bool operator!= (Zero const& lhs, BasicPtr  const& rhs);
    bool operator== (BasicPtr const& lhs, Zero  const& rhs);
    bool operator!= (BasicPtr const& lhs, Zero  const& rhs);
/*****************************************************************************/

};

#endif // __ZERO_H_
