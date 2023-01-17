#ifndef __LESS_H_
#define __LESS_H_

#include "BinaryOp.h"

namespace Symbolics
{
    class Less: public BinaryOp
    {
    public:
        // Konstruktor
        Less( BasicPtrVec const& args );
        // Konstruktor
        Less( BasicPtr const& arg1, BasicPtr const& arg2 );
        // Destruktor
        ~Less();

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

#endif // __LESS_H_