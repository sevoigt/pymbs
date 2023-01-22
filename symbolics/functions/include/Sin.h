#ifndef __SIN_H_
#define __SIN_H_

#include "UnaryOp.h"
#include "List.h"

namespace Symbolics
{

    class Sin: public UnaryOp, public List
    {
    public:
        // Konstruktor
        Sin( BasicPtrVec const& args );
        // Konstruktor
        Sin( BasicPtr const& arg);
        // Destruktor
        ~Sin();

        // toString
        std::string toString() const;

        // Vereinfachen, wenn unver�ndert, dann NULL
        BasicPtr simplify();

        static BasicPtr New( BasicPtr const& arg);

        static inline ScopePtr  getScope() { return m_scope; };
        static inline void newScope() { m_scope = ScopePtr(new Scope()); };

        // trigonometrische Vereinfachung
        static void tricksimplify();

        // derivative
        BasicPtr der();
        BasicPtr der(BasicPtr const& symbol);
    private:
        static ScopePtr m_scope;

    };

    typedef boost::intrusive_ptr< Sin> ConstSinPtr;
    typedef boost::intrusive_ptr<Sin> SinPtr;

};

#endif // __SIN_H_