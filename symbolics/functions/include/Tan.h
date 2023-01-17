#ifndef __TAN_H_
#define __TAN_H_

#include "UnaryOp.h"
#include "List.h"

namespace Symbolics
{

    class Tan: public UnaryOp, public List
    {
    public:
        // Konstruktor
        Tan( BasicPtrVec const& args );
        // Konstruktor
        Tan( BasicPtr const& arg);
        // Destruktor
        ~Tan();

        // toString
        std::string toString() const;

        // Vereinfachen, wenn unver�ndert, dann NULL
        BasicPtr simplify();

        static BasicPtr New( BasicPtr const& arg);

        static inline ScopePtr  getScope() { return m_scope; };
        static inline void newScope() { m_scope = ScopePtr(new Scope()); };

        // derivative
        BasicPtr der();
        BasicPtr der(BasicPtr const& symbol);
    private:
        static ScopePtr m_scope;
    };

    typedef boost::intrusive_ptr< Tan> ConstTanPtr;
    typedef boost::intrusive_ptr<Tan> TanPtr;

};

#endif // __TAN_H_