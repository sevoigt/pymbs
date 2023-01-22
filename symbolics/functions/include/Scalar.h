#ifndef __SCALAR_H_
#define __SCALAR_H_

#include "UnaryOp.h"

namespace Symbolics
{
    class Scalar: public UnaryOp
    {
    public:
        // Konstruktor
        Scalar( BasicPtrVec const& args );
        // Konstruktor
        Scalar( BasicPtr const& arg );
        // Destruktor
        ~Scalar();

        // toString
        std::string toString() const;

        // Vereinfachen, wenn unveraendert, dann NULL
        BasicPtr simplify();

        static BasicPtr New(BasicPtr const& e);

        // derivative
        inline BasicPtr der() { return New(getArg()->der()); };
        inline BasicPtr der(BasicPtr const& symbol) { return New(getArg()->der(symbol)); };

	protected:
		void validate();

    };

    typedef boost::intrusive_ptr< Scalar> ConstScalarPtr;
    typedef boost::intrusive_ptr<Scalar> ScalarPtr;

};

#endif // __SCALAR_H_