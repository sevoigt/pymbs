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

#ifndef __OUTER_H_
#define __OUTER_H_

#include "BinaryOp.h"

namespace Symbolics
{

    class Outer: public BinaryOp
    {
    public:
        // Konstruktor
        Outer( BasicPtrVec const& args );
        // Konstruktor
        Outer( BasicPtr const& arg1, BasicPtr const& arg2 );
        // Destruktor
        ~Outer();

        // toString
        std::string toString() const;

        // Vereinfachen, wenn unverändert, dann NULL
        BasicPtr simplify();

        static BasicPtr New( BasicPtr const& arg1, BasicPtr const& arg2 );

        // derivative
        inline BasicPtr der() { return New(getArg1()->der(), getArg2()->der()); };
        inline BasicPtr der(BasicPtr const& symbol) { return New(getArg1()->der(symbol), getArg2()->der(symbol)); };

	protected:
		void validate();
    };

    typedef boost::intrusive_ptr<const Outer> ConstOuterPtr;
    typedef boost::intrusive_ptr<Outer> OuterPtr;

};

#endif // __OUTER_H_