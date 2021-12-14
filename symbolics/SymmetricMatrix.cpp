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

#include "SymmetricMatrix.h"
#include "Util.h"
#include "str.h"
#include "Operators.h"
#include <sstream>

using namespace Symbolics;

/*****************************************************************************/
SymmetricMatrix::SymmetricMatrix(  Shape const& shape ): Matrix(shape,false)
/*****************************************************************************/
{
    if (shape.getDimension(1) != shape.getDimension(2))
        throw InternalError("SymmetricMatrix is only defined vor square matrixes!");
    // Shape anpassen und mit Nullen fuellen
    changeShape( shape, true );
}
/*****************************************************************************/

/*****************************************************************************/
SymmetricMatrix::SymmetricMatrix(  BasicPtrVec const& values,  Shape const& shape ): Matrix(shape,false)
/*****************************************************************************/
{
    if (shape.getDimension(1) != shape.getDimension(2))
        throw InternalError("SymmetricMatrix is only defined vor square matrixes!");
    // Shape speichern
    m_shape = shape;
	reserveArgs( getNumEl() );
    // Groesse pruefen
    if (getNumEl() != values.size())
        throw ShapeError("Size of values (" + str(values.size()) + " must match size of SymmetricMatrix " + m_shape.toString() + "!");
    // args uebernehmen
    for (size_t i=0;i<values.size();++i)
        addArg(values[i]);
}
/*****************************************************************************/


/*****************************************************************************/
SymmetricMatrix::SymmetricMatrix(  SymmetricMatrix const& src ): Matrix(src.getShape(),false)
/*****************************************************************************/
{
  reserveArgs( getNumEl() );
    for (size_t i=0;i<src.getArgsSize();++i)
        addArg(src.get(i));
}
/*****************************************************************************/


/*****************************************************************************/
SymmetricMatrix::~SymmetricMatrix()
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
 BasicPtr SymmetricMatrix::subs( ConstBasicPtr const& old_exp, 
                       BasicPtr const& new_exp)
/*****************************************************************************/
{
    if (*this == *old_exp.get())
        return new_exp;

    // subs args
    BasicPtrVec subs_args;
    subs_args.reserve( getArgsSize() );
    for (size_t i=0; i<getArgsSize(); ++i)
        subs_args.push_back(getArg(i)->subs(old_exp,new_exp));

    return BasicPtr( new SymmetricMatrix(subs_args, m_shape) );
}
/*****************************************************************************/

/*****************************************************************************/
 BasicPtr SymmetricMatrix::iterateExp(Symbolics::Basic::Iterator &v)
/*****************************************************************************/
{
    // iterate args
    BasicPtrVec iter_args;
    iter_args.reserve(getArgsSize());
    for (size_t i=0; i<getArgsSize(); ++i)
    {
        iter_args.push_back(getArg(i)->iterateExp(v));
    }
    return v.process_Arg(BasicPtr(new SymmetricMatrix(iter_args, m_shape)));
}
/*****************************************************************************/

/*****************************************************************************/
SymmetricMatrix SymmetricMatrix::operator+( SymmetricMatrix const& rhs) const
/*****************************************************************************/
{
    // Kopie anlegen
    SymmetricMatrix c( *this );
    // Addition
    c += rhs;
    // Rueckgabe
    return c;
}
/*****************************************************************************/

/*****************************************************************************/
SymmetricMatrix SymmetricMatrix::operator+( Int const& rhs) const
/*****************************************************************************/
{
    // Kopie anlegen
    SymmetricMatrix c( *this );
    // Addition
    // Wert addieren
    for (size_t i=0; i<getNumEl(); ++i)
        c.set(i,c(i) + rhs);
    // Rueckgabe
    return c;
}
/*****************************************************************************/

/*****************************************************************************/
SymmetricMatrix SymmetricMatrix::operator+( Real const& rhs) const
/*****************************************************************************/
{
    // Kopie anlegen
    SymmetricMatrix c( *this );
    // Addition
    // Wert addieren
    for (size_t i=0; i<getNumEl(); ++i)
        c.set(i,c(i) + rhs);
    // Rueckgabe
    return c;
}
/*****************************************************************************/

/*****************************************************************************/
SymmetricMatrix& SymmetricMatrix::operator+=( SymmetricMatrix const& rhs)
/*****************************************************************************/
{
    m_simplified = false;
    // Neuen Shape berechnen, koennte eine Exception werfen
     Shape newShape( m_shape+rhs.m_shape );

    // Sind wir ein Skalar?
    if (m_shape.getNrDimensions() == 0)
    {
        // Wert merken
        BasicPtr val = getArg(0);
        // Shape aendern und NICHT mit Nullen fuellen
        changeShape(newShape, false);

        // Werte kopieren (mit push_back, da SymmetricMatrix leer)
        for (size_t i=0; i<rhs.getNumEl(); ++i)
            addArg(val + rhs(i));

		// fertig
        return *this;
    }

    // Ist rhs ein Skalar?
    if (rhs.m_shape.getNrDimensions() == 0)
    {
        // Wert merken
        BasicPtr val = rhs(0);
        // Wert addieren
        for (size_t i=0; i<getNumEl(); ++i)
            setArg(i,getArg(i) + val);

		// fertig
        return *this;
    }

    // Weil beim Shape keine Exception kanm, stimmen beide Shapes ueberein
    for (size_t i=0; i<getNumEl(); ++i)
    {
        setArg(i,getArg(i) + rhs(i));
    }

    // Rueckgabe
    return *this;
}
/*****************************************************************************/


/*****************************************************************************/
SymmetricMatrix SymmetricMatrix::operator-( SymmetricMatrix const& rhs) const
/*****************************************************************************/
{
    // Kopie anlegen
    SymmetricMatrix c( *this );
    // Subtraktion
    c -= rhs;
    // Rueckgabe
    return c;
}
/*****************************************************************************/

/*****************************************************************************/
SymmetricMatrix SymmetricMatrix::operator-( Int const& rhs) const
/*****************************************************************************/
{
    // Kopie anlegen
    SymmetricMatrix c( *this );
    // Addition
    // Wert addieren
    for (size_t i=0; i<getNumEl(); ++i)
        c.set(i,c(i) - rhs);
    // Rueckgabe
    return c;
}
/*****************************************************************************/

/*****************************************************************************/
SymmetricMatrix SymmetricMatrix::operator-( Real const& rhs) const
/*****************************************************************************/
{
    // Kopie anlegen
    SymmetricMatrix c( *this );
    // Addition
    // Wert addieren
    for (size_t i=0; i<getNumEl(); ++i)
        c.set(i,c(i) - rhs);
    // Rueckgabe
    return c;
}
/*****************************************************************************/

/*****************************************************************************/
SymmetricMatrix& SymmetricMatrix::operator-=( SymmetricMatrix const& rhs)
/*****************************************************************************/
{
    m_simplified = false;
    // Neuen Shape berechnen, koennte eine Exception werfen
    Shape newShape( m_shape+rhs.m_shape );

    // Sind wir ein Skalar?
    if (m_shape.getNrDimensions() == 0)
    {
        // Wert merken
        BasicPtr val = getArg(0);
        // Shape aendern und NICHT mit Nullen fuellen
        changeShape(newShape, false);
        // Werte kopieren
        for (size_t i=0; i<rhs.getNumEl(); ++i)
            addArg(val - rhs(i));

		// fertig
        return *this;
    }

    // Ist rhs ein Skalar?
    if (rhs.m_shape.getNrDimensions() == 0)
    {
        // Wert merken
        BasicPtr val = rhs(0);
        // Wert addieren
        for (size_t i=0; i<getNumEl(); ++i)
        {
            setArg(i,getArg(i) - val);
        }
        // fertig
        return *this;
    }

    // Weil beim Shape keine Exception kanm, stimmen beide Shapes ueberein
    for (size_t i=0; i<getNumEl(); ++i)
    {
        setArg(i,getArg(i) - rhs(i));
    }

    // Rueckgabe
    return *this;
}
/*****************************************************************************/

/*****************************************************************************/
SymmetricMatrix SymmetricMatrix::operator*( Int const& rhs) const
/*****************************************************************************/
{
    // Kopie anlegen
    SymmetricMatrix c( *this );
    // Addition
    // Wert addieren
    for (size_t i=0; i<getNumEl(); ++i)
        c.set(i,c(i) * rhs);
    // Rueckgabe
    return c;
}
/*****************************************************************************/

/*****************************************************************************/
SymmetricMatrix SymmetricMatrix::operator*( Real const& rhs) const
/*****************************************************************************/
{
    // Kopie anlegen
    SymmetricMatrix c( *this );
    // Addition
     // Wert addieren
    for (size_t i=0; i<getNumEl(); ++i)
        c.set(i,c(i) * rhs);
    // Rueckgabe
    return c;
}
/*****************************************************************************/

/*****************************************************************************/
SymmetricMatrix SymmetricMatrix::operator/( Int const& rhs) const
/*****************************************************************************/
{
    // Kopie anlegen
    SymmetricMatrix c( *this );
    // Division
    c /= static_cast<double>(rhs.getValue());    
    // Rueckgabe
    return c;
}
/*****************************************************************************/

/*****************************************************************************/
SymmetricMatrix SymmetricMatrix::operator/( Real const& rhs) const
/*****************************************************************************/
{
    // Kopie anlegen
    SymmetricMatrix c( *this );
    // Division
    c /= rhs.getValue();    
    // Rueckgabe
    return c;
}
/*****************************************************************************/

/*****************************************************************************/
SymmetricMatrix SymmetricMatrix::operator/( double rhs) const
/*****************************************************************************/
{
    // Kopie anlegen
    SymmetricMatrix c( *this );
    // Division
    c /= rhs;    
    // Rueckgabe
    return c;
}
/*****************************************************************************/


/*****************************************************************************/
SymmetricMatrix& SymmetricMatrix::operator/=( double rhs)
/*****************************************************************************/
{
    // Alle Werte teilen
    for (size_t i=0; i<getNumEl(); ++i)
    {
        setArg(i,getArg(i) * (1/rhs));
    }

    return *this;
}
/*****************************************************************************/


/*****************************************************************************/
SymmetricMatrix::Inserter SymmetricMatrix::operator=( BasicPtr &a)
/*****************************************************************************/
{
    m_simplified = false;
    // Wert speichern und Inserter zurueckgeben
    return Inserter(*this, a);
}
/*****************************************************************************/


/*****************************************************************************/
SymmetricMatrix::Inserter SymmetricMatrix::operator=( int a)
/*****************************************************************************/
{
    // Wert speichern und Inserter zurueckgeben
    BasicPtr i(new Int(a));
    return Inserter(*this, i);
}
/*****************************************************************************/


/*****************************************************************************/
SymmetricMatrix::Inserter SymmetricMatrix::operator=( double a)
/*****************************************************************************/
{
    // Wert speichern und Inserter zurueckgeben
    BasicPtr r(new Real(a));
    return Inserter(*this, r);
}
/*****************************************************************************/


/*****************************************************************************/
SymmetricMatrix::Inserter::Inserter(SymmetricMatrix &mat,  BasicPtr &a): m_SymmetricMatrix(mat), m_counter(0)
/*****************************************************************************/
{
    // Wert speichern
    insert(a);
}
/*****************************************************************************/


/*****************************************************************************/
SymmetricMatrix::Inserter::~Inserter()
/*****************************************************************************/
{
    // Index pruefen
    // Der Destruktor wird beim Uebergeben aus dem = aufgerufen, in diesem Fall ist m_counter == 1
    if ((m_counter != 1) && (m_counter != m_SymmetricMatrix.getNumEl()))
        throw IndexError("Numbers of right hand side arguments does not match shape! Expected " 
                            + str(m_SymmetricMatrix.getNumEl()) + " but got " + str(m_counter) );
}
/*****************************************************************************/


/*****************************************************************************/
void SymmetricMatrix::Inserter::insert( BasicPtr &a)
/*****************************************************************************/
{
    // Wert speichern
    if (m_counter >= m_SymmetricMatrix.getNumEl())
        throw IndexError("Index out of bounds!");
    m_SymmetricMatrix.set(m_counter++,a);
}
/*****************************************************************************/


/*****************************************************************************/
SymmetricMatrix::Inserter& SymmetricMatrix::Inserter::operator,( BasicPtr &a)
/*****************************************************************************/
{
    // Wert speichern und uns selbst zurueckgeben
    insert(a);
    return *this;
}
/*****************************************************************************/


/*****************************************************************************/
SymmetricMatrix::Inserter& SymmetricMatrix::Inserter::operator,( int a)
/*****************************************************************************/
{
    // Wert speichern und uns selbst zurueckgeben
    BasicPtr i(new Int(a));
    insert(i);
    return *this;
}
/*****************************************************************************/


/*****************************************************************************/
SymmetricMatrix::Inserter& SymmetricMatrix::Inserter::operator,( double a)
/*****************************************************************************/
{
    // Wert speichern und uns selbst zurueckgeben
    BasicPtr r(new Real(a));
    insert(r);
    return *this;
}
/*****************************************************************************/

/*****************************************************************************/

SymmetricMatrix SymmetricMatrix::operator-() const
/*****************************************************************************/
{
    // Sonst halt alles drehen
    SymmetricMatrix tmp( m_shape );

    // Nun alles kopieren
    for(size_t i=0; i<getNumEl(); ++i)
        tmp.set(i,-getArg(i));

    // fertig
    return tmp;
}
/*****************************************************************************/


/*****************************************************************************/
 SymmetricMatrix& SymmetricMatrix::operator=(  SymmetricMatrix &mat )
/*****************************************************************************/
{
    m_simplified = false;
    // Shape aendern und NICHT mit Nullen fuellen
    changeShape( mat.getShape(), false );        

    // Werte kopieren
    for(size_t i=0; i<getNumEl(); ++i)
        addArg( mat(i) );

    // uns selbst zurueckgeben
    return *this;
}
/*****************************************************************************/


/*****************************************************************************/
bool Symbolics::operator== (SymmetricMatrix const& lhs, BasicPtr const& rhs)
/*****************************************************************************/
{
    if (rhs->getType() == Type_Matrix)
    {
        const SymmetricMatrix *crhs = Util::getAsConstPtr<SymmetricMatrix>(rhs);
        return lhs == (*crhs);
    }
    return false;
}
/*****************************************************************************/
/*****************************************************************************/
bool Symbolics::operator== (BasicPtr const& lhs, SymmetricMatrix const& rhs)
/*****************************************************************************/
{
    if (lhs->getType() == Type_Matrix)
    {
        const SymmetricMatrix *clhs = Util::getAsConstPtr<SymmetricMatrix>(lhs);
        return (*clhs) == rhs;
    }
    return false;
}
/*****************************************************************************/


/*****************************************************************************/
bool Symbolics::operator!= (SymmetricMatrix const& lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    if (rhs->getType() == Type_Matrix)
    {
        const SymmetricMatrix *crhs = Util::getAsConstPtr<SymmetricMatrix>(rhs);
        return lhs != (*crhs);
    }
    return false;
}
/*****************************************************************************/
/*****************************************************************************/
bool Symbolics::operator!= (BasicPtr const& lhs, SymmetricMatrix  const& rhs)
/*****************************************************************************/
{
    if (lhs->getType() == Type_Matrix)
    {
        const SymmetricMatrix *clhs = Util::getAsConstPtr<SymmetricMatrix>(lhs);
        return (*clhs) != rhs;
    }
    return false;
}
/*****************************************************************************/


/*****************************************************************************/
SymmetricMatrix Symbolics::operator+ (Int const& lhs, SymmetricMatrix  const& rhs)
/*****************************************************************************/
{
    // Kopie anlegen
    SymmetricMatrix c( rhs );
    // Addition
    // Wert addieren
    for (size_t i=0; i<rhs.getShape().getNumEl(); ++i)
        c.set(i,lhs + c(i));
    // Rueckgabe
    return c;
}
/*****************************************************************************/
/*****************************************************************************/
SymmetricMatrix Symbolics::operator+ (Real const& lhs, SymmetricMatrix  const& rhs)
/*****************************************************************************/
{
    // Kopie anlegen
    SymmetricMatrix c( rhs );
    // Addition
    // Wert addieren
    for (size_t i=0; i<rhs.getShape().getNumEl(); ++i)
        c.set(i,lhs + c(i));
    // Rueckgabe
    return c;
}
/*****************************************************************************/
/*****************************************************************************/
BasicPtr Symbolics::operator+ (SymmetricMatrix lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    BasicPtr l( new SymmetricMatrix(lhs) );
    return l + rhs;
}
/*****************************************************************************/
/*****************************************************************************/
BasicPtr Symbolics::operator+ (BasicPtr const& lhs, SymmetricMatrix rhs)
/*****************************************************************************/
{
    BasicPtr r( new SymmetricMatrix(rhs) );
    return lhs + r;
}
/*****************************************************************************/

/*****************************************************************************/
SymmetricMatrix Symbolics::operator- (Int const& lhs, SymmetricMatrix  const& rhs)
/*****************************************************************************/
{
    // Kopie anlegen
    SymmetricMatrix c( rhs );
    // Addition
    // Wert addieren
    for (size_t i=0; i<rhs.getShape().getNumEl(); ++i)
        c.set(i,lhs - c(i));
    // Rueckgabe
    return c;
}
/*****************************************************************************/
/*****************************************************************************/
SymmetricMatrix Symbolics::operator- (Real const& lhs, SymmetricMatrix  const& rhs)
/*****************************************************************************/
{
    // Kopie anlegen
    SymmetricMatrix c( rhs );
    // Addition
    // Wert addieren
    for (size_t i=0; i<rhs.getShape().getNumEl(); ++i)
        c.set(i,lhs - c(i));
    // Rueckgabe
    return c;
}
/*****************************************************************************/
/*****************************************************************************/
BasicPtr Symbolics::operator- (SymmetricMatrix lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    BasicPtr l( new SymmetricMatrix(lhs) );
    return l - rhs;
}
/*****************************************************************************/
/*****************************************************************************/
BasicPtr Symbolics::operator- (BasicPtr const& lhs, SymmetricMatrix rhs)
/*****************************************************************************/
{
    BasicPtr r( new SymmetricMatrix(rhs) );
    return lhs - r;
}
/*****************************************************************************/

/*****************************************************************************/
SymmetricMatrix Symbolics::operator* (Int const& lhs, SymmetricMatrix  const& rhs)
/*****************************************************************************/
{
    // Kopie anlegen
    SymmetricMatrix c( rhs );
    // Addition
    // Wert addieren
    for (size_t i=0; i<rhs.getShape().getNumEl(); ++i)
        c.set(i,lhs * c(i));
    // Rueckgabe
    return c;
}
/*****************************************************************************/
/*****************************************************************************/
SymmetricMatrix Symbolics::operator* (Real const& lhs, SymmetricMatrix  const& rhs)
/*****************************************************************************/
{
    // Kopie anlegen
    SymmetricMatrix c( rhs );
    // Addition
    // Wert addieren
    for (size_t i=0; i<rhs.getShape().getNumEl(); ++i)
        c.set(i,lhs * c(i));
    // Rueckgabe
    return c;
}
/*****************************************************************************/
/*****************************************************************************/
BasicPtr Symbolics::operator* (SymmetricMatrix lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    BasicPtr l( new SymmetricMatrix(lhs) );
    return l * rhs;
}
/*****************************************************************************/
/*****************************************************************************/
BasicPtr Symbolics::operator* (BasicPtr const& lhs, SymmetricMatrix rhs)
/*****************************************************************************/
{
    BasicPtr r( new SymmetricMatrix(rhs) );
    return lhs * r;
}
/*****************************************************************************/