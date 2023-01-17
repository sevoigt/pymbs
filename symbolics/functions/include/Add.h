#ifndef __ADD_H_
#define __ADD_H_

#include "NaryOp.h"

namespace Symbolics
{
    class Add: public NaryOp
    {
    public:
        // Konstruktor
        Add( BasicPtr const& arg1, BasicPtr const& arg2 );
        // Konstruktor
        Add( BasicPtrVec const& args );
        // Destruktor
        ~Add();

        // Vergleich
        bool operator==(  Basic const& rhs ) const ;

        // toString
        std::string toString() const;

        // Vereinfachen, wenn unver�ndert, dann NULL
        BasicPtr simplify();

        // lhs + rhs
        static BasicPtr New(BasicPtr const& lhs, BasicPtr const& rhs);

        // derivative
        BasicPtr der();
        BasicPtr der(BasicPtr const& symbol);
    };

    typedef boost::intrusive_ptr< Add> ConstAddPtr;
    typedef boost::intrusive_ptr<Add> AddPtr;
};

#endif // __ADD_H_