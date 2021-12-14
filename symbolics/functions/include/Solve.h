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

        // Vereinfachen, wenn unverändert, dann NULL
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