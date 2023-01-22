#ifndef __C_WRITER_H_
#define __C_WRITER_H_

#include "Writer.h"
#include "CPrinter.h"

namespace Symbolics
{
    class CWriter: public Writer
    {
    public:
        // Konstruktor
        CWriter( std::map<std::string, std::string> &kwds );
        CWriter();
        // Destruktor
        ~CWriter();

    protected:
        double generateTarget_Impl(Graph::Graph& g);

		CPrinter *m_p; // Der Hauptprinter dieser Writerklasse

		std::string writeEquations(std::vector<Graph::Assignment> const& equations) const;
		double generateFunctionmodule(int n);

    private:
		bool m_pymbs_wrapper;
		bool m_simulink_sfunction;
		bool m_include_visual;

		double generateDerState(Graph::Graph& g, int &dim);
		double generateVisual(Graph::Graph& g);
		double generateSensors(Graph::Graph& g);
        
		double generatePymbsWrapper(Graph::Graph& g);

		double generateAll(Graph::Graph& g, int &dim);

    };
};

#endif // __C_WRITER_H_
