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

#ifndef __TOBASIC_H_
#define __TOBASIC_H_

#include <Python.h>
#include <vector>
#include "Symbolics.h"
#include "Graph.h"

using namespace Symbolics;

// Elemente aus einem Tuple zusammensuchen
BasicPtrVec gatherArgs( PyObject *o, Graph::Graph* &g );
// Den internen sympy Baum durchlaufen
BasicPtr toBasic( PyObject *o, Graph::Graph* &g );
// Eine Liste nach std::vec konvertieren
BasicPtrVec toBasicVec( PyObject *o, Graph::Graph* &g );
// Ein Tupel in einen Shape wandeln
Shape toShape( PyObject *tuple );
// Ein Shape in ein Tupel wandeln
PyObject* fromShape(  Shape const& shape );

// PyObject in Unary
template < class T > BasicPtr toUnary( PyObject *o, Graph::Graph* &g );
// PyObject in Binary
template < class T > BasicPtr toBinary( PyObject *o, Graph::Graph* &g );

// PyObject in BasicPtr
BasicPtr getBasic( PyObject *o );

#endif // __TOBASIC_H_