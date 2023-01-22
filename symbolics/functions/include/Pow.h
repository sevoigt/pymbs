#ifndef __POW_H_
#define __POW_H_

#include "BinaryOp.h"

namespace Symbolics
{

    class Pow: public BinaryOp
    {
    public:
        // Konstruktor
        Pow( BasicPtrVec const& args );
        // Konstruktor
        Pow( BasicPtr const& arg1, BasicPtr const& arg2 ); 
        // Konstruktor
        Pow( BasicPtr const& arg1, int arg2 ); 
        // Konstruktor
        Pow( BasicPtr const& arg1, double arg2 ); 
        // Destruktor
        ~Pow();

        // Base und Exponent
        inline BasicPtr getBase() const { return getArg1(); };
        inline BasicPtr getExponent() const  { return getArg2(); };

        // toString
        std::string toString() const;

        // Vereinfachen, wenn unver�ndert
        BasicPtr simplify();

        static BasicPtr New( BasicPtr const& arg1, BasicPtr const& arg2);

        // derivative
        BasicPtr der();
        BasicPtr der(BasicPtr const& symbol);

	protected:
		void validate();
    };

    typedef boost::intrusive_ptr< Pow> ConstPowPtr;
    typedef boost::intrusive_ptr<Pow> PowPtr;

};

#endif // __POW_H_