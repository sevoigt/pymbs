#ifndef __CSharp_PRINTER_H_
#define __CSharp_PRINTER_H_

#include "Printer.h"

namespace Symbolics
{
    class CSharpPrinter: public Symbolics::Printer
    {
    public:
        // Construktor
        CSharpPrinter();
        // Destruktor
        ~CSharpPrinter();

        // Overwrite comment method
        std::string comment( std::string const& comment ) const;

        std::string dimension( BasicPtr const& basic );

    protected:
        // Overwrite methods
        std::string print_Element( const Element *e );
        std::string print_Matrix( const Matrix *mat );
        std::string print_Skew( const Skew *s );
        std::string print_Pow( const Pow *pow );
        std::string print_Zero( const Zero *z );
        std::string print_Bool( const Bool *b );

		// Overwrite methods different from standard
		std::string print_Abs ( const Abs *s );
        std::string print_If( const If *e );
        //std::string print_Int( const Int *c );
        
        std::string print_Jacobian( const Jacobian *j );
        std::string print_Inverse ( const Inverse *c );
        std::string print_Sign( const Sign *s );
        std::string print_Solve( const Solve *s );
        std::string print_Transpose( const Transpose *s );  
        
        std::string print_Acos( const Acos *s );
        std::string print_Asin( const Asin *s );
        std::string print_Atan( const Atan *s );
        std::string print_Atan2( const Atan2 *s );
        std::string print_Cos( const Cos *c );
        std::string print_Sin( const Sin *s );
        std::string print_Tan( const Tan *s );
    };
};

#endif // __CSharp_PRINTER_H_
