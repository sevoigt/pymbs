#ifndef __IF_H_
#define __IF_H_

#include "NaryOp.h"

namespace Symbolics
{
    class If: public NaryOp
    {
    public:
        // Konstruktor
        If( BasicPtr const& cond, BasicPtr const& arg1, BasicPtr const& arg2 );
        // Konstruktor
        If( BasicPtrVec const& args );
        // Destruktor
        ~If();

        // Vergleich
        bool operator==(  Basic const& rhs ) const ;

        // toString
        std::string toString() const;

        // Vereinfachen, wenn unver�ndert, dann NULL
        BasicPtr simplify();

        // lhs + rhs
        static BasicPtr New(BasicPtr const& cond, BasicPtr const& arg1, BasicPtr const& arg2);

        inline BasicPtr der() { return New(getArg(0),getArg(1)->der(),getArg(2)->der()); };
        inline BasicPtr der(BasicPtr const& symbol) { return New(getArg(0),getArg(1)->der(symbol),getArg(2)->der(symbol)); };
    };

};

#endif // __IF_H_