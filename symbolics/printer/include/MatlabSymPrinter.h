#ifndef __MATLAB_SYM_PRINTER_H_
#define __MATLAB_SYM_PRINTER_H_

#include "MatlabPrinter.h"

namespace Symbolics
{
    class MatlabSymPrinter: public MatlabPrinter
    {
    public:
        // Konstruktor
        MatlabSymPrinter();
        // Destruktor
        ~MatlabSymPrinter();

        // Bei der Deklaration von symbolischen Variablen wird dimension gebraucht
        std::string dimension( BasicPtr const& basic );

    protected:
        // Funktionen die sich vom Standard unterscheiden, ueberschreiben
        std::string print_Atan2( const Atan2 *s );
        std::string print_Sign( const Sign *s );

    };
};

#endif // __MATLAB_SYM_PRINTER_H_
