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

#ifndef __BOOL_H_
#define __BOOL_H_

#include "Basic.h"
#include "Real.h"
#include "str.h"

namespace Symbolics
{

/*****************************************************************************/
    class Bool;
    typedef boost::intrusive_ptr<Bool> BoolPtr;
    typedef boost::intrusive_ptr< Bool> ConstBoolPtr;
/*****************************************************************************/
    class Bool: public Basic
    {
    public:
        // Konstruktor
        Bool(  const bool value );
        ~Bool();
        
        inline bool getValue() const { return m_value; };

        static BasicPtr New( const bool b) { return BasicPtr(new Bool(b)); };

        inline std::string toString() const { return str(m_value); };

        inline BasicPtr simplify() { return BasicPtr(this); };

        bool operator==(  bool const& rhs ) const;
        bool operator==(  Basic const& rhs ) const;
        bool operator==(  Bool const& rhs ) const;
        bool operator!=(  bool const& rhs ) const;
        bool operator!=(  Basic const& rhs ) const;
        bool operator!=(  Bool const& rhs ) const;
		    bool operator<( Basic  const& rhs ) const;

        // Ersetzen
         BasicPtr subs( ConstBasicPtr const& old_exp,  BasicPtr const& new_exp);
         BasicPtr iterateExp(Symbolics::Basic::Iterator &v);

        // atoms
         inline void getAtoms(BasicSet &atoms) {};
         inline void getAtoms(BasicSizeTMap &atoms) {};

        // args
        inline size_t getArgsSize() const { return 0; };
        inline BasicPtr const& getArg(size_t i) const { return getArgres; };

        // derivative
        inline BasicPtr der() { throw InternalError("Derivative of Bool does not exist!"); return Zero::getZero(); };
        inline BasicPtr der(BasicPtr const& symbol) { return Zero::getZero(); };
        
    protected:
         bool m_value;

         static BasicPtr getArgres;
    };

    // Operatoren mit Bool
    bool operator== (Bool const& lhs, BasicPtr  const& rhs);
    bool operator!= (Bool const& lhs, BasicPtr  const& rhs);
    bool operator== (BasicPtr const& lhs, Bool  const& rhs);
    bool operator!= (BasicPtr const& lhs, Bool  const& rhs);

/*****************************************************************************/

};

#endif // __BOOL_H_
