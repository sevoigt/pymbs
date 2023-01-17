#ifndef __SKEW_H_
#define __SKEW_H_

#include "UnaryOp.h"

namespace Symbolics
{

    class Skew: public UnaryOp
    {
    public:
        // Konstruktor
        Skew( BasicPtrVec const& args );
        // Konstruktor
        Skew( BasicPtr const& exp );
        // Destruktor
        ~Skew();

        // toString
        std::string toString() const;

        // Vereinfachen, wenn unver�ndert, dann NULL
        BasicPtr simplify();

        static BasicPtr New( BasicPtr const& arg);

        // derivative
        inline BasicPtr der() { return New(getArg()->der()); };
        inline BasicPtr der(BasicPtr const& symbol) { return New(getArg()->der(symbol)); };

	protected:
		void validate();
    };

    typedef boost::intrusive_ptr< Skew> ConstSkewPtr;
    typedef boost::intrusive_ptr<Skew> SkewPtr;

};

#endif // __SKEW_H_