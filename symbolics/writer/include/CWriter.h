/*
This file is part of PyMbs.

PyMbs is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as 
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

PyMbs is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public 
License along with PyMbs.
If not, see <http://www.gnu.org/licenses/>.

Copyright 2011, 2012 Carsten Knoll, Christian Schubert, 
                     Jens Frenkel, Sebastian Voigt
*/

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
