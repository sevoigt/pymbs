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

#ifndef __GREATER_H_
#define __GREATER_H_

#include "BinaryOp.h"

namespace Symbolics
{
    class Greater: public BinaryOp
    {
    public:
        // Konstruktor
        Greater( BasicPtrVec const& args );
        // Konstruktor
        Greater( BasicPtr const& arg1, BasicPtr const& arg2 );
        // Destruktor
        ~Greater();

        // toString
        std::string toString() const;

        // Vereinfachen, wenn unverändert, dann NULL
        BasicPtr simplify();

        static BasicPtr New( BasicPtr const& arg1, BasicPtr const& arg2 );

        // derivative
        inline BasicPtr der() { return New(getArg1()->der(),getArg2()->der()); };
        inline BasicPtr der(BasicPtr const& symbol) { return New(getArg1()->der(symbol),getArg2()->der(symbol)); };
    };
};

#endif // __GREATER_H_