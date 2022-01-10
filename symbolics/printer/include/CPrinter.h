#ifndef __C_PRINTER_H_
#define __C_PRINTER_H_

#include "Printer.h"

namespace Symbolics
{
    class CPrinter: public Symbolics::Printer
    {
    public:
        // Konstruktor
        CPrinter();
        // Destruktor
        ~CPrinter();

        //comment Funktion �berschreiben
        std::string comment( std::string const& comment ) const;

        std::string dimension( BasicPtr const& basic );

    protected:
        //geforderte Funktionen �berschreiben
        std::string print_Element( const Element *e );
        std::string print_Matrix( const Matrix *mat );
        std::string print_Skew( const Skew *s );  // Nur Fehlermeldung
        std::string print_Pow( const Pow *pow );
        std::string print_Zero( const Zero *z );
        std::string print_Bool( const Bool *b );

		//Funktionen die sich vom Standard unterscheiden, ueberschreiben
		std::string print_Abs ( const Abs *s );
        std::string print_If( const If *e );
        //std::string print_Int( const Int *c ); //Ints als doubles ausgeben?
        //std::string print_Inverse ( const Inverse *c );
        std::string print_Sign( const Sign *s );
        std::string print_Solve( const Solve *s ); // Nur Fehlermeldung hier
        std::string print_Transpose( const Transpose *s ); //Nur Fehlermeldung
    };
};

#endif // __C_PRINTER_H_
