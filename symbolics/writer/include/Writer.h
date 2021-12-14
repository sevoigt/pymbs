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

#ifndef __WRITER_H_
#define __WRITER_H_

#include <string>
#include <vector>
#include "Symbolics.h"
#include "Graph.h"

namespace Symbolics
{
    class Writer
    {
    public:
/*****************************************************************************/
        typedef enum _CATEGORY_
        {
            SENSOR = 0x100,                  // Expressions Defined by the User
            SENSOR_VISUAL = 0x200,           // Expressions Defined by the User
        } Category;
/*****************************************************************************/

    protected:
        // privater Konstruktor, keine Objekte erstellen
        Writer(bool makeScalar);
        // Destruktor
        virtual ~Writer();

        // file
        std::string m_path;
        std::string m_name;

        virtual double generateTarget_Impl(Graph::Graph& g) = 0;

        std::string getHeaderLine();

		// Vielleicht ist folgende Funktion logisch sinnvoller in einer anderen Klasse aufgehoben - wenn jemand weiß wo, einfach ändern.
		// Die Funktion ist als Parameter für die std::sort Funktion gedacht, um Graph::VariableVec Vektoren zu sortieren.
		// Die Funktion muss static sein, da sie als functionpointer verwendet wird
		static bool sortVariableVec (boost::intrusive_ptr<Symbolics::Symbol> a, boost::intrusive_ptr<Symbolics::Symbol> b);

    public:
        double generateTarget(std::string name, std::string path, Graph::Graph& g, bool optimize);

    private:
        bool m_scalar;
        

    };
};

#endif // __WRITER_H_
