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