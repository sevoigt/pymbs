#ifndef __TRANSPOSE_H_
#define __TRANSPOSE_H_

#include "UnaryOp.h"

namespace Symbolics
{

    class Transpose: public UnaryOp
    {
    public:
        // Konstruktor
        Transpose( BasicPtrVec const& args );
        // Konstruktor
        Transpose( BasicPtr const& arg );
        // Destruktor
        ~Transpose();

        // toString
        std::string toString() const;

        // Vereinfachen, wenn unver�ndert, dann NULL
        BasicPtr simplify();

		    static BasicPtr New( BasicPtr const& arg );

        // derivative
        inline BasicPtr der() { return New(getArg()->der()); };
        inline BasicPtr der(BasicPtr const& symbol) { return New(getArg()->der(symbol)); };

    };

    typedef boost::intrusive_ptr< Transpose> ConstTransposePtr;
    typedef boost::intrusive_ptr<Transpose> TransposePtr;

};

#endif // __TRANSPOSE_H_