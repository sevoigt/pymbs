#ifndef __ATAN2_H_
#define __ATAN2_H_

#include "BinaryOp.h"
#include "List.h"

namespace Symbolics
{

    class Atan2: public BinaryOp, public List
    {
    public:
        // Konstruktor
        Atan2( BasicPtrVec const& args );
        // Konstruktor
        Atan2( BasicPtr const& arg1, BasicPtr const& arg2 );
        // Destruktor
        ~Atan2();

        // toString
        std::string toString() const;

        // Vereinfachen, wenn unver�ndert, dann NULL
        BasicPtr simplify();

        static BasicPtr New( BasicPtr const& arg1, BasicPtr const& arg2);

        static inline ScopePtr  getScope() { return m_scope; };
        static inline void newScope() { m_scope = ScopePtr(new Scope()); };

        // derivative
        BasicPtr der();
        BasicPtr der(BasicPtr const& symbol);
    private:
        static ScopePtr m_scope;
	protected:
		void validate();

    };

    typedef boost::intrusive_ptr< Atan2> ConstAtan2Ptr;
    typedef boost::intrusive_ptr<Atan2> Atan2Ptr;

};

#endif // __ATAN2_H_