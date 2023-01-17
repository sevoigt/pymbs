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

        // Vereinfachen, wenn unver�ndert, dann NULL
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