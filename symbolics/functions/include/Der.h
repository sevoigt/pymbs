#ifndef __DER_H_
#define __DER_H_

#include "UnaryOp.h"

namespace Symbolics
{

    class Der: public UnaryOp
    {
    public:
        // Konstruktor
        Der( BasicPtrVec const& args );
        // Konstruktor
        Der( BasicPtr const& arg );
        // Destruktor
        ~Der();

        // toString
        std::string toString() const;

        // Vereinfachen, wenn unver�ndert, dann NULL
        BasicPtr simplify();

        static BasicPtr New( BasicPtr const& arg);

        inline BasicPtr der() { return New(BasicPtr(this)); };
        BasicPtr der(BasicPtr const& symbol) { return New(getArg()->der(symbol)); };
    };

    typedef boost::intrusive_ptr< Der> ConstDerPtr;
    typedef boost::intrusive_ptr<Der> DerPtr;

};

#endif // __DER_H_