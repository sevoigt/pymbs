#ifndef __OUTER_H_
#define __OUTER_H_

#include "BinaryOp.h"

namespace Symbolics
{

    class Outer: public BinaryOp
    {
    public:
        // Konstruktor
        Outer( BasicPtrVec const& args );
        // Konstruktor
        Outer( BasicPtr const& arg1, BasicPtr const& arg2 );
        // Destruktor
        ~Outer();

        // toString
        std::string toString() const;

        // Vereinfachen, wenn unver�ndert, dann NULL
        BasicPtr simplify();

        static BasicPtr New( BasicPtr const& arg1, BasicPtr const& arg2 );

        // derivative
        inline BasicPtr der() { return New(getArg1()->der(), getArg2()->der()); };
        inline BasicPtr der(BasicPtr const& symbol) { return New(getArg1()->der(symbol), getArg2()->der(symbol)); };

	protected:
		void validate();
    };

    typedef boost::intrusive_ptr<const Outer> ConstOuterPtr;
    typedef boost::intrusive_ptr<Outer> OuterPtr;

};

#endif // __OUTER_H_