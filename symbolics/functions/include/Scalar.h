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

#ifndef __SCALAR_H_
#define __SCALAR_H_

#include "UnaryOp.h"

namespace Symbolics
{
    class Scalar: public UnaryOp
    {
    public:
        // Konstruktor
        Scalar( BasicPtrVec const& args );
        // Konstruktor
        Scalar( BasicPtr const& arg );
        // Destruktor
        ~Scalar();

        // toString
        std::string toString() const;

        // Vereinfachen, wenn unverändert, dann NULL
        BasicPtr simplify();

        static BasicPtr New(BasicPtr const& e);

        // derivative
        inline BasicPtr der() { return New(getArg()->der()); };
        inline BasicPtr der(BasicPtr const& symbol) { return New(getArg()->der(symbol)); };

	protected:
		void validate();

    };

    typedef boost::intrusive_ptr< Scalar> ConstScalarPtr;
    typedef boost::intrusive_ptr<Scalar> ScalarPtr;

};

#endif // __SCALAR_H_