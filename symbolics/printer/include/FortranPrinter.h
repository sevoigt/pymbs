#ifndef __FORTRAN_PRINTER_H_
#define __FORTRAN_PRINTER_H_

#include "Printer.h"

namespace Symbolics
{
    class FortranPrinter: public Symbolics::Printer
    {
    public:
        // Konstruktor
        FortranPrinter();
        // Destruktor
        ~FortranPrinter();

        //comment Funktion �berschreiben
        std::string comment( std::string const& comment ) const;

        std::string dimension( BasicPtr const& basic );

    protected:
        //geforderte Funktionen �berschreiben
        std::string print_Element( const Element *e );
        std::string print_Matrix( const Matrix *mat );
        std::string print_Skew( const Skew *s ); 
        std::string print_Pow( const Pow *pow );
        std::string print_Zero( const Zero *z );
        std::string print_Bool( const Bool *b );

		// Funktionen die sich vom Standard unterscheiden, ueberschreiben
        std::string print_Mul( const Mul *mul );
        //std::string print_If( const If *e );
        std::string print_Int( const Int *c );
		std::string print_Inverse( const Inverse *c );
        std::string print_Real( const Real *c );
        std::string print_Scalar( const Scalar *s );
        std::string print_Sign( const Sign *s );
        std::string print_Solve( const Solve *s );
        std::string print_Symbol( const Symbol *symbol );
    };
};

#endif // __FORTRAN_PRINTER_H_
