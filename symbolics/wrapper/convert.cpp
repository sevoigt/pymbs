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

#include "convert.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include "CBasic.h"
#include "str.h"

#define max(a,b) (a > b) ? a : b

using namespace Symbolics;

#pragma region gatherArgs
/*****************************************************************************/
// Elemente aus einem Tuple zusammensuchen
/*****************************************************************************/
BasicPtrVec gatherArgs( PyObject *o, Graph::Graph* &g )
/*****************************************************************************/
{
    // Auf NULL pruefen
    if (o == NULL) throw InternalError("Internal Error: gatherArgs - Argument must not be NULL!");

    // Vektor anlegen
    BasicPtrVec res;

    // Args holen
    PyObject *args = PyObject_GetAttrString(o, "args");
    if (args == NULL) throw InternalError("args ist Null?!");

    // Groesse
     size_t nargs = PyTuple_Size(args);
    if (nargs == 0) return res;
    
    // Alle restlichen Elemente holen
    for( size_t i=0; i<nargs; ++i)
        res.push_back( toBasic( PyTuple_GetItem(args,i), g ) );

    // Fertig
    return res;
}
/*****************************************************************************/
#pragma endregion

#pragma region toBasic
/*****************************************************************************/
// Den internen sympy Baum durchlaufen
/*****************************************************************************/
BasicPtr toBasic( PyObject *o, Graph::Graph* &g)
/*****************************************************************************/
{
    // Auf NULL pruefen
    if (o == NULL) 
        throw InternalError("Internal Error: toBasic - Argument must not be NULL!");

	// Falls es ein CBasic ist, sind wir schon fertig
	BasicPtr const& b = getBasic(o);
	if (b.get() != NULL) return b;

    // Was haben wir fuer ein Objekt
    PyObject* type = PyObject_Type(o);
    if (type == NULL) throw InternalError("type ist Null?!");

    // Namen extrahieren
    PyObject *attrTypeName = PyObject_GetAttrString(type, "__name__");
    if (attrTypeName == NULL) throw InternalError("attrTypeName ist Null?!");
    std::string typeName = PyUnicode_AsUTF8(attrTypeName);

    // Klassen unterscheiden
    // Symbol
    if (typeName == "Variable")
    {
        // Namen des Symbols extrahieren
        PyObject *nameObj = PyObject_GetAttrString(o, "name");
        if (nameObj == NULL) throw InternalError("name ist Null?!");
         const char *name = PyUnicode_AsUTF8(nameObj);
        // Und versuche es aus dem Graphen zu holen
        return g->getSymbol( name );
    }
    // Float
    if ((typeName == "Float") || (typeName == "Real") || (typeName == "Rational"))
    {
        // Konstante zurueckgeben
         double val = PyFloat_AsDouble(o);
        return BasicPtr( new Real(val) );
    }
    // Integer
    if (typeName == "Integer")
    {
        // Konstante zurueckgeben
         int val = static_cast<int>(PyLong_AsLong(o));
        // Konstante zurueckgeben
        return BasicPtr( new Int(val) );
    }
    // One
    if (typeName == "One")
        return Int::getOne();
    // NegativeOne
    if (typeName == "NegativeOne")
        return Int::getMinusOne();
    // Zero
    if (typeName == "Zero")
        return Zero::getZero();

    // Mul
    if (typeName == "Mul")
    {
        // Args bekommen
        BasicPtrVec args = gatherArgs( o, g );
        // Mul erstellen
        return BasicPtr( new Mul(args) );
    }
    // Add
    if (typeName == "Add")
    {
        // Args bekommen
        BasicPtrVec args = gatherArgs( o, g );
        // Add erstellen
        return BasicPtr( new Add(args) );
    }
    // Pow
    if (typeName == "Pow")
    {
        // Args holen
        PyObject *args = PyObject_GetAttrString(o, "args");
        // Auf Tuple pruefen
        if (!PyTuple_Check(args)) throw InternalError("Internal Error: Pow - args must be a tuple!");
        if (PyTuple_Size(args) != 2) throw InternalError("Internal Error: PowArgs tuple must be of size 2!");
        // Args: arg, dim1, dim2
        BasicPtr arg = toBasic( PyTuple_GetItem(args, 0), g );
         double exponent = PyFloat_AsDouble( PyTuple_GetItem(args, 1) );
        BasicPtr exp(new Real(exponent));
        return Pow::New(arg, exp);
    }
    // Cosinus
    if (typeName == "cos")
        return toUnary<Cos>(o,g);
    // Sinus
    if (typeName == "sin")
        return toUnary<Sin>(o,g);

    // Matrix
    if (typeName == "Matrix")
    {
        // Shape holen
        PyObject *shapeObj = PyObject_GetAttrString(o, "shape");
        // In Shape wandeln
        Shape shape( toShape(shapeObj) );
        // Auf Vektor pruefen
        if (shape.getDimension(2) == 1)
            shape = Shape(shape.getDimension(1));
        // Neue Matrix anlegen
        Matrix *mat = new Matrix( shape );
        // Elemente kopieren
        for (size_t m=0; m<shape.getDimension(1); ++m)
        {
            for (size_t n=0; n<shape.getDimension(2); ++n)
            {
                PyObject *el = PyObject_GetItem(o, Py_BuildValue("(ii)", m,n));
                if (el == NULL)
                {
                     std::string msg = "Internal Error: Could not get Matrix Element (" + str(m) + "," + str(n) + ")!";
                    throw InternalError(msg.c_str());
                }
                mat->set(m,n, toBasic( el, g ) );
            }
        }
        
        // Matrix zurueckgeben
        return BasicPtr( mat );
    }
    // SymmetricMatrix
    if (typeName == "SymmetricMatrix")
    {
        // Shape holen
        PyObject *shapeObj = PyObject_GetAttrString(o, "shape");
        // In Shape wandeln
        Shape shape( toShape(shapeObj) );
        // Neue Matrix anlegen
        SymmetricMatrix *mat = new SymmetricMatrix( shape );
        // Elemente kopieren
        for (size_t m=0; m<shape.getDimension(1); ++m)
        {
            for (size_t n=m; n<shape.getDimension(2); ++n)
            {
                PyObject *el = PyObject_GetItem(o, Py_BuildValue("(ii)", m,n));
                if (el == NULL)
                {
                     std::string msg = "Internal Error: Could not get Matrix Element (" + str(m) + "," + str(n) + ")!";
                    throw InternalError(msg.c_str());
                }
                mat->set(m,n, toBasic( el, g ));
            }
        }
        
        // Matrix zurueckgeben
        return BasicPtr( mat );
    }
    // Skew
    if (typeName == "skew")
        return toUnary<Skew>(o,g);
    if (typeName == "transpose")
        return toUnary<Transpose>(o,g);

    // Element
    if (typeName == "element")
    {
        // Args holen
        PyObject *args = PyObject_GetAttrString(o, "args");
        // Auf Tuple pruefen
        if (!PyTuple_Check(args)) throw InternalError("Internal Error: element - args must be a tuple!");
        if (PyTuple_Size(args) != 3) throw InternalError("Internal Error: elementArgs tuple must be of size 3!");
        // Args: arg, dim1, dim2
        BasicPtr arg = toBasic( PyTuple_GetItem(args, 0), g );
         size_t dim1 = PyLong_AsSsize_t( PyTuple_GetItem(args, 1) );
         size_t dim2 = PyLong_AsSsize_t( PyTuple_GetItem(args, 2) );
        return Element::New(arg, dim1, dim2);
    }
    // Atan2
    if (typeName == "atan2")
        return toBinary<Atan2>(o,g);
    // Atan
    if (typeName == "atan")
        return toUnary<Atan>(o,g);
    // Pi
    if (typeName == "Pi")
        return BasicPtr( new Real( M_PI ) );
    // Der
    if (typeName == "der")
        return toUnary<Der>(o,g);
    // Abs
    if ((typeName == "Abs") || (typeName == "abs"))
        return toUnary<Abs>(o,g);
    // Acos
    if (typeName == "acos")
        return toUnary<Acos>(o,g);
    // Asin
    if (typeName == "asin")
        return toUnary<Asin>(o,g);
    // Half
    if (typeName == "Half")
        return BasicPtr( new Real(0.5) );
    // Scalar
    if (typeName == "scalar")
        return toUnary<Scalar>(o,g);
    // Solve
    if (typeName == "solve")
        return toBinary<Solve>(o,g);

    // Unbekannte Klasse
    std::cout << "Unknown Function '" + typeName + "'! Trying to ignore it." << std::endl;
    // Args holen
    BasicPtrVec args = gatherArgs( o, g );
    // Unknown erstellen
    return BasicPtr( new Unknown(typeName, args) );

}
/*****************************************************************************/
#pragma endregion


#pragma region toBasicVec
/*****************************************************************************/
// Eine Liste nach std::vec konvertieren
/*****************************************************************************/
std::vector< BasicPtr> toBasicVec( PyObject *o, Graph::Graph* &g )
/*****************************************************************************/
{
    // Im Falle eines Falles, Skalar
    if (o == NULL) throw InternalError("Internal Error: toBasicVec - Argument must not be NULL!");

    // Auf Tuple pruefen
    if (!PyList_Check(o)) throw InternalError("Internal Error: toBasicVec - Argument must be a tuple!");

    // Groesse holen
     size_t l = PyList_Size(o);
    
    // Alle Elemente konvertieren
    std::vector<BasicPtr> vec;
    for (size_t i=0; i<l; ++i)
    {
        BasicPtr basic = toBasic( PyList_GetItem(o,i), g );
        vec.push_back(basic);
    }

    // Fertig
    return vec;
}
/*****************************************************************************/
#pragma endregion


#pragma region Shape
/*****************************************************************************/
// Ein Tupel in einen Shape wandeln
/*****************************************************************************/
Shape toShape( PyObject *tuple )
/*****************************************************************************/
{
    // Im Falle eines Falles, Skalar
    if (tuple == NULL) return Shape();

	// Auf Int pruefen
	if (PyLong_Check(tuple))
	{
		const size_t n = PyLong_AsSsize_t(tuple);
		return Shape(n,n);
	}

    // Auf Tuple pruefen
    if (!PyTuple_Check(tuple)) throw InternalError("Internal Error: toShape - Argument must be a tuple!");

    // Laenge feststellen
    const size_t l = PyTuple_Size( tuple );

    // Anhand der Laenge unterscheiden
    switch(l)
    {
    case 0:
        return Shape();
    case 1:
        {
            // Erste Dimension holen
            PyObject *dim1 = PyTuple_GetItem(tuple, 0);
            if (dim1 == NULL) throw InternalError("Internal Error: toShape(1)!");
            // Shape erstellen
            if (!PyLong_Check(dim1)) throw InternalError("Internal Error: toShape - dim1 must be an integer!");
            return Shape( PyLong_AsSsize_t( dim1 ) );
        }
    case 2:
        {
            // Erste Dimension holen
            PyObject *dim1 = PyTuple_GetItem(tuple, 0);
            if (dim1 == NULL) throw InternalError("Internal Error: toShape(2)!");
            // Zweite Dimension holen
            PyObject *dim2 = PyTuple_GetItem(tuple, 1);
            if (dim2 == NULL) throw InternalError("Internal Error: toShape(2)!");
            // Shape erstellen
            if (!PyLong_Check(dim1)) throw InternalError("Internal Error: toShape - dim1 must be an integer!");
            if (!PyLong_Check(dim2)) throw InternalError("Internal Error: toShape - dim2 must be an integer!");
            return Shape( PyLong_AsSsize_t( dim1 ), PyLong_AsSsize_t( dim2 ) );
        }
	case 3:
        {
            // Anzahl Dimensionen
            PyObject *ndim = PyTuple_GetItem(tuple, 0);
            if (ndim == NULL) throw InternalError("Internal Error: toShape(3)!");
            // Erste Dimension holen
            PyObject *dim1 = PyTuple_GetItem(tuple, 1);
            if (dim1 == NULL) throw InternalError("Internal Error: toShape(3)!");
            // Zweite Dimension holen
            PyObject *dim2 = PyTuple_GetItem(tuple, 2);
            if (dim2 == NULL) throw InternalError("Internal Error: toShape(3)!");
            // Shape erstellen
            if (!PyLong_Check(ndim)) throw InternalError("Internal Error: toShape - ndim must be an integer!");
            if (!PyLong_Check(dim1)) throw InternalError("Internal Error: toShape - dim1 must be an integer!");
            if (!PyLong_Check(dim2)) throw InternalError("Internal Error: toShape - dim2 must be an integer!");
            return Shape( PyLong_AsSsize_t( ndim ), PyLong_AsSsize_t( dim1 ), PyLong_AsSsize_t( dim2 ) );
        }

    default:
        throw InternalError("Internal Error: toShape - len(tuple)>3!");
    }
}
/*****************************************************************************/


/*****************************************************************************/
// Ein Shape in ein Tupel wandeln
/*****************************************************************************/
PyObject* fromShape(  Shape const& shape )
/*****************************************************************************/
{
    // Anhand der Dimension unterscheiden
	const int m = shape.getDimension(1);
	const int n = shape.getDimension(2);

    switch(shape.getNrDimensions())
    {
    case 0:
        return Py_BuildValue("()");
    case 1:
        return Py_BuildValue("(i)", max(m,n) );
    case 2:
        return Py_BuildValue("(ii)", m, n);
    default:
        throw InternalError("Internal Error: fromShape - Unsupported Number of Dimensions!");
    }
}
/*****************************************************************************/
#pragma endregion


#pragma region toUnary/Binary
/*****************************************************************************/
// PyObject in Unary
/*****************************************************************************/
template < class T > BasicPtr toUnary( PyObject *o, Graph::Graph* &g )
/*****************************************************************************/
{
    // Args holen
    PyObject *args = PyObject_GetAttrString(o, "args");
    // Auf Tuple pruefen
    if (!PyTuple_Check(args)) throw InternalError("Internal Error: Unary - args must be a tuple!");
    if (PyTuple_Size(args) != 1) throw InternalError("Internal Error: UnaryArgs tuple must be of size 1!");
    // Args: arg
    BasicPtr arg = toBasic( PyTuple_GetItem(args, 0), g );
    return T::New(arg);
}
/*****************************************************************************/


/*****************************************************************************/
// PyObject in Binary
/*****************************************************************************/
template < class T > BasicPtr toBinary( PyObject *o, Graph::Graph* &g )
/*****************************************************************************/
{
    // Args holen
    PyObject *args = PyObject_GetAttrString(o, "args");
    // Auf Tuple pruefen
    if (!PyTuple_Check(args)) throw InternalError("Internal Error: Binary - args must be a tuple!");
    if (PyTuple_Size(args) != 2) throw InternalError("Internal Error: BinaryArgs tuple must be of size 2!");
    // Args: arg1 und arg2
    BasicPtr arg1 = toBasic( PyTuple_GetItem(args, 0), g );
    BasicPtr arg2 = toBasic( PyTuple_GetItem(args, 1), g );
    return T::New(arg1, arg2);
}
/*****************************************************************************/
#pragma endregion

/*****************************************************************************/
// PyObject in BasicPtr
/*****************************************************************************/
BasicPtr getBasic( PyObject *o )
/*****************************************************************************/
{
    // Type == CBasic
    if (PyType_IsSubtype(Py_TYPE(o), &Symbolics::Python::CBasicObjectType))
    {
        Symbolics::Python::CBasicObject* basic = reinterpret_cast<Symbolics::Python::CBasicObject*>(o);
        return basic->m_basic;
    }
    // Type == int
    if (PyLong_Check(o))
        return BasicPtr( new Int( PyLong_AsLong(o) ) );
    // Type == float
    if (PyFloat_Check(o))
        return BasicPtr( new Real( PyFloat_AsDouble(o) ) );

    // Es hat nicht geklappt
	throw InternalError("Object could not be cast into a basic!");
}
/*****************************************************************************/
