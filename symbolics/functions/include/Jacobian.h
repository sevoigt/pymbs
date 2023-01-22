#ifndef __JACOBIAN_H_
#define __JACOBIAN_H_

#include "BinaryOp.h"

namespace Symbolics
{

    class Jacobian: public BinaryOp
    {
    public:
        // Konstruktor
        Jacobian( BasicPtr const& exp, BasicPtr const&  symbols);
        // Konstruktor
        Jacobian( BasicPtrVec const& args );
        // Destruktor
        ~Jacobian();

        // toString
        std::string toString() const;

        // Vereinfachen
        BasicPtr simplify();

        static BasicPtr New( BasicPtr const& exp, BasicPtr const&  symbols);

        BasicPtr der();
        BasicPtr der(BasicPtr const& symbol);
	protected:
		void validate();
    };

    typedef boost::intrusive_ptr< Jacobian> ConstJacobianPtr;
    typedef boost::intrusive_ptr<Jacobian> JacobianPtr;

};

#endif // __JACOBIAN_H_