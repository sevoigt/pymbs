/*
This file is part of PyMbs.

PyMbs is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as 
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

PyMbs is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public 
License along with PyMbs.
If not, see <http://www.gnu.org/licenses/>.

Copyright 2011, 2012 Carsten Knoll, Christian Schubert, 
                     Jens Frenkel, Sebastian Voigt
*/

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