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

#ifndef __ACOS_H_
#define __ACOS_H_

#include "UnaryOp.h"
#include "List.h"

namespace Symbolics
{

    class Acos: public UnaryOp, public List
    {
    public:
        // Konstruktor
        Acos( BasicPtrVec const& args );
        // Konstruktor
        Acos( BasicPtr const& arg );
        // Destruktor
        ~Acos();

        // toString
        std::string toString() const;

        // Vereinfachen, wenn unverändert, dann NULL
        BasicPtr simplify();

        static BasicPtr New( BasicPtr const& arg);

        static inline ScopePtr  getScope() { return m_scope; };
        static inline void newScope() { m_scope = ScopePtr(new Scope()); };

        // derivative
        BasicPtr der();
        BasicPtr der(BasicPtr const& symbol);

    private:
        static ScopePtr m_scope;
    };

    typedef boost::intrusive_ptr< Acos> ConstAcosPtr;
    typedef boost::intrusive_ptr<Acos> AcosPtr;

};

#endif // __ACOS_H_