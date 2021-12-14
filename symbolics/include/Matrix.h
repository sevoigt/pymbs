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

#ifndef __MATRIX_H_
#define __MATRIX_H_

#include "Basic.h"
#include "Error.h"
#include "NaryOp.h"
#include "Int.h"
#include <map>

namespace Symbolics
{

/*****************************************************************************/
    class Matrix: public NaryOp
    {
    public:
        // Konstruktor (mit Nullen füllen)
        Matrix(  Shape const& shape, bool fillZeros = true );
        // Konstruktor (mit Array)
        Matrix(  BasicPtrVec const& values,  Shape const& shape );
        // Kopierkonstruktor
        Matrix(  Matrix const& src );

        // Destruktor
        virtual ~Matrix();

        // toString
        std::string toString() const;

        // Vereinfachen, wenn unverändert, dann NULL
        BasicPtr simplify();

        BasicPtr iterateExp(Symbolics::Basic::Iterator &v);

        // Vergleich
        bool operator==( Basic const& rhs) const;
        bool operator<(Basic  const& rhs) const;

         // Operatoren
        Matrix operator+( Matrix const& rhs) const;
        Matrix operator+( Int const& rhs) const;
        Matrix operator+( Real const& rhs) const;
        Matrix& operator+=( Matrix const& rhs);
        Matrix operator-( Matrix const& rhs) const;
        Matrix operator-( Int const& rhs) const;
        Matrix operator-( Real const& rhs) const;
        Matrix& operator-=( Matrix const& rhs);
        Matrix operator*( Matrix const& rhs) const;
        Matrix operator*( Int const& rhs) const;
        Matrix operator*( Real const& rhs) const;
        Matrix& operator*=( Matrix const& rhs);
        Matrix operator/( Int const& rhs) const;
        Matrix operator/( Real const& rhs) const;
        Matrix operator/( double rhs) const;
        Matrix& operator/=( double rhs);
        Matrix operator-() const;

        // Wert abfragen (0 basiert)
        BasicPtr const& get( size_t zeroBasedRow,  size_t zeroBasedCol) const;
        BasicPtr const& get( size_t zeroBasedRow) const;
		    inline BasicPtr const& operator()( size_t zeroBasedRow,  size_t zeroBasedCol ) const { return get(zeroBasedRow, zeroBasedCol); };
		    inline BasicPtr const& operator()( size_t zeroBasedRow ) const { return get(zeroBasedRow); };

        // wert Setzen
        void set( size_t zeroBasedRow,  size_t zeroBasedCol, BasicPtr const& arg);
        void set( size_t zeroBasedRow, BasicPtr const& arg);

        // Transponieren
        virtual Matrix transpose() const;

        // Funktor
        template< typename F >
        struct UnaryFunctor
        {
            inline BasicPtr operator()( BasicPtr arg )
            {
                BasicPtr unaryarg( new F(arg) );
                return unaryarg->simplify();
            };
        };
        template< typename F, typename Arg >
        struct BinaryFunctor
        {
        public:
            inline BinaryFunctor( Arg arg ): m_arg(arg) {};
            inline BasicPtr operator()( BasicPtr base )
            {
                BasicPtr binaryarg( new F(base, m_arg) );
                return binaryarg->simplify();
            }
        protected:
            Arg m_arg;
        };
        template< typename F, typename Arg1, typename Arg2 >
        struct TernaryFunctor
        {
        public:
            inline TernaryFunctor( Arg1 arg1, Arg2 arg2 ): m_arg1(arg1), m_arg2(arg2) {};
            inline BasicPtr operator()( BasicPtr base )
            {
                BasicPtr ternaryarg( new F(base, m_arg1, m_arg2) );
                return ternaryarg->simplify();
            }
        protected:
            Arg1 m_arg1;
            Arg2 m_arg2;
        };

        template< typename F >
        BasicPtr applyFunctor( F &f ) const
        {
            // neue Matrix anlegen
            Matrix* mat = new Matrix(getShape());

            // mit Elementen fuellen
            for(size_t i=0; i<getNumEl(); ++i)
                mat->set(i, f( getArg(i) ));

            mat->m_simplified=true;
            // fertig
            return BasicPtr(mat);
        };

        // Inserter, so dass mit Matrix = 1,2,3 eingefügt werden kann
        // Matrix = 1 gibt einen Inserter zurück, dessen , Operator überladen ist ...
        struct Inserter
        {
        public:
            // Konstruktor
            Inserter( Matrix & matrix,  BasicPtr const& a );
            // Destruktor
            ~Inserter();
            // , Operator überladen
            Inserter& operator,( BasicPtr const& a);
            Inserter& operator,( int a);
            Inserter& operator,( double a);
        protected:
            void insert( BasicPtr const& a);
            // Referenz auf Matrix
            Matrix &m_matrix;
            // Zaehler, wo wir gerade sind
            size_t m_counter;
        };
        Inserter operator=(BasicPtr const &a);
        Inserter operator=(int a);
        Inserter operator=(double a);
        Matrix& operator=(Matrix const& mat);
    
        virtual inline size_t getNumEl() const { return m_shape.getNumEl(); };

        // derivative
        BasicPtr der();
        BasicPtr der(BasicPtr const& symbol);
        
    protected:
        virtual inline size_t getIndex(size_t row, size_t col) const { return row*m_shape.getDimension(2) + col; };
        
        inline void changeShape( Shape const& newShape, bool fillZeros=true )
        {
            // Argumente loeschen
            clearArgs();
            if (m_shape != newShape)
            {
                // neuen Shape uebernehmen
                m_shape = newShape;
                // Groesse der Args definieren
                reserveArgs( getNumEl());
            }
            // Mit Nullen fuellen
            if (fillZeros)
                for(size_t i=0; i<getNumEl(); ++i)
                    addArg( Zero::getZero() );
        };

    };
/*****************************************************************************/

    // Operatoren mit Matrix
    Matrix operator+ (Int const& lhs, Matrix  const& rhs);
    Matrix operator- (Int const& lhs, Matrix  const& rhs);
    Matrix operator* (Int const& lhs, Matrix  const& rhs);

       Matrix operator+ (Real const& lhs, Matrix  const& rhs);
    Matrix operator- (Real const& lhs, Matrix  const& rhs);
    Matrix operator* (Real const& lhs, Matrix  const& rhs);

    BasicPtr operator+ (Matrix lhs, BasicPtr  const& rhs);
    BasicPtr operator+ (BasicPtr const& lhs, Matrix rhs);
    BasicPtr operator- (Matrix lhs, BasicPtr  const& rhs);
    BasicPtr operator- (BasicPtr const& lhs, Matrix rhs);
    BasicPtr operator* (Matrix lhs, BasicPtr  const& rhs);
    BasicPtr operator* (BasicPtr const& lhs, Matrix rhs);
    bool operator== (Matrix const& lhs, BasicPtr  const& rhs);
    bool operator!= (Matrix const& lhs, BasicPtr  const& rhs);
    bool operator== (BasicPtr const& lhs, Matrix  const& rhs);
    bool operator!= (BasicPtr const& lhs, Matrix  const& rhs);

};

#endif // __MATRIX_H_
