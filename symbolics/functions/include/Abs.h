#ifndef __ABS_H_
#define __ABS_H_

#include "UnaryOp.h"
#include "If.h"

namespace Symbolics
{
    class Abs: public UnaryOp
    {
    public:
        // Konstruktor
        Abs( BasicPtrVec const& args );
        // Konstruktor
        Abs( BasicPtr const& arg );
        // Destruktor
        ~Abs();

        // toString
        std::string toString() const;

        // Vereinfachen, wenn unver�ndert, dann NULL
        BasicPtr simplify();

        static BasicPtr New( BasicPtr const& arg);

        // derivative
        BasicPtr der();
        BasicPtr der(BasicPtr const& symbol);
    };

    typedef boost::intrusive_ptr< Abs> ConstAbsPtr;
    typedef boost::intrusive_ptr<Abs> AbsPtr;

};

#endif // __ABS_H_