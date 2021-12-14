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

#ifndef __ADD_H_
#define __ADD_H_

#include "NaryOp.h"

namespace Symbolics
{
    class Add: public NaryOp
    {
    public:
        // Konstruktor
        Add( BasicPtr const& arg1, BasicPtr const& arg2 );
        // Konstruktor
        Add( BasicPtrVec const& args );
        // Destruktor
        ~Add();

        // Vergleich
        bool operator==(  Basic const& rhs ) const ;

        // toString
        std::string toString() const;

        // Vereinfachen, wenn unverändert, dann NULL
        BasicPtr simplify();

        // lhs + rhs
        static BasicPtr New(BasicPtr const& lhs, BasicPtr const& rhs);

        // derivative
        BasicPtr der();
        BasicPtr der(BasicPtr const& symbol);
    };

    typedef boost::intrusive_ptr< Add> ConstAddPtr;
    typedef boost::intrusive_ptr<Add> AddPtr;
};

#endif // __ADD_H_