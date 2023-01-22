#ifndef __ATAN_H_
#define __ATAN_H_

#include "UnaryOp.h"
#include "List.h"

namespace Symbolics
{

    class Atan: public UnaryOp, public List
    {
    public:
        // Konstruktor
        Atan( BasicPtrVec const& args );
        // Konstruktor
        Atan( BasicPtr const& arg );
        // Destruktor
        ~Atan();

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

    typedef boost::intrusive_ptr< Atan> ConstAtanPtr;
    typedef boost::intrusive_ptr<Atan> AtanPtr;

};

#endif // __ATAN_H_