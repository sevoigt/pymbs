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