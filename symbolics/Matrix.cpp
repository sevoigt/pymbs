#include "Matrix.h"
#include "Util.h"
#include "str.h"
#include "Operators.h"
#include <sstream>


#include <iostream>
#include <fstream>
#include "str.h"
using namespace Symbolics;


/*****************************************************************************/
Matrix::Matrix(  Shape const& shape, bool fillZeros ): NaryOp(Type_Matrix, BasicPtrVec())
/*****************************************************************************/
{
    // Shape 
    changeShape( shape, fillZeros );
}
/*****************************************************************************/

/*****************************************************************************/
Matrix::Matrix(  BasicPtrVec const& values, Shape const& shape ): NaryOp(Type_Matrix, values)
/*****************************************************************************/
{
    // Shape speichern
    m_shape = shape;
    // Groesse pruefen
    if (getNumEl() != values.size())
        throw ShapeError("Size of values (" + str(values.size()) + " must match size of Matrix " + m_shape.toString() + "!");
}
/*****************************************************************************/


/*****************************************************************************/
Matrix::Matrix( Matrix const& src ): NaryOp(Type_Matrix, BasicPtrVec())
/*****************************************************************************/
{
    // Shape speichern
    m_shape = src.getShape();
	reserveArgs( getNumEl() );
    for (size_t i=0;i<src.getShape().getDimension(1);++i)
        for (size_t j=0;j<src.getShape().getDimension(2);++j)
            addArg(src.get(i,j));
}
/*****************************************************************************/


/*****************************************************************************/
Matrix::~Matrix()
/*****************************************************************************/
{
}
/*****************************************************************************/


/****************************************************************************/
std::string Matrix::toString() const
/*****************************************************************************/
{
    // Wert in String wandeln
    std::stringstream ss;
    
    // Skalar
    if (m_shape.getNrDimensions() == 0)
        return getArg(0)->toString();

    // Vektor
    else if (m_shape.getNrDimensions() == 1)
    {
        size_t rows = m_shape.getDimension(1);
        ss << "vector([";
        for (size_t i=0; i<rows; ++i)
        {
            ss << getArg(i)->toString();
            if (i < (rows-1)) ss << ",";
        }
        ss << "])";
    }

    // Matrix
    else if (m_shape.getNrDimensions() == 2)
    {
         size_t rows = m_shape.getDimension(1);
         size_t cols = m_shape.getDimension(2);
        ss << "matrix([";
        for (size_t m=0; m<rows; ++m)
        {
            if (m>0) ss << "];[";
            for (size_t n=0; n<cols; ++n)
            {
                ss << (*this)(m,n)->toString();
                if (n < (cols-1)) ss << ",";
            }
        }
        ss << "])";
    }
    
    // Zurueckgeben des Strings
#ifdef _DEBUG
    return "Matrix(" + ss.str() + ")";
#else
    return ss.str();
#endif
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Matrix::simplify()
/*****************************************************************************/
{
    if (m_simplified)
        return BasicPtr(this);
    m_simplified = true;
    // Argument vereinfachen
    simplifyArgs();
    return BasicPtr(this);
}
/*****************************************************************************/


/*****************************************************************************/
 BasicPtr Matrix::iterateExp(Symbolics::Basic::Iterator &v)
/*****************************************************************************/
{
    // iterate args
    BasicPtrVec iter_args;
    iter_args.reserve(getArgsSize());
    for (size_t i=0; i<getArgsSize(); ++i)
    {
        iter_args.push_back(getArg(i)->iterateExp(v));
    }
    return v.process_Arg(BasicPtr(new Matrix(iter_args, m_shape)));
}
/*****************************************************************************/

/*****************************************************************************/
bool Matrix::operator==( Basic const& rhs) const
/*****************************************************************************/
{
  // Versuch zu casten
    if (rhs.getType() != Type_Matrix) return false;
    const Matrix &mat = dynamic_cast<const Matrix&>(rhs);

    // Speichergroesse vergleichen
    if (m_shape != mat.getShape()) return false;

    // Werte vergleichen
    size_t rows = m_shape.getDimension(1);
    size_t cols = m_shape.getDimension(2);
    for (size_t m=0; m<rows; ++m)
    {
        for (size_t n=0; n<cols; ++n)
        {
            if ( getArg(getIndex(m,n)) != mat(m,n) )
                return false;
        }
    }

    // Gleich
    return true;
}
/*****************************************************************************/


/*****************************************************************************/
bool Matrix::operator<(Basic const& rhs) const
/*****************************************************************************/
{
    // ConstBasicPtrpen vergleichen
    if (getType() != rhs.getType())
        return getType() < rhs.getType();

    // Vergleich Shape
    if (getShape() != rhs.getShape())
        return (getShape() < rhs.getShape());

    // Versuch zu casten
    const Matrix& mat = dynamic_cast<const Matrix&>(rhs);

    // Speichergroesse vergleichen
    if (m_shape != mat.getShape()) 
        return m_shape < mat.getShape();

    // Argumente vergleichen
    size_t rows = m_shape.getDimension(1);
    size_t cols = m_shape.getDimension(2);
    for (size_t m=0; m<rows; ++m)
    {
        for (size_t n=0; n<cols; ++n)
        {
            // Args vergleichen
            if (getArg(getIndex(m,n)) != mat(m,n))
                return (getArg(getIndex(m,n)) < mat(m,n));
        }
    }
    // all args equal return false
    return false;
}
/*****************************************************************************/
    

/*****************************************************************************/
Matrix Matrix::operator+( Matrix const& rhs) const
/*****************************************************************************/
{
    // Kopie anlegen
    Matrix c( *this );
    // Addition
    c += rhs;
    // Rueckgabe
    return c;
}
/*****************************************************************************/

/*****************************************************************************/
Matrix Matrix::operator+( Int const& rhs) const
/*****************************************************************************/
{
    // Kopie anlegen
    Matrix c( *this );
    // Addition
    // Wert addieren
    size_t rows = m_shape.getDimension(1);
    size_t cols = m_shape.getDimension(2);
    for (size_t m=0; m<rows; ++m)
        for (size_t n=0; n<cols; ++n)
            c.set(m,n,c(m,n) + rhs);
    // Rueckgabe
    return c;
}
/*****************************************************************************/

/*****************************************************************************/
Matrix Matrix::operator+( Real const& rhs) const
/*****************************************************************************/
{
    // Kopie anlegen
    Matrix c( *this );
    // Addition
    // Wert addieren
    size_t rows = m_shape.getDimension(1);
    size_t cols = m_shape.getDimension(2);
    for (size_t m=0; m<rows; ++m)
    {
        for (size_t n=0; n<cols; ++n)
        {
            c.set(m,n,c(m,n) + rhs);
        }
    }
    // Rueckgabe
    return c;
}
/*****************************************************************************/

/*****************************************************************************/
Matrix& Matrix::operator+=( Matrix const& rhs)
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

        // Werte kopieren (mit push_back, da Matrix leer)
        size_t rows = m_shape.getDimension(1);
        size_t cols = m_shape.getDimension(2);
        for (size_t m=0; m<rows; ++m)
            for (size_t n=0; n<cols; ++n)
                addArg(val + rhs(m,n));

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
            setArg(i,getArg(i) + val);
        }
        // fertig
        return *this;
    }

    // Weil beim Shape keine Exception kanm, stimmen beide Shapes ueberein
    size_t rows = m_shape.getDimension(1);
    size_t cols = m_shape.getDimension(2);
    for (size_t m=0; m<rows; ++m)
    {
        for (size_t n=0; n<cols; ++n)
        {
            setArg(getIndex(m,n),getArg(getIndex(m,n)) + rhs(m,n));
        }
    }

    // Rueckgabe
    return *this;
}
/*****************************************************************************/


/*****************************************************************************/
Matrix Matrix::operator-( Matrix const& rhs) const
/*****************************************************************************/
{
    // Kopie anlegen
    Matrix c( *this );
    // Subtraktion
    c -= rhs;
    // Rueckgabe
    return c;
}
/*****************************************************************************/

/*****************************************************************************/
Matrix Matrix::operator-( Int const& rhs) const
/*****************************************************************************/
{
    // Kopie anlegen
    Matrix c( *this );
    // Addition
    // Wert addieren
    size_t rows = m_shape.getDimension(1);
    size_t cols = m_shape.getDimension(2);
    for (size_t m=0; m<rows; ++m)
    {
        for (size_t n=0; n<cols; ++n)
        {
            c.set(m,n,c(m,n) - rhs);
        }
    }
    // Rueckgabe
    return c;
}
/*****************************************************************************/

/*****************************************************************************/
Matrix Matrix::operator-( Real const& rhs) const
/*****************************************************************************/
{
    // Kopie anlegen
    Matrix c( *this );
    // Addition
    // Wert addieren
    size_t rows = m_shape.getDimension(1);
    size_t cols = m_shape.getDimension(2);
    for (size_t m=0; m<rows; ++m)
    {
        for (size_t n=0; n<cols; ++n)
        {
            c.set(m,n,c(m,n) - rhs);
        }
    }
    // Rueckgabe
    return c;
}
/*****************************************************************************/

/*****************************************************************************/
Matrix& Matrix::operator-=( Matrix const& rhs)
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
        size_t rows = m_shape.getDimension(1);
        size_t cols = m_shape.getDimension(2);
        for (size_t m=0; m<rows; ++m)
            for (size_t n=0; n<cols; ++n)
                addArg(val - rhs(m,n));

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
    size_t rows = m_shape.getDimension(1);
    size_t cols = m_shape.getDimension(2);
    for (size_t m=0; m<rows; ++m)
    {
        for (size_t n=0; n<cols; ++n)
        {
            setArg(getIndex(m,n),getArg(getIndex(m,n)) - rhs(m,n));
        }
    }

    // Rueckgabe
    return *this;
}
/*****************************************************************************/


/*****************************************************************************/
Matrix Matrix::operator*( Matrix const& rhs) const
/*****************************************************************************/
{
     // Kopie anlegen
    Matrix c( *this );
    // Multiplikation
    c *= rhs;
    // Rueckgabe
    return c;
}
/*****************************************************************************/

/*****************************************************************************/
Matrix Matrix::operator*( Int const& rhs) const
/*****************************************************************************/
{
    // Kopie anlegen
    Matrix c( *this );
    // Addition
    // Wert addieren
    size_t rows = m_shape.getDimension(1);
    size_t cols = m_shape.getDimension(2);
    for (size_t m=0; m<rows; ++m)
    {
        for (size_t n=0; n<cols; ++n)
        {
            c.set(m,n,c(m,n) * rhs);
        }
    }
    // Rueckgabe
    return c;
}
/*****************************************************************************/

/*****************************************************************************/
Matrix Matrix::operator*( Real const& rhs) const
/*****************************************************************************/
{
    // Kopie anlegen
    Matrix c( *this );
    // Addition
     // Wert addieren
    size_t rows = m_shape.getDimension(1);
    size_t cols = m_shape.getDimension(2);
    for (size_t m=0; m<rows; ++m)
    {
        for (size_t n=0; n<cols; ++n)
        {
            c.set(m,n,c(m,n) * rhs);
        }
    }
    // Rueckgabe
    return c;
}
/*****************************************************************************/

/*****************************************************************************/
Matrix& Matrix::operator*=( Matrix const& rhs)
/*****************************************************************************/
{
    m_simplified = false;
    // Neuen Shape berechnen, koennte eine Exception werfen
    Shape newShape( m_shape*rhs.m_shape );

    // Sind wir ein Skalar?
    if (m_shape.getNrDimensions() == 0)
    {
        // Wert merken
        BasicPtr val = getArg(0);
        // Shape aendern
        changeShape( newShape, false );
        // Werte kopieren
        size_t rows = m_shape.getDimension(1);
        size_t cols = m_shape.getDimension(2);
        for (size_t m=0; m<rows; ++m)
            for (size_t n=0; n<cols; ++n)
                addArg(val * rhs(m,n));

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
            setArg(i,getArg(i) * val);
        }
        // fertig
        return *this;
    }

    // Sind wir ein Vektor?
    if (m_shape.getNrDimensions() == 1)
    {
        // Ist der andere ein Vektor?
        if (rhs.m_shape.getNrDimensions() == 1)
        {
          if (m_shape.getDimension(1) == 1)  // Skalarprodukt berechnen
          {
            BasicPtr val = Zero::getZero();
            for (size_t i=0; i<getNumEl(); ++i)
                val = val + getArg(i)*rhs(i);

            // Shape anpassen
            changeShape( newShape, false );
            addArg(val);
            return *this;
          }
        }
    }
    
    // Vektor*Matrix, Matrix*Vektor oder Matrix*Matrix Multiplikation
    BasicPtrVec newArgs;
    newArgs.reserve( newShape.getNumEl() );
    size_t dim1 = m_shape.getDimension(1);
    size_t dimInner = m_shape.getDimension(2);    // == rhs.m_shape.getDimension(1)
    size_t dim2 = rhs.m_shape.getDimension(2);
    // Werte berechnen
    for (size_t m=0;m<dim1;++m)
    {
        for(size_t n=0;n<dim2;++n)
        {
            BasicPtr val = Zero::getZero();
            for(size_t i=0; i<dimInner; ++i)
                val = val + get(m,i)*rhs(i,n);
            newArgs.push_back(val);
        }
    }
    clearArgs();
    // Werte uebernehmen (etwas Hardcore)
    m_shape = newShape;
	  reserveArgs( getNumEl() );
    for (size_t i=0;i<newArgs.size();++i)
		addArg(newArgs[i]);

    // Rueckgabe
    return *this;
}
/*****************************************************************************/

/*****************************************************************************/
Matrix Matrix::operator/( Int const& rhs) const
/*****************************************************************************/
{
    // Kopie anlegen
    Matrix c( *this );
    // Division
    c /= static_cast<double>(rhs.getValue());    
    // Rueckgabe
    return c;
}
/*****************************************************************************/

/*****************************************************************************/
Matrix Matrix::operator/( Real const& rhs) const
/*****************************************************************************/
{
    // Kopie anlegen
    Matrix c( *this );
    // Division
    c /= rhs.getValue();    
    // Rueckgabe
    return c;
}
/*****************************************************************************/

/*****************************************************************************/
Matrix Matrix::operator/( double rhs) const
/*****************************************************************************/
{
    // Kopie anlegen
    Matrix c( *this );
    // Division
    c /= rhs;    
    // Rueckgabe
    return c;
}
/*****************************************************************************/


/*****************************************************************************/
Matrix& Matrix::operator/=( double rhs)
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
Matrix::Inserter Matrix::operator=( BasicPtr const& a)
/*****************************************************************************/
{
    m_simplified = false;
    // Wert speichern und Inserter zurueckgeben
    return Inserter(*this, a);
}
/*****************************************************************************/


/*****************************************************************************/
Matrix::Inserter Matrix::operator=( int a)
/*****************************************************************************/
{
    // Wert speichern und Inserter zurueckgeben
    BasicPtr i(new Int(a));
    return Inserter(*this, i);
}
/*****************************************************************************/


/*****************************************************************************/
Matrix::Inserter Matrix::operator=( double a)
/*****************************************************************************/
{
    // Wert speichern und Inserter zurueckgeben
    BasicPtr r(new Real(a));
    return Inserter(*this, r);
}
/*****************************************************************************/


/*****************************************************************************/
Matrix::Inserter::Inserter(Matrix & matrix,  BasicPtr const& a): m_matrix(matrix), m_counter(0)
/*****************************************************************************/
{
    // Wert speichern
    insert(a);
}
/*****************************************************************************/


/*****************************************************************************/
Matrix::Inserter::~Inserter()
/*****************************************************************************/
{
    // Index pruefen
    // Der Destruktor wird beim Uebergeben aus dem = aufgerufen, in diesem Fall ist m_counter == 1
    if ((m_counter != 1) && (m_counter != m_matrix.getNumEl()))
        throw IndexError("Numbers of right hand side arguments does not match shape! Expected " 
                            + str(m_matrix.getNumEl()) + " but got " + str(m_counter) );
}
/*****************************************************************************/


/*****************************************************************************/
void Matrix::Inserter::insert( BasicPtr const& a)
/*****************************************************************************/
{
    // Wert speichern
    if (m_counter >= m_matrix.getNumEl())
        throw IndexError("Index out of bounds!");
    m_matrix.set(m_counter++,a);
}
/*****************************************************************************/


/*****************************************************************************/
Matrix::Inserter& Matrix::Inserter::operator,( BasicPtr const& a)
/*****************************************************************************/
{
    // Wert speichern und uns selbst zurueckgeben
    insert(a);
    return *this;
}
/*****************************************************************************/


/*****************************************************************************/
Matrix::Inserter& Matrix::Inserter::operator,( int a)
/*****************************************************************************/
{
    // Wert speichern und uns selbst zurueckgeben
    BasicPtr i(new Int(a));
    insert(i);
    return *this;
}
/*****************************************************************************/


/*****************************************************************************/
Matrix::Inserter& Matrix::Inserter::operator,( double a)
/*****************************************************************************/
{
    // Wert speichern und uns selbst zurueckgeben
    BasicPtr r(new Real(a));
    insert(r);
    return *this;
}
/*****************************************************************************/


/*****************************************************************************/
BasicPtr const& Matrix::get( size_t zeroBasedRow,  size_t zeroBasedCol) const
/*****************************************************************************/
{
     size_t i = getIndex(zeroBasedRow,zeroBasedCol);
    if (i >= getNumEl())
        throw IndexError("Index out of bounds!");
    return getArg(i);
}
/*****************************************************************************/


/*****************************************************************************/
BasicPtr const& Matrix::get( size_t zeroBasedRow) const
/*****************************************************************************/
{
    if (zeroBasedRow >= getNumEl())
        throw IndexError("Index out of bounds!");
    return getArg(zeroBasedRow);
}
/*****************************************************************************/

/*****************************************************************************/
void Matrix::set( size_t zeroBasedRow,  size_t zeroBasedCol, BasicPtr const& arg)
/*****************************************************************************/
{
    size_t i = getIndex(zeroBasedRow,zeroBasedCol);
    if (i >= getNumEl())
        throw IndexError("Index out of bounds!");
    m_simplified = false;
    setArg(i,arg);
}
/*****************************************************************************/


/*****************************************************************************/
void Matrix::set( size_t zeroBasedRow, BasicPtr const& arg) 
/*****************************************************************************/
{
    if (zeroBasedRow >= getNumEl())
        throw IndexError("Index out of bounds!");
    m_simplified = false;
    setArg(zeroBasedRow,arg);
}
/*****************************************************************************/


/*****************************************************************************/
Matrix Matrix::transpose() const
/*****************************************************************************/
{
    // Bei einem Skalar ist es egal
    if (m_shape.getNrDimensions() == 0)
    {
        return *this;
    }

    // Sonst halt alles drehen
    Matrix tmp( m_shape.transpose() );

    // Nun alles kopieren
    for( size_t m=0; m<m_shape.getDimension(1); m++)
        for( size_t n=0; n<m_shape.getDimension(2); n++)
            tmp.set(n,m,get(m,n));

    // fertig
    return tmp;
}
/*****************************************************************************/


/*****************************************************************************/
Matrix Matrix::operator-() const
/*****************************************************************************/
{
    // Sonst halt alles drehen
    Matrix tmp( m_shape );

    // Nun alles kopieren
    for(size_t i=0; i<getNumEl(); ++i)
        tmp.set(i,-get(i));

    // fertig
    return tmp;
}
/*****************************************************************************/


/*****************************************************************************/
 Matrix& Matrix::operator=(  Matrix const& mat )
/*****************************************************************************/
{
    m_simplified = false;
    // Shape aendern und NICHT mit Nullen fuellen
    changeShape( mat.getShape(), false );        

    // Werte kopieren
    size_t rows = m_shape.getDimension(1);
    size_t cols = m_shape.getDimension(2);
    for (size_t m=0; m<rows; ++m)
        for (size_t n=0; n<cols; ++n)
            addArg( mat(m,n) );

	// uns selbst zurueckgeben
    return *this;
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Matrix::der()
/*****************************************************************************/
{
    // lehre matrix
    Matrix *tmp = new Matrix( m_shape );

    // Nun alles differenzieren
    for(size_t i=0; i<getNumEl(); ++i)
        tmp->set(i,get(i)->der());

    // fertig
    return BasicPtr(tmp);
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Matrix::der(BasicPtr const& symbol)
/*****************************************************************************/
{
    // lehre matrix
    Matrix *tmp = new Matrix( m_shape );

    // Nun alles differenzieren
    for(size_t i=0; i<getNumEl(); ++i)
        tmp->set(i,get(i)->der(symbol));

    // fertig
    return BasicPtr(tmp);
}
/*****************************************************************************/

/*****************************************************************************/
bool Symbolics::operator== (Matrix const& lhs, BasicPtr const& rhs)
/*****************************************************************************/
{
    if (rhs->getType() == Type_Matrix)
    {
        const Matrix *crhs = Util::getAsConstPtr<Matrix>(rhs);
        return lhs == (*crhs);
    }
    return false;
}
/*****************************************************************************/
/*****************************************************************************/
bool Symbolics::operator== (BasicPtr const& lhs, Matrix const& rhs)
/*****************************************************************************/
{
    if (lhs->getType() == Type_Matrix)
    {
        const Matrix *clhs = Util::getAsConstPtr<Matrix>(lhs);
        return (*clhs) == rhs;
    }
    return false;
}
/*****************************************************************************/


/*****************************************************************************/
bool Symbolics::operator!= (Matrix const& lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    if (rhs->getType() == Type_Matrix)
    {
        const Matrix *crhs = Util::getAsConstPtr<Matrix>(rhs);
        return lhs != (*crhs);
    }
    return false;
}
/*****************************************************************************/
/*****************************************************************************/
bool Symbolics::operator!= (BasicPtr const& lhs, Matrix  const& rhs)
/*****************************************************************************/
{
    if (lhs->getType() == Type_Matrix)
    {
        const Matrix *clhs = Util::getAsConstPtr<Matrix>(lhs);
        return (*clhs) != rhs;
    }
    return false;
}
/*****************************************************************************/


/*****************************************************************************/
Matrix Symbolics::operator+ (Int const& lhs, Matrix  const& rhs)
/*****************************************************************************/
{
    // Kopie anlegen
    Matrix c( rhs );
    // Addition
    // Wert addieren
    for (size_t i=0; i<rhs.getNumEl(); ++i)
        c.set(i,lhs + c(i));
    // Rueckgabe
    return c;
}
/*****************************************************************************/
/*****************************************************************************/
Matrix Symbolics::operator+ (Real const& lhs, Matrix  const& rhs)
/*****************************************************************************/
{
    // Kopie anlegen
    Matrix c( rhs );
    // Addition
    // Wert addieren
    for (size_t i=0; i<rhs.getNumEl(); ++i)
        c.set(i,lhs + c(i));
    // Rueckgabe
    return c;
}
/*****************************************************************************/
/*****************************************************************************/
BasicPtr Symbolics::operator+ (Matrix lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    BasicPtr l( new Matrix(lhs) );
    return l + rhs;
}
/*****************************************************************************/
/*****************************************************************************/
BasicPtr Symbolics::operator+ (BasicPtr const& lhs, Matrix rhs)
/*****************************************************************************/
{
    BasicPtr r( new Matrix(rhs) );
    return lhs + r;
}
/*****************************************************************************/

/*****************************************************************************/
Matrix Symbolics::operator- (Int const& lhs, Matrix  const& rhs)
/*****************************************************************************/
{
    // Kopie anlegen
    Matrix c( rhs );
    // Addition
    // Wert addieren
    for (size_t i=0; i<rhs.getNumEl(); ++i)
        c.set(i,lhs - c(i));
    // Rueckgabe
    return c;
}
/*****************************************************************************/
/*****************************************************************************/
Matrix Symbolics::operator- (Real const& lhs, Matrix  const& rhs)
/*****************************************************************************/
{
    // Kopie anlegen
    Matrix c( rhs );
    // Addition
    // Wert addieren
    for (size_t i=0; i<rhs.getNumEl(); ++i)
        c.set(i,lhs - c(i));
    // Rueckgabe
    return c;
}
/*****************************************************************************/
/*****************************************************************************/
BasicPtr Symbolics::operator- (Matrix lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    BasicPtr l( new Matrix(lhs) );
    return l - rhs;
}
/*****************************************************************************/
/*****************************************************************************/
BasicPtr Symbolics::operator- (BasicPtr const& lhs, Matrix rhs)
/*****************************************************************************/
{
    BasicPtr r( new Matrix(rhs) );
    return lhs - r;
}
/*****************************************************************************/

/*****************************************************************************/
Matrix Symbolics::operator* (Int const& lhs, Matrix  const& rhs)
/*****************************************************************************/
{
    // Kopie anlegen
    Matrix c( rhs );
    // Addition
    // Wert addieren
    for (size_t i=0; i<rhs.getNumEl(); ++i)
        c.set(i,lhs * c(i));
    // Rueckgabe
    return c;
}
/*****************************************************************************/
/*****************************************************************************/
Matrix Symbolics::operator* (Real const& lhs, Matrix  const& rhs)
/*****************************************************************************/
{
    // Kopie anlegen
    Matrix c( rhs );
    // Multiplication
    // Wert multiplizieren
    for (size_t i=0; i<rhs.getNumEl(); ++i)
        c.set(i,lhs * c(i));
    // Rueckgabe
    return c;
}
/*****************************************************************************/
/*****************************************************************************/
BasicPtr Symbolics::operator* (Matrix lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    BasicPtr l( new Matrix(lhs) );
    return l * rhs;
}
/*****************************************************************************/
/*****************************************************************************/
BasicPtr Symbolics::operator* (BasicPtr const& lhs, Matrix rhs)
/*****************************************************************************/
{
    BasicPtr r( new Matrix(rhs) );
    return lhs * r;
}
/*****************************************************************************/