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

#ifndef __REAL_H_
#define __REAL_H_

#include "Basic.h"
#include "Int.h"
#include "str.h"

namespace Symbolics
{

/*****************************************************************************/
    class Int;
    class Real;
    typedef boost::intrusive_ptr<Real> RealPtr;
    typedef boost::intrusive_ptr< Real> ConstRealPtr;
/*****************************************************************************/
    class Real: public Basic
    {
    public:
        // Konstruktor
        Real(  const double value );
        Real(  const Int i );
        ~Real();
        
        static BasicPtr New( const double r) { return BasicPtr(new Real(r)); };

        inline double getValue() const { return m_value; };

        inline std::string toString() const { return str(m_value); };

        inline BasicPtr simplify() { return BasicPtr(this); };

        bool operator==(  double const& rhs ) const;
        bool operator==(  Basic const& rhs ) const;
        bool operator==(  Real const& rhs ) const;
        bool operator!=(  double const& rhs ) const;
        bool operator!=(  Basic const& rhs ) const;
        bool operator!=(  Real const& rhs ) const;
        bool operator<(Basic  const& rhs) const;
        Real operator+ ( Int const& rhs);
        Real operator- ( Int const& rhs);
        Real operator* ( Int const& rhs);
        Real operator+ ( Real const& rhs);
        Real operator- ( Real const& rhs);
        Real operator* ( Real const& rhs);

        Real operator+ ( int const& rhs);
        Real operator- ( int const& rhs);
        Real operator* ( int const& rhs);
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

        // derivative
        inline BasicPtr der() { return Zero::getZero(); };
        inline BasicPtr der(BasicPtr const& symbol) { return Zero::getZero(); };
        
    protected:
         double m_value;

         static BasicPtr getArgres;
    };

    // Operatoren mit Real
    BasicPtr operator+ (Real lhs, BasicPtr  const& rhs);
    BasicPtr operator+ (BasicPtr const& lhs, Real rhs);
    BasicPtr operator- (Real lhs, BasicPtr  const& rhs);
    BasicPtr operator- (BasicPtr const& lhs, Real rhs);
    BasicPtr operator* (Real lhs, BasicPtr  const& rhs);
    BasicPtr operator* (BasicPtr const& lhs, Real rhs);
    bool operator== (Real const& lhs, BasicPtr  const& rhs);
    bool operator!= (Real const& lhs, BasicPtr  const& rhs);
    bool operator== (BasicPtr const& lhs, Real  const& rhs);
    bool operator!= (BasicPtr const& lhs, Real  const& rhs);

/*****************************************************************************/

};

#endif // __REAL_H_
