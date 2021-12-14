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

#ifndef __DER_H_
#define __DER_H_

#include "UnaryOp.h"

namespace Symbolics
{

    class Der: public UnaryOp
    {
    public:
        // Konstruktor
        Der( BasicPtrVec const& args );
        // Konstruktor
        Der( BasicPtr const& arg );
        // Destruktor
        ~Der();

        // toString
        std::string toString() const;

        // Vereinfachen, wenn unverändert, dann NULL
        BasicPtr simplify();

        static BasicPtr New( BasicPtr const& arg);

        inline BasicPtr der() { return New(BasicPtr(this)); };
        BasicPtr der(BasicPtr const& symbol) { return New(getArg()->der(symbol)); };
    };

    typedef boost::intrusive_ptr< Der> ConstDerPtr;
    typedef boost::intrusive_ptr<Der> DerPtr;

};

#endif // __DER_H_