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

#ifndef __UNKNOWN_H_
#define __UNKNOWN_H_

#include "NaryOp.h"

namespace Symbolics
{
    class Unknown: public NaryOp
    {
    public:
        // Konstruktor
        Unknown(  std::string const& name, BasicPtrVec const& args );
        // Destruktor
        ~Unknown();
    
        // Name
        inline std::string getName() const  { return m_name; };

        // toString
        std::string toString() const;

        // Vereinfachen, wenn unverändert, dann NULL
        BasicPtr simplify();

        // Ersetzen
         BasicPtr subs( ConstBasicPtr const& old_exp,  BasicPtr const& new_exp);
        // Iterate
         BasicPtr iterateExp( Iterator &v );

		    static BasicPtr New( std::string const& name, BasicPtrVec const& args );

        // derivative
        BasicPtr der();
        BasicPtr der(BasicPtr const& symbol);

    protected:
        // Name der Unbekannten Funktion
         std::string m_name;
    };

    typedef boost::intrusive_ptr< Unknown> ConstUnknownPtr;
    typedef boost::intrusive_ptr<Unknown> UnknownPtr;
};

#endif // __UNKNOWN_H_