#ifndef __NEG_H_
#define __NEG_H_

#include "UnaryOp.h"

namespace Symbolics
{

    class Neg: public UnaryOp
    {
    public:
        // Konstruktor
        Neg( BasicPtrVec const& args );
        // Konstruktor
        Neg( BasicPtr const& arg );
        // Destruktor
        ~Neg();

        // toString
        std::string toString() const;

        // Vereinfachen, wenn unver�ndert, dann NULL
        BasicPtr simplify();

        static BasicPtr New(BasicPtr const& e);

        // derivative
        inline BasicPtr der() { return New(getArg()->der()); };
        inline BasicPtr der(BasicPtr const& symbol) { return New(getArg()->der(symbol)); };
    };

    typedef boost::intrusive_ptr< Neg> ConstNegPtr;
    typedef boost::intrusive_ptr<Neg> NegPtr;

};

#endif // __NEG_H_