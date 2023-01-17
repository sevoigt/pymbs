#include "Shape.h"
#include <stdarg.h>
#include <stdlib.h>
#include "str.h"

#ifndef min
#define min(a,b) (a<b) ? a : b
#endif

using namespace Symbolics;

/*****************************************************************************/
Shape::Shape(): m_ndim(0), m_dim1(1), m_dim2(1)
/*****************************************************************************/
{
   m_numElem = m_dim1*m_dim2;
}
/*****************************************************************************/


/*****************************************************************************/
Shape::Shape( size_t dim1): m_ndim(1), m_dim1(dim1), m_dim2(1)
/*****************************************************************************/
{
    m_numElem = m_dim1*m_dim2;
    // Vektor
    if (dim1 == 0)
        throw ShapeError("dim1 must be greater than zero, when specifying a vector (ndim==1)!");
}
/*****************************************************************************/


/*****************************************************************************/
Shape::Shape( size_t dim1,  size_t dim2): m_ndim(2), m_dim1(dim1), m_dim2(dim2)
/*****************************************************************************/
{
    m_numElem = m_dim1*m_dim2;
    // Matrix
    if (dim1 == 0)
        throw ShapeError("dim1 must be greater than zero, when specifying a matrix (ndim==2)!");
    if (dim2 == 0)
        throw ShapeError("dim2 must be greater than zero, when specifying a matrix (ndim==2)!");
}
/*****************************************************************************/


/*****************************************************************************/
Shape::Shape( size_t ndim,  size_t dim1,  size_t dim2): m_ndim(ndim), m_dim1(dim1), m_dim2(dim2)
/*****************************************************************************/
{
    m_numElem = m_dim1*m_dim2;
    // ndim pruefen
    if ((ndim < 0) || (ndim > 2))
        throw ShapeError("ndim must be in the range of [0,2]!");
    if ((ndim == 0) && ((dim1 != 1) || (dim2 != 1))) 
        throw ShapeError("If a scalar is specified, at both dimensions must be 1!");
    if ((ndim == 1) && ((dim1 != 1) && (dim2 != 1))) 
        throw ShapeError("If a vector is specified, at least one dimension must be 1!");

    // Matrix
    if (dim1 == 0)
        throw ShapeError("dim1 must be greater than zero, when specifying a matrix (ndim==2)!");
    if (dim2 == 0) 
        throw ShapeError("dim2 must be greater than zero, when specifying a matrix (ndim==2)!");
}
/*****************************************************************************/


/*****************************************************************************/
Shape::~Shape()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
std::string Shape::toString() const
/*****************************************************************************/
{
#ifdef _DEBUG
    return "Shape(" + str(m_ndim) + "," + str(m_dim1) + "," + str(m_dim2) + ")";
    throw ShapeError("toString failed because Shape " + toString() + " is neither scalar, vector or matrix!");
#else
    // Skalar
    if (getNrDimensions() == 0) return "()";
    // Vektor
    if (getNrDimensions() == 1) return "(" + str(getDimension(1)) + ")";
    // Matrix
    if (getNrDimensions() == 2) return "(" + str(getDimension(1)) + "," + str(getDimension(2)) + ")";
    throw ShapeError("toString failed because Shape " + toString() + " is neither scalar, vector or matrix!");
#endif
}
/*****************************************************************************/


/*****************************************************************************/
Shape& Shape::operator+= ( Shape const& rhs)
/*****************************************************************************/
{
    // Beim Addieren sollten Shapes gleich sein
    // Nur bei einem Skalar ist es egal

    // Auf Skalar testen
    if (getNrDimensions() == 0)
    {
        m_ndim = rhs.m_ndim;
        m_dim1 = rhs.m_dim1;
        m_dim2 = rhs.m_dim2;
        m_numElem = m_dim1*m_dim2;
        return *this;
    }
    if (rhs.getNrDimensions() == 0)
        return *this;

    // Shapes vergleichen
    if (*this != rhs)
        throw ShapeError("Shapes " + toString() + "!=" + rhs.toString() + " do not match!");

    // Sonst ist alles ok
    return *this;
}
/*****************************************************************************/
/*****************************************************************************/
Shape Shape::operator+ ( Shape const& rhs) const
/*****************************************************************************/
{
    Shape tmp(*this);
    tmp += rhs;
    return tmp;
}
/*****************************************************************************/



/*****************************************************************************/
Shape& Shape::operator*= ( Shape const& rhs)
/*****************************************************************************/
{
    // Multiplizieren:
    // Skalar: egal
    // Ansonsten muessen dim2 und dim1 passen

    // Auf Skalar testen
    if (getNrDimensions() == 0)
    {
        m_ndim = rhs.m_ndim;
        m_dim1 = rhs.m_dim1;
        m_dim2 = rhs.m_dim2;
        m_numElem = m_dim1*m_dim2;
        return *this;
    }
    if (rhs.getNrDimensions() == 0)
        return *this;

    // Innere Dimension pruefen
    if ( getDimension(2) != rhs.getDimension(1) )
        throw ShapeError("Inner dimensions must be equal!");

    // Auf Vektor testen, Ergebnis ist Skalar
    if ((getNrDimensions() == 1) && (rhs.getNrDimensions() == 1))
    {
      // unterscheide inneres oder aeusseres Produkt
      if (m_dim1 == 1) // inneres Product
      {
        m_ndim = 0;
        m_dim1 = 1;
        m_dim2 = 1;
        m_numElem = m_dim1*m_dim2;
      }
      else // aeusseres Product
      {
        m_ndim = 2;
		    m_dim2 = rhs.getDimension(2);
        m_numElem = m_dim1*m_dim2;
      }
        return *this;
    }


    // Matrix * Matrix
    m_ndim = min(getNrDimensions(), rhs.getNrDimensions());
    m_dim2 = rhs.getDimension(2);
    m_numElem = m_dim1*m_dim2;    
    

    return *this;
}
/*****************************************************************************/
/*****************************************************************************/
Shape Shape::operator* ( Shape const& rhs) const
/*****************************************************************************/
{
    Shape tmp(*this);
    tmp *= rhs;
    return tmp;
}
/*****************************************************************************/


/*****************************************************************************/
bool Shape::operator== ( Shape const& rhs) const
/*****************************************************************************/
{
    // Dimension vergleichen
    if (getNrDimensions() != rhs.getNrDimensions()) return false;
    // 1. Dimension
	if (getDimension(1) != rhs.getDimension(1)) return false;
    // 2. Dimension
	if (getDimension(2) != rhs.getDimension(2)) return false;

    // Alles gleich
    return true;
}
/*****************************************************************************/


/*****************************************************************************/
bool Shape::operator!= ( Shape const& rhs) const
/*****************************************************************************/
{
    return !(operator==(rhs));
}
/*****************************************************************************/


/*****************************************************************************/
bool Shape::operator<(Shape const& rhs) const
/*****************************************************************************/
{
    // compare Dimension 
    if (m_ndim != rhs.m_ndim)
        return m_ndim < rhs.m_ndim;
    // compare each dim
    if (m_dim1 == rhs.m_dim1)
        return m_dim2 < rhs.m_dim2;
    return m_dim1 < rhs.m_dim1;
}
/*****************************************************************************/

