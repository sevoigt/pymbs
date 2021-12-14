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

#ifndef __BINARY_OP_H_
#define __BINARY_OP_H_

#include "Basic.h"
#include <vector>

namespace Symbolics
{

    class BinaryOp: public Basic
    {
    public:
        // Konstruktor
        BinaryOp(  Basic_Type type,  BasicPtrVec const& args );
        // Konstruktor
        BinaryOp(  Basic_Type type, BasicPtr const& arg1, BasicPtr const& arg2 );
        // Destruktor
        virtual ~BinaryOp();

        // Vergleich
        bool operator==(  Basic const& rhs ) const;
        bool operator<(Basic  const& rhs) const;

        // Argumente bekommen
        inline BasicPtr const& getArg1() const { return m_arg1.getArg(); };
        inline BasicPtr const& getArg2()const { return m_arg2.getArg(); };

        // args
        inline size_t getArgsSize() const { return 2; };
        inline BasicPtr const& getArg(size_t i) const 
        {
          switch(i)
          {
          case 0: return getArg1();
          case 1: return getArg2();
          default: 
            throw InternalError("BinaryOp hase only two args");
          }
          return getArgres;
        };

        // Ersetzen
        BasicPtr subs( ConstBasicPtr const& old_exp,  BasicPtr const& new_exp);
        
        // Atoms
        void getAtoms(Basic::BasicSet& atoms);
        void getAtoms(BasicSizeTMap &atoms);

    protected:
        inline void setArg1( BasicPtr const& arg) { m_arg1.setArg(arg); };
        inline void setArg2( BasicPtr const& arg) { m_arg2.setArg(arg); };
        inline void simplifyArgs() { setArg1(getArg1()->simplify()); setArg2(getArg2()->simplify()); };

    private:
        // Arguments
        Argument m_arg1;
        Argument m_arg2;

        static BasicPtr getArgres;
    };
    typedef boost::intrusive_ptr< const BinaryOp > ConstBinaryOpPtr;
    typedef boost::intrusive_ptr<  BinaryOp > BinaryOpPtr;

};

#endif // __BINARY_OP_H_