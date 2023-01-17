#ifndef __INVERSE_H_
#define __INVERSE_H_

#include "UnaryOp.h"
#include "Der.h"

namespace Symbolics
{

    class Inverse: public UnaryOp
    {
    public:
        // Konstruktor
        Inverse( BasicPtrVec const& args );
        // Konstruktor
        Inverse( BasicPtr const& arg );
        // Destruktor
        ~Inverse();

        // toString
        std::string toString() const;

        // Vereinfachen, wenn unver�ndert, dann NULL
        BasicPtr simplify();

		    static BasicPtr New( BasicPtr const& arg );

        // derivative
        inline BasicPtr der() { return Der::New(BasicPtr(this)); };
        inline BasicPtr der(BasicPtr const& symbol) { return Der::New(BasicPtr(this)); };

    };

    typedef boost::intrusive_ptr< Inverse> ConstInversePtr;
    typedef boost::intrusive_ptr<Inverse> InversePtr;

};

#endif // __TRANSPOSE_H_