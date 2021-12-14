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

#ifndef __FMU_WRITER_H_
#define __FMU_WRITER_H_

#include "CWriter.h"

namespace Symbolics
{
    class FMUWriter: public CWriter
    {
    public:
        // Konstruktor
        FMUWriter( std::map<std::string, std::string> &kwds );
        FMUWriter();
        // Destruktor
        ~FMUWriter();

    protected:
        double generateTarget_Impl(Graph::Graph& g);

		std::string m_base_path;
		std::string m_util_folder, m_7_Zip_app;
        bool m_compile;

    private:
		void init();
        std::vector<std::string> split_path(std::string path);

		double generateXML(Graph::Graph& g);
		double generateModel(Graph::Graph& g, int &dim);

       	int m_numberOfStates;
        std::string m_guid;
        std::map<std::string, int> m_valueReferences;

    };
};

#endif // __FMU_WRITER_H_
