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

#ifndef __PYTHONWRITER_WRITER_H_
#define __PYTHONWRITER_WRITER_H_

#include "Writer.h"
#include "PythonPrinter.h"

namespace Symbolics
{
  std::vector<std::string> split(const std::string &s, char delim);

  class PythonWriter: public Writer
  {
  public:
    // Konstruktor
    PythonWriter( std::map<std::string, std::string> &kwds );
    PythonWriter();
    // Destruktor
    ~PythonWriter();

  protected:
    double generateTarget_Impl(Graph::Graph& g);
	std::string writeEquations(std::vector<Graph::Assignment> const& equations) const;

    PythonPrinter *m_p; // Der Hauptprinter dieser Writerklasse

  private:
    double generateStateDerivative(Graph::Graph& g);
    double generateVisualSensors(Graph::Graph& g);
    double generateSensors(Graph::Graph& g);
  };
};

#endif // __PYTHONWRITER_WRITER_H_
