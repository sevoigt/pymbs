#ifndef __SOLVE_H_
#define __SOLVE_H_

#include "BinaryOp.h"

namespace Symbolics
{

    class Solve: public BinaryOp
    {
    public:
        // Konstruktor
        Solve( BasicPtrVec const& args );
        // Konstruktor
        Solve( BasicPtr const& arg1, BasicPtr const& arg2 ); 
        // Destruktor
        ~Solve();

        // toString
        std::string toString() const;

        // Vereinfachen, wenn unver�ndert, dann NULL
        BasicPtr simplify();

        static BasicPtr New( BasicPtr const& arg1, BasicPtr const& arg2);

        // derivative
        BasicPtr der();
        BasicPtr der(BasicPtr const& symbol);

    protected:

		void validate();

    };

    typedef boost::intrusive_ptr< Solve> ConstSolvePtr;
    typedef boost::intrusive_ptr<Solve> SolvePtr;

};

#endif // __POW_H_