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
