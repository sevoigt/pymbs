#ifndef __EQUAL_H_
#define __EQUAL_H_

#include "BinaryOp.h"

namespace Symbolics
{
    class Equal: public BinaryOp
    {
    public:
        // Konstruktor
        Equal( BasicPtrVec const& args );
        // Konstruktor
        Equal( BasicPtr const& arg1, BasicPtr const& arg2 );
        // Destruktor
        ~Equal();

        // toString
        std::string toString() const;

        // Vereinfachen, wenn unver�ndert, dann NULL
        BasicPtr simplify();

        static BasicPtr New( BasicPtr const& arg1, BasicPtr const& arg2 );

        // derivative
        inline BasicPtr der() { return New(getArg1()->der(),getArg2()->der()); };
        inline BasicPtr der(BasicPtr const& symbol) { return New(getArg1()->der(symbol),getArg2()->der(symbol)); };
    };
};

#endif // __GREATER_H_