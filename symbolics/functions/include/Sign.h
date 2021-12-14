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

#ifndef __SIGN_H_
#define __SIGN_H_

#include "UnaryOp.h"

namespace Symbolics
{

    class Sign: public UnaryOp
    {
    public:
        // Konstruktor
        Sign( BasicPtrVec const& args );
        // Konstruktor
        Sign( BasicPtr const& exp );
        // Destruktor
        ~Sign();

        // toString
        std::string toString() const;

        // Vereinfachen, wenn unverändert, dann NULL
        BasicPtr simplify();

        static BasicPtr New( BasicPtr const& arg);

        // derivative
        BasicPtr der();
        BasicPtr der(BasicPtr const& symbol);
    };

};

#endif // __SKEW_H_