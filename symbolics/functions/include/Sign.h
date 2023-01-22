#ifndef __SIGN_H_
#define __SIGN_H_

#include "UnaryOp.h"

namespace Symbolics
{

    class Sign: public UnaryOp
    {
    public:
        // Konstruktor
        Sign( BasicPtrVec const& args );
        // Konstruktor
        Sign( BasicPtr const& exp );
        // Destruktor
        ~Sign();

        // toString
        std::string toString() const;

        // Vereinfachen, wenn unver�ndert, dann NULL
        BasicPtr simplify();

        static BasicPtr New( BasicPtr const& arg);

        // derivative
        BasicPtr der();
        BasicPtr der(BasicPtr const& symbol);
    };

};

#endif // __SKEW_H_