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

#ifndef __ZERO_H_
#define __ZERO_H_

#include "Basic.h"
#include "str.h"

namespace Symbolics
{
/*****************************************************************************/
    class Zero: public Basic
    {
    public:
        Zero();
        Zero(Shape const& s);

        ~Zero();

        inline std::string toString() const { return "0"; };

        inline BasicPtr simplify() { return BasicPtr(this); };

        bool operator==(  Basic const& rhs ) const;
        bool operator!=(  Basic const& rhs ) const { return m_type != rhs.getType(); };
        bool operator<( Basic const& rhs ) const { return m_type < rhs.getType(); };

        template< typename T >
        BasicPtr operator* ( T const& t) { return BasicPtr(this); };

        template< typename T >
        T operator+ ( T const& t) { return t; };

        // Ersetzen
        BasicPtr subs( ConstBasicPtr const& old_exp,  BasicPtr const& new_exp);
        BasicPtr iterateExp(Symbolics::Basic::Iterator &v);

        // atoms
        inline void getAtoms(BasicSet &atoms) {};
        inline void getAtoms(BasicSizeTMap &atoms) {};

        // args
        inline size_t getArgsSize() const { return 0; };
        inline BasicPtr const& getArg(size_t i) const { return getArgres; };
    
        inline static BasicPtr & getZero() { return zero; };
        static BasicPtr getZero(Shape const& s);
        static BasicPtr getExactZero(Shape const& s);

        // derivative
        inline BasicPtr der() { return BasicPtr(this); };
        inline BasicPtr der(BasicPtr const& symbol) { return BasicPtr(this); };

    protected:
        static BasicPtr zero;

        static BasicPtr getArgres;

    };
/*****************************************************************************/
    // Operatoren mit Zero
    bool operator== (Zero const& lhs, BasicPtr  const& rhs);
    bool operator!= (Zero const& lhs, BasicPtr  const& rhs);
    bool operator== (BasicPtr const& lhs, Zero  const& rhs);
    bool operator!= (BasicPtr const& lhs, Zero  const& rhs);
/*****************************************************************************/

};

#endif // __ZERO_H_
