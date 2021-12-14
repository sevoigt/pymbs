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
