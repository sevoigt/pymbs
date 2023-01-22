#ifndef __FMU_PRINTER_H_
#define __FMU_PRINTER_H_

#include "CPrinter.h"
#include <map>

namespace Symbolics
{
    class FMUPrinter: public Symbolics::CPrinter
    {
    public:
        // Konstruktor
        FMUPrinter(const std::map<std::string, int> *valueReferences);
        // Destruktor
        ~FMUPrinter();

    protected:
		const std::map<std::string, int> *m_valueReferencesP;

		// Funktionen die sich vom CPrinter unterscheiden, ueberschreiben
        std::string print_Symbol( const Symbol *symbol );
		std::string print_Element( const Element *e );
		std::string print_Der( const Der *d );
        std::string print_Bool( const Bool *b );

    };
};

#endif // __FMU_PRINTER_H_
