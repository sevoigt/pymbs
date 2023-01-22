#ifndef __COS_H_
#define __COS_H_

#include "UnaryOp.h"
#include "List.h"

namespace Symbolics
{

    class Cos: public UnaryOp, public List
    {
    public:
        // Konstruktor
        Cos( BasicPtrVec const& args );
        // Konstruktor
        Cos( BasicPtr const& arg);
        // Destruktor
        ~Cos();

        // toString
        std::string toString() const;

        // Vereinfachen, wenn unver�ndert, dann NULL
        BasicPtr simplify();

        static BasicPtr New( BasicPtr const& arg);

        static inline ScopePtr  getScope() { return m_scope; };
        static inline void newScope() { m_scope = ScopePtr(new Scope()); };

        BasicPtr der();
        BasicPtr der(BasicPtr const& symbol);
    private:
        static ScopePtr m_scope;
    };

    typedef boost::intrusive_ptr< Cos> ConstCosPtr;
    typedef boost::intrusive_ptr<Cos> CosPtr;

};

#endif // __COS_H_