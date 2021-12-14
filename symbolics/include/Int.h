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

#ifndef __INT_H_
#define __INT_H_

#include "Basic.h"
#include "Zero.h"
#include "Real.h"
#include "str.h"

namespace Symbolics
{

/*****************************************************************************/
    class Int;
    typedef boost::intrusive_ptr<Int> IntPtr;
    typedef boost::intrusive_ptr< Int> ConstIntPtr;
	class Real;
/*****************************************************************************/
    class Int: public Basic
    {
    public:
        // Konstruktor
        Int(  const int value );
        ~Int();

        static BasicPtr New( const int i);

        inline int getValue() const  { return m_value; };

        inline std::string toString() const { return str(m_value); };

        inline BasicPtr simplify() { return BasicPtr(this); };

        bool operator==(  Int const& rhs ) const;
        bool operator==(  Basic const& rhs ) const;
        bool operator==(  int const& rhs ) const;
        bool operator!=(  Int const& rhs ) const;
        bool operator!=(  Basic const& rhs ) const;
        bool operator!=(  int const& rhs ) const;
        bool operator<(Basic const& rhs) const;
        Int operator+ ( Int const& rhs);
        Int operator- ( Int const& rhs);
        Int operator* ( Int const& rhs);
        Real operator+ ( Real const& rhs);
        Real operator- ( Real const& rhs);
        Real operator* ( Real const& rhs);

        Int operator+ ( int const& rhs);
        Int operator- ( int const& rhs);
        Int operator* ( int const& rhs);
        Real operator+ ( double const& rhs);
        Real operator- ( double const& rhs);
        Real operator* ( double const& rhs);

        // Ersetzen
        BasicPtr subs( ConstBasicPtr const& old_exp,  BasicPtr const& new_exp);
        BasicPtr iterateExp(Symbolics::Basic::Iterator &v);

        // atoms
        inline void getAtoms(BasicSet &atoms) {};
        inline void getAtoms(BasicSizeTMap &atoms) {};

        // args
        inline size_t getArgsSize() const { return 0; };
        inline BasicPtr const& getArg(size_t i) const { return getArgres; };
        
        // 0,1,-1
        inline static BasicPtr & getZero() { return Zero::getZero(); };
        inline static BasicPtr & getOne() { return one; };
        inline static BasicPtr & getMinusOne() { return minusOne; };

        // derivative
        inline BasicPtr der() { return Zero::getZero(); };
        inline BasicPtr der(BasicPtr const& symbol) { return Zero::getZero(); };

    protected:
        int m_value;

        static BasicPtr one;
        static BasicPtr minusOne;

        static BasicPtr getArgres;
    };

    // Operatoren mit Int
    BasicPtr operator+ (Int lhs, BasicPtr  const& rhs);
    BasicPtr operator+ (BasicPtr const& lhs, Int rhs);
    BasicPtr operator- (Int lhs, BasicPtr  const& rhs);
    BasicPtr operator- (BasicPtr const& lhs, Int rhs);
    BasicPtr operator* (Int lhs, BasicPtr  const& rhs);
    BasicPtr operator* (BasicPtr const& lhs, Int rhs);
    bool operator== (Int const& lhs, BasicPtr  const& rhs);
    bool operator!= (Int const& lhs, BasicPtr  const& rhs);
    bool operator== (BasicPtr const& lhs, Int  const& rhs);
    bool operator!= (BasicPtr const& lhs, Int  const& rhs);


/*****************************************************************************/

};

#endif // __INT_H_
