#ifndef __GREATER_H_
#define __GREATER_H_

#include "BinaryOp.h"

namespace Symbolics
{
    class Greater: public BinaryOp
    {
    public:
        // Konstruktor
        Greater( BasicPtrVec const& args );
        // Konstruktor
        Greater( BasicPtr const& arg1, BasicPtr const& arg2 );
        // Destruktor
        ~Greater();

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