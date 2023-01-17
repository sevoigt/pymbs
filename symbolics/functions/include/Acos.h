#ifndef __ACOS_H_
#define __ACOS_H_

#include "UnaryOp.h"
#include "List.h"

namespace Symbolics
{

    class Acos: public UnaryOp, public List
    {
    public:
        // Konstruktor
        Acos( BasicPtrVec const& args );
        // Konstruktor
        Acos( BasicPtr const& arg );
        // Destruktor
        ~Acos();

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

    typedef boost::intrusive_ptr< Acos> ConstAcosPtr;
    typedef boost::intrusive_ptr<Acos> AcosPtr;

};

#endif // __ACOS_H_