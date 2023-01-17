#ifndef __FORTRAN_WRITER_H_
#define __FORTRAN_WRITER_H_

#include "Writer.h"
#include "FortranPrinter.h"

namespace Symbolics
{
    class FortranWriter: public Writer
    {
    public:
        // Konstruktor
        FortranWriter( std::map<std::string, std::string> &kwds );
        FortranWriter();
        // Destruktor
        ~FortranWriter();

    protected:
        double generateTarget_Impl(Graph::Graph& g);
        
        std::string writeEquations(std::vector<Graph::Assignment> const& equations, std::vector<std::string> &additionalVarDefs) const;
        double generateFunctionmodule();

        FortranPrinter *m_p; // Der Hauptprinter dieser Writerklasse

    private:
		bool m_pymbs_wrapper;

		double generateDerState(Graph::Graph& g);
		double generateSensors(Graph::Graph& g);
		double generateVisual(Graph::Graph& g);

		double generatePymbsWrapper(Graph::Graph& g);
    };
};

#endif // __FORTRAN_WRITER_H_
