#ifndef __PYTHON_PRINTER_H_
#define __PYTHON_PRINTER_H_

#include "Printer.h"

namespace Symbolics
{
    class PythonPrinter: public Symbolics::Printer
    {
    public:
        // Konstruktor
        PythonPrinter();
        // Destruktor
        ~PythonPrinter();

        //comment Funktion �berschreiben
        std::string comment( std::string const& comment ) const;

    protected:
        //geforderte Funktionen �berschreiben
        std::string print_Element( const Element *e );
        std::string print_Matrix( const Matrix *mat );
        std::string print_Skew( const Skew *s ); 
        std::string print_Pow( const Pow *pow );
        std::string print_Zero( const Zero *z );
        std::string print_Bool( const Bool *b );

        // Funktionen die sich vom Standard unterscheiden, ueberschreiben
        std::string print_If ( const If *e );
        std::string print_Inverse( const Inverse *c );
        std::string print_Mul( const Mul *mul );
        std::string print_Scalar( const Scalar *s );
        std::string print_Solve( const Solve *s );
        std::string print_Transpose( const Transpose *t );
    };
};

#endif // __PYTHON_PRINTER_H_
