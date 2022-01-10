#ifndef __CSHARP_WRITER_H_
#define __CSHARP_WRITER_H_

#include "Writer.h"
#include "CSharpPrinter.h"

namespace Symbolics
{
    class CSharpWriter: public Writer
    {
    public:
        // Constructor
        CSharpWriter( std::map<std::string, std::string> &kwds );
        CSharpWriter();
        // Destructor
        ~CSharpWriter();

    protected:
        double generateTarget_Impl(Graph::Graph& g);

		CSharpPrinter *m_p; // Der Hauptprinter dieser Writerklasse

		std::string writeEquations(std::vector<Graph::Assignment> const& equations) const;

    private:
		bool m_include_visual;

		double generateDerState(Graph::Graph& g, int &dim);
		double generateVisual(Graph::Graph& g);
		double generateSensors(Graph::Graph& g);

		double generateAll(Graph::Graph& g, int &dim);

    };
};

#endif // __CSHARP_WRITER_H_
