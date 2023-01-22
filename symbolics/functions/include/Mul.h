#ifndef __MUL_H_
#define __MUL_H_

#include "NaryOp.h"

namespace Symbolics
{

    class Mul: public NaryOp
    {
    public:
        // Konstruktor
        Mul( BasicPtr const& arg1, BasicPtr const& arg2 );
        // Konstruktor
        Mul( BasicPtrVec const& args );

        // Destruktor
        ~Mul();

        // Vergleich
        bool operator==( Basic const& rhs) const;

        // toString
        std::string toString() const;

        // Vereinfachen, wenn unver�ndert, dann NULL
        BasicPtr simplify();

        static BasicPtr New(BasicPtr const& lhs,BasicPtr const& rhs);

        // derivative
        BasicPtr der();
        BasicPtr der(BasicPtr const& symbol);
    };

    typedef boost::intrusive_ptr< Mul> ConstMulPtr;
    typedef boost::intrusive_ptr<Mul> MulPtr;

};

#endif // __MUL_H_