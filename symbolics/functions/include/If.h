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

#ifndef __IF_H_
#define __IF_H_

#include "NaryOp.h"

namespace Symbolics
{
    class If: public NaryOp
    {
    public:
        // Konstruktor
        If( BasicPtr const& cond, BasicPtr const& arg1, BasicPtr const& arg2 );
        // Konstruktor
        If( BasicPtrVec const& args );
        // Destruktor
        ~If();

        // Vergleich
        bool operator==(  Basic const& rhs ) const ;

        // toString
        std::string toString() const;

        // Vereinfachen, wenn unverändert, dann NULL
        BasicPtr simplify();

        // lhs + rhs
        static BasicPtr New(BasicPtr const& cond, BasicPtr const& arg1, BasicPtr const& arg2);

        inline BasicPtr der() { return New(getArg(0),getArg(1)->der(),getArg(2)->der()); };
        inline BasicPtr der(BasicPtr const& symbol) { return New(getArg(0),getArg(1)->der(symbol),getArg(2)->der(symbol)); };
    };

};

#endif // __IF_H_