#ifndef __ASIN_H_
#define __ASIN_H_

#include "UnaryOp.h"
#include "List.h"

namespace Symbolics
{

    class Asin: public UnaryOp, public List
    {
    public:
        // Konstruktor
        Asin(  BasicPtrVec const&args );
        // Konstruktor
        Asin( BasicPtr const& arg );
        // Destruktor
        ~Asin();

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

    typedef boost::intrusive_ptr< Asin> ConstAsinPtr;
    typedef boost::intrusive_ptr<Asin> AsinPtr;

};

#endif // __ASIN_H_