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

#ifndef __UNARY_OP_H_
#define __UNARY_OP_H_

#include "Basic.h"
#include <vector>

namespace Symbolics
{

    class UnaryOp: public Basic
    {
    public:
        // Konstruktor
        UnaryOp(  Basic_Type type,  BasicPtrVec const& args );        
        // Konstruktor
        UnaryOp(  Basic_Type type, BasicPtr const& arg );
        // Destruktor
        virtual ~UnaryOp();

        // Vergleichsoperator
        bool operator==( Basic const& rhs) const;
        bool operator<(Basic  const& rhs) const;

        // Argument bekommen
        inline BasicPtr const& getArg() const { return m_arg.getArg(); };

        inline void getAtoms(Basic::BasicSet &atoms) { getArg()->getAtoms(atoms);} ;
        inline void getAtoms(BasicSizeTMap &atoms) { getArg()->getAtoms(atoms);} ;

        // Ersetzen
        BasicPtr subs( ConstBasicPtr const& old_exp,  BasicPtr const& new_exp);

        // args
        inline size_t getArgsSize() const { return 1; };
        inline BasicPtr const& getArg(size_t i) const { return getArg(); };

    protected:
        inline void setArg( BasicPtr const& arg) { m_arg.setArg(arg); };
        inline void simplifyArg() { setArg(getArg()->simplify()); };
    
    private:
        // Arguments
        Argument m_arg;
    };

    typedef boost::intrusive_ptr< UnaryOp> ConstUnaryOpPtr;
    typedef boost::intrusive_ptr<UnaryOp> UnaryOpPtr;

};

#endif // __UNARY_OP_H_
