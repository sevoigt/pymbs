#ifndef __SymmetricMatrix_H_
#define __SymmetricMatrix_H_

#include "Matrix.h"

namespace Symbolics
{

/*****************************************************************************/
    class SymmetricMatrix: public Matrix
    {
    public:
        // Konstruktor (mit Nullen f�llen)
        SymmetricMatrix( Shape const& shape );
        // Konstruktor (mit Array)
        SymmetricMatrix( BasicPtrVec const& values,  Shape const& shape );
        // Kopierkonstruktor
        SymmetricMatrix( SymmetricMatrix const& src );

        // Destruktor
        virtual ~SymmetricMatrix();

        BasicPtr subs( ConstBasicPtr const& old_exp, BasicPtr const& new_exp);

        BasicPtr iterateExp(Symbolics::Basic::Iterator &v);

         // Operatoren
        SymmetricMatrix operator+( SymmetricMatrix const& rhs) const;
        SymmetricMatrix operator+( Int const& rhs) const;
        SymmetricMatrix operator+( Real const& rhs) const;
        SymmetricMatrix& operator+=( SymmetricMatrix const& rhs);
        SymmetricMatrix operator-( SymmetricMatrix const& rhs) const;
        SymmetricMatrix operator-( Int const& rhs) const;
        SymmetricMatrix operator-( Real const& rhs) const;
        SymmetricMatrix& operator-=( SymmetricMatrix const& rhs);
        using Matrix::operator*;
        SymmetricMatrix operator*( Int const& rhs) const;
        SymmetricMatrix operator*( Real const& rhs) const;
        SymmetricMatrix operator/( Int const& rhs) const;
        SymmetricMatrix operator/( Real const& rhs) const;
        SymmetricMatrix operator/( double rhs) const;
        SymmetricMatrix& operator/=( double rhs);
        SymmetricMatrix operator-() const;

        // Transponieren
        Matrix transpose() const { return *this; };

        // Inserter, so dass mit SymmetricMatrix = 1,2,3 eingef�gt werden kann
        // SymmetricMatrix = 1 gibt einen Inserter zur�ck, dessen , Operator �berladen ist ...
        struct Inserter
        {
        public:
            // Konstruktor
            Inserter( SymmetricMatrix &constant,  BasicPtr &a );
            // Destruktor
            ~Inserter();
            // , Operator �berladen
            Inserter& operator,( BasicPtr &a);
            Inserter& operator,( int a);
            Inserter& operator,( double a);
        protected:
            void insert( BasicPtr &a);
            // Referenz auf SymmetricMatrix
            SymmetricMatrix &m_SymmetricMatrix;
            // Zaehler, wo wir gerade sind
            size_t m_counter;
        };
        Inserter operator=( BasicPtr &a);
        Inserter operator=( int a);
        Inserter operator=( double a);
        SymmetricMatrix& operator=( SymmetricMatrix &mat);
    
        inline size_t getNumEl() const { return ((m_shape.getDimension(1)+1)*m_shape.getDimension(1))/2; };

    protected:
        inline size_t getIndex(size_t row, size_t col) const 
        { 
          size_t n = m_shape.getDimension(1);
          if ((row >= n) || (col >= n)) return n*n;
          if (row > col)
          {
              size_t tmp = row;
              row = col;
              col = tmp;
          }          
          return n*row-(row*(row-1))/2+col-row; 
        };

    };
/*****************************************************************************/

    // Operatoren mit SymmetricMatrix
    SymmetricMatrix operator+ (Int const& lhs, SymmetricMatrix  const& rhs);
    SymmetricMatrix operator- (Int const& lhs, SymmetricMatrix  const& rhs);
    SymmetricMatrix operator* (Int const& lhs, SymmetricMatrix  const& rhs);

    SymmetricMatrix operator+ (Real const& lhs, SymmetricMatrix  const& rhs);
    SymmetricMatrix operator- (Real const& lhs, SymmetricMatrix  const& rhs);
    SymmetricMatrix operator* (Real const& lhs, SymmetricMatrix  const& rhs);

    BasicPtr operator+ (SymmetricMatrix lhs, BasicPtr  const& rhs);
    BasicPtr operator+ (BasicPtr const& lhs, SymmetricMatrix rhs);
    BasicPtr operator- (SymmetricMatrix lhs, BasicPtr  const& rhs);
    BasicPtr operator- (BasicPtr const& lhs, SymmetricMatrix rhs);
    BasicPtr operator* (SymmetricMatrix lhs, BasicPtr  const& rhs);
    BasicPtr operator* (BasicPtr const& lhs, SymmetricMatrix rhs);
    bool operator== (SymmetricMatrix const& lhs, BasicPtr  const& rhs);
    bool operator!= (SymmetricMatrix const& lhs, BasicPtr  const& rhs);
    bool operator== (BasicPtr const& lhs, SymmetricMatrix  const& rhs);
    bool operator!= (BasicPtr const& lhs, SymmetricMatrix  const& rhs);

};

#endif // __SymmetricMatrix_H_
