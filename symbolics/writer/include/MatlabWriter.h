#ifndef __MATLAB_WRITER_H_
#define __MATLAB_WRITER_H_

#include "Writer.h"
#include "MatlabPrinter.h"

namespace Symbolics
{
    class MatlabWriter: public Writer
    {
    public:
        // Konstruktor
        MatlabWriter( std::map<std::string, std::string> &kwds );
        MatlabWriter();
        // Destruktor
        ~MatlabWriter();

    protected:
        double generateTarget_Impl(Graph::Graph& g);
        std::string writeEquations(std::vector<Graph::Assignment> const& equations) const;

        MatlabPrinter *m_p; // Der Hauptprinter dieser Writerklasse
        bool m_symbolicmode;

    private:
		double generateBasicSim(Graph::Graph& g);
		double generateDerState(Graph::Graph& g);
		double generateInputs(Graph::Graph& g);
		double generateSensors(Graph::Graph& g);

		double generateSymDerState(Graph::Graph& g);
		double generateSymSensors(Graph::Graph& g);
    };
};

#endif // __MATLAB_WRITER_H_
