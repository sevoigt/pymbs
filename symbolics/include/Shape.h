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

#ifndef __SHAPE_H_
#define __SHAPE_H_

#include <exception>
#include <string>
#include "Error.h"

namespace Symbolics
{
    /*****************************************************************************/
    class Shape
    {
    public:
        // Konstruktor
        // Skalar
        Shape();
        // Vektor
        Shape( size_t dim1);
        // Matrix
        Shape( size_t dim1,  size_t dim2);
        // Userdefined
        Shape( size_t ndim,  size_t dim1,  size_t dim2);
        // Destruktor
        ~Shape();

        // Dimension herausbekommen
        inline size_t getNrDimensions() const  { return m_ndim; };
        // Groesse einer Dimension herausbekommen
        inline size_t getDimension(size_t oneBasedIndex) const  // 1-basiert
        {
            switch (oneBasedIndex)
            {
            case 1:
                return m_dim1;
            case 2:
                return m_dim2;
            default:
                return 1;
            }
        };
        // Gesamtgroesse
        inline size_t getNumEl() const { return m_numElem; };

        // toString
        std::string toString() const;

        // Operatoren
        Shape& operator+= ( Shape const& rhs);
        Shape& operator*= ( Shape const& rhs);
        Shape operator+ ( Shape const& rhs) const;
        Shape operator* ( Shape const& rhs) const;
        bool operator== ( Shape const& rhs) const;
        bool operator!= ( Shape const& rhs) const;
        bool operator<(Shape const& rhs) const;
        // Transpose
        inline Shape transpose() const { return Shape( m_ndim, m_dim2, m_dim1 ); };

    protected:
        // Anzahl der Dimensionen
        size_t m_ndim;
        // Groesse der einzelnen Dimensionen
        size_t m_dim1;
        size_t m_dim2;
        // Anzahl der Elemente
        size_t m_numElem;
    };
    /*****************************************************************************/    
};

#endif // __SHAPE_H_