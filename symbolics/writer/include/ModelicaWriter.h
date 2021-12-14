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

#ifndef __MODELICA_WRITER_H_
#define __MODELICA_WRITER_H_

#include "Writer.h"

namespace Symbolics
{
    class ModelicaWriter: public Writer
    {
    public:
      // Konstruktor
		  ModelicaWriter();
		  ModelicaWriter( std::map<std::string, std::string> &kwds );
      // Destruktor
      ~ModelicaWriter();

    protected:
		  // Settings
		  bool m_InputsAsInputs;
      std::string package;

      double generateTarget_Impl(Graph::Graph& g);

    };
};

#endif // __MODELICA_WRITER_H_
