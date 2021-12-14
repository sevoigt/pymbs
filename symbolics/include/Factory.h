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

#ifndef __FACTORY_H_
#define __FACTORY_H_

#include "Symbolics.h"

// Declarations
namespace Symbolics
{
    /*****************************************************************************/
    class Factory
    {
    private:
        Factory();
        ~Factory();

    public:
        static BasicPtr newBasic( Basic_Type type,  BasicPtr &arg, Shape const& shape );
        static BasicPtr newBasic( Basic_Type type,  BasicPtr &arg1, BasicPtr &arg2, Shape const& shape );
        static BasicPtr newBasic( Basic_Type type,  BasicPtrVec &args, Shape const& shape );
    };
    /*****************************************************************************/
};
#endif // __FACTORY_H_