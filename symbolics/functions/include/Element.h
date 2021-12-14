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

#ifndef __ELEMENT_H_
#define __ELEMENT_H_

#include "NaryOp.h"

namespace Symbolics
{

    class Element: public NaryOp
    {
    public:
        // Konstruktor
        Element( BasicPtrVec const& args );
        // Konstruktor
        Element( BasicPtr const& arg, size_t zeroBasedRow, size_t zeroBasedCol );
        // Destruktor
        ~Element();

        // toString
        std::string toString() const;

        // Vereinfachen, wenn unverändert, dann NULL
        BasicPtr simplify();

        // Zeile
        inline size_t getRow() const { return m_row; };
        // Spalte
        inline size_t getCol() const { return m_col; };

        static BasicPtr New( BasicPtr const& arg, size_t zeroBasedRow, size_t zeroBasedCol );

        // derivative
        inline BasicPtr der() { return New(getArg(0)->der(),getRow(),getCol()); };
        inline BasicPtr der(BasicPtr const& symbol) { return New(getArg(0)->der(symbol),getRow(),getCol()); };

        class Scalarizer: public Basic::Iterator
        {
        public:
            Scalarizer() {;}
            ~Scalarizer() {;}
          
            BasicPtrMap scalars;
            BasicPtr process_Arg(BasicPtr const &p);
        };

    protected:
        // Zeile und Spalte, aus der ausgewaehlt werden soll
        size_t m_row;
        size_t m_col;

		void validate();
    };

    typedef boost::intrusive_ptr< Element> ConstElementPtr;
    typedef boost::intrusive_ptr<Element> ElementPtr;

};

#endif // __ELEMENT_H_