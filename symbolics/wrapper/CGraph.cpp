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

#include "CGraph.h"
#include "CAssignment.h"
#include "SymbolicsError.h"
#include "convert.h"
#include <string>
#include <Python.h>
#include "Util.h"
#include "ModelicaWriter.h"
#include "PythonWriter.h"
#include "MatlabWriter.h"
#include "FortranWriter.h"
#include "CWriter.h"
#include "FMUWriter.h"
#include "Writer.h"
#include "CBasic.h"
#include "CSymbol.h"
#include <map>


using namespace Symbolics::Python;

// Konstruktor
static int CGraph_init(CGraphObject *self, PyObject *args, PyObject *kwds);
static void CGraph_del(CGraphObject *self);

// Funktionen
static PyObject* CGraph_addVariable(CGraphObject *self, PyObject *args);
static PyObject* CGraph_getVariable(CGraphObject *self, PyObject *args);
static PyObject* CGraph_getVariables(CGraphObject *self, PyObject *args);
static PyObject* CGraph_getAssignments(CGraphObject *self, PyObject *args);
static PyObject* CGraph_getinitVal(CGraphObject *self, PyObject *args);
static PyObject* CGraph_addEquation(CGraphObject *self, PyObject *args);
static PyObject* CGraph_getShape(CGraphObject *self, PyObject *args);
static PyObject* CGraph_buildGraph(CGraphObject *self, PyObject *args);
static PyObject* CGraph_writeOutput(CGraphObject *self, PyObject *args, PyObject *kwds);

// Tabelle mit allen Funktionen
static PyMethodDef CGraph_methods[] = {
	{"addVariable",				(PyCFunction)CGraph_addVariable,				METH_VARARGS, "add a variable, throws exception if not successful"},
	{"getVariable",				(PyCFunction)CGraph_getVariable,				METH_VARARGS, "get a variable from the graph, returns None if not found"},
	{"getVariables",			(PyCFunction)CGraph_getVariables,				METH_VARARGS, "get all variable from the graph from given kind, returns None if not found"},
	{"getAssignments",			(PyCFunction)CGraph_getAssignments,				METH_VARARGS, "get all assignments from the graph for a given kind"},
	{"getinitVal",				(PyCFunction)CGraph_getinitVal,					METH_VARARGS, "get a initialisation value of a variable from the graph, returns None if not found"},
	{"addEquation",				(PyCFunction)CGraph_addEquation,				METH_VARARGS, "add an equation or a block of equations, throws exception if not successful"},
	{"getShape",				(PyCFunction)CGraph_getShape,					METH_VARARGS, "return the shape of an expression"},
	{"buildGraph",				(PyCFunction)CGraph_buildGraph,					METH_VARARGS, "build graph and perform optimizations"},
	{"writeOutput",				(PyCFunction)CGraph_writeOutput,	METH_VARARGS | METH_KEYWORDS, "write code, throws exception if not successful"},
	{NULL}
};

// Dokumentation
static char CGraph_doc[] = 
	"Very fast implementation of a graph for sympy objects";

// TypeObject
PyTypeObject Symbolics::Python::CGraphObjectType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"symbolics.CGraph",        /* tp_name           */
	sizeof(CGraphObject),    /* tp_basicsize      */
	0,                        /* tp_itemsize       */
	0,                        /* tp_dealloc        */
	0,                        /* tp_print          */
	0,                        /* tp_getattr        */
	0,                        /* tp_setattr        */
	0,                        /* tp_compare        */
	0,                        /* tp_repr           */
	0,                        /* tp_as_number      */
	0,                        /* tp_as_sequence    */
	0,                        /* tp_as_mapping     */
	0,                        /* tp_hash           */
	0,                        /* tp_call           */
	0,                        /* tp_str            */
	0,                        /* tp_getattro       */
	0,                        /* tp_setattro       */
	0,                        /* tp_as_buffer      */
	Py_TPFLAGS_DEFAULT,        /* tp_flags          */
	CGraph_doc,                /* tp_doc            */
	0,                        /* tp_traverse       */
	0,                        /* tp_clear          */
	0,                        /* tp_richcompare    */
	0,                        /* tp_weaklistoffset */
	0,                        /* tp_iter           */
	0,                        /* tp_iternext       */
	CGraph_methods,             /* tp_methods        */
	0,                        /* tp_members        */
	0,                        /* tp_getset         */
	0,                        /* tp_base           */
	0,                        /* tp_dict           */
	0,                        /* tp_descr_get      */
	0,                        /* tp_descr_set      */
	0,                        /* tp_dictoffset     */
	(initproc)CGraph_init,    /* tp_init           */
	0,                        /* tp_alloc          */
	0,                        /* tp_new            */
	0,                        /* tp_free           */
	0,                        /* tp_is_gc          */
	0,                        /* tp_bases          */
	0,                        /* tp_mro            */
	0,                        /* tp_cache          */
	0,                        /* tp_subclasses     */
	0,                        /* tp_weaklist       */
	(destructor)CGraph_del,    /* tp_del            */
};
#pragma endregion


#pragma region CGraph
// Konstruktor
/*****************************************************************************/
static int CGraph_init(CGraphObject *self, PyObject *args, PyObject *kwds)
	/*****************************************************************************/
{
	// Konstruktor aufrufen und damit neuen Graphen erstellen
	self->m_graph = new Symbolics::Graph::Graph();
	return 0;
}
/*****************************************************************************/


// Destruktor
/*****************************************************************************/
static void CGraph_del(CGraphObject *self)
	/*****************************************************************************/
{
	// Konstruktor aufrufen und damit neuen Graphen erstellen
	if (self->m_graph != NULL)
	{
		delete self->m_graph;
		self->m_graph = NULL;
	}
}
/*****************************************************************************/


// Funktionen
/*****************************************************************************/
static PyObject* CGraph_addVariable(CGraphObject *self, PyObject *args)
	/*****************************************************************************/
{
	try
	{
		// Name der Variable und Shape extrahieren
		const char *name = NULL;
		PyObject* shape_tuple = NULL;
		PyObject* initial = NULL;
		const char* comment;
		int kind;
		if (!PyArg_ParseTuple(args, "sOiOs", &name, &shape_tuple, &kind, &initial, &comment))
			return NULL;

		// Shape generieren
		Symbolics::Shape shape(toShape(shape_tuple));

		// Neues Symbol anlegen
		SymbolPtr symbol( new Symbol(name, shape, static_cast<Symbol_Kind>(kind) ) );

		// Name an Graphen weiterreichen
		if (initial != Py_None)
		{
			BasicPtr initialValue( toBasic(initial, self->m_graph) );
			self->m_graph->addSymbol( symbol, initialValue.get(), comment );
		}
		else            
			self->m_graph->addSymbol( symbol, NULL, comment );

		// Wenn wir bis hier her gekommen sind, dann Symbol als CSymbol zurueckgeben
		CSymbolObject* sym = new CSymbolObject();
		sym->m_basic = symbol;
		PyObject *obj = PyObject_Init( sym, &CSymbolObjectType );

		// RefCount erhoehen und zurueckgeben
		Py_IncRef(obj);
		return obj;
	}
	STD_ERROR_HANDLER(NULL);
}
/*****************************************************************************/


/*****************************************************************************/
static PyObject* CGraph_getVariable(CGraphObject *self, PyObject *args)
	/*****************************************************************************/
{
	try
	{
		// Name der Variable extrahieren
		const char *name = NULL;
		if (!PyArg_ParseTuple(args, "s", &name))
			return NULL;

		// Graphen fragen
		BasicPtr symbol = self->m_graph->getSymbol( name );

		// Haben wir es gefunden?
		if (symbol.get() == NULL)
		{
			Py_IncRef(Py_None);
			return Py_None;
		}

		// Wenn wir bis hier her gekommen sind, dann Symbol als CSymbol zurueckgeben
		CSymbolObject* sym = new CSymbolObject();
		sym->m_basic = symbol;
		PyObject *obj = PyObject_Init( sym, &CSymbolObjectType );

		// RefCount erhoehen und zurueckgeben
		Py_IncRef(obj);
		return obj;
	}
	STD_ERROR_HANDLER(NULL);
}
/*****************************************************************************/

/*****************************************************************************/
static PyObject* CGraph_getVariables(CGraphObject *self, PyObject *args)
	/*****************************************************************************/
{
	try
	{
		// Expression extrahieren
		int i;
		// Argumente parsen
		if (!PyArg_ParseTuple(args, "i", &i))
			return 0;

		// Graphen fragen
		SymbolPtrVec symbols = self->m_graph->getVariables( i );

		PyObject* pylist = PyList_New(symbols.size());

		for (size_t i=0;i<symbols.size();i++)
		{
			CSymbolObject* sym = new CSymbolObject();
			sym->m_basic = symbols[i];
			PyObject *obj = PyObject_Init( sym, &CSymbolObjectType );
			PyList_SetItem(pylist,i,obj);
		}


		// RefCount erhoehen und zurueckgeben
		Py_IncRef(pylist);
		return pylist;
	}
	STD_ERROR_HANDLER(NULL);
}
/*****************************************************************************/

/*****************************************************************************/
static PyObject* CGraph_getAssignments(CGraphObject *self, PyObject *args)
	/*****************************************************************************/
{
	try
	{
		// Expression extrahieren
		int kind;
		// Argumente parsen
		if (!PyArg_ParseTuple(args, "i", &kind))
			return 0;

		// Graphen fragen
		Graph::AssignmentsPtr a = self->m_graph->getAssignments(kind);
		// Gleichungen holen
		std::vector<Graph::Assignment> assignments = a->getEquations();

		PyObject* pylist = PyList_New(assignments.size());

		for (size_t i=0;i<assignments.size();i++)
		{
			CAssignmentObject* ass = new CAssignmentObject(assignments[i]);
			PyObject *obj = PyObject_Init( ass, &CAssignmentObjectType );
			PyList_SetItem(pylist,i,obj);
		}

		// RefCount erhoehen und zurueckgeben
		Py_IncRef(pylist);
		return pylist;
	}
	STD_ERROR_HANDLER(NULL);
}
/*****************************************************************************/

/*****************************************************************************/
static PyObject* CGraph_getinitVal(CGraphObject *self, PyObject *args)
	/*****************************************************************************/
{
	try
	{
		// Expression extrahieren
		PyObject *o;
		// Argumente parsen
		if (!PyArg_ParseTuple(args, "O", &o))
			return 0;

		SymbolPtr symbol(Util::getAsPtr<Symbol>(getBasic(o)));

		// Graphen fragen
		BasicPtr initvalue = self->m_graph->getinitVal( symbol );

		// Haben wir es gefunden?
		if (initvalue.get() == NULL)
		{
			Py_IncRef(Py_None);
			return Py_None;
		}

		// Wenn wir bis hier her gekommen sind, dann Symbol als CSymbol zurueckgeben
		CBasicObject* ival = new CBasicObject();
		ival->m_basic = initvalue;
		PyObject *obj = PyObject_Init( ival, &CBasicObjectType );

		// RefCount erhoehen und zurueckgeben
		Py_IncRef(obj);
		return obj;
	}
	STD_ERROR_HANDLER(NULL);
}
/*****************************************************************************/

/*****************************************************************************/
static PyObject* CGraph_addEquation(CGraphObject *self, PyObject *args)
	/*****************************************************************************/
{
	try
	{
		// lhs, rhs, category extrahieren
		PyObject *lhs_Object = NULL;
		PyObject *rhs_Object = NULL;
		PyObject *implicit_Object = Py_False;

		// Argumente parsen
		if (!PyArg_ParseTuple(args, "OO|O", &lhs_Object, &rhs_Object, &implicit_Object))
			return NULL;

		// implicit
		bool implicit = (implicit_Object == Py_True);

		// ist rhs eine List dann BasicVector
		if (PyList_Check(rhs_Object) && PyList_Check(lhs_Object))
		{
			size_t lhs_size = PyList_Size(lhs_Object);
			size_t rhs_size = PyList_Size(rhs_Object);
			if (lhs_size!=rhs_size)
				throw InternalError("Graph.addEquation called with invalid Arguments, Rhs size of list is different to Lhs list size!");

			BasicPtrVec rhs_vec, lhs_vec;
			for (size_t i=0; i<lhs_size; ++i)
			{
				lhs_vec.push_back(toBasic( PyList_GetItem(lhs_Object, i), self->m_graph));
				rhs_vec.push_back(toBasic( PyList_GetItem(rhs_Object, i), self->m_graph));
			}
			// Graphen aufrufen
			self->m_graph->addExpression( lhs_vec, rhs_vec, implicit );
		}
		else
		{

			// Ist lhs oder rhs eine Liste => dann einen Vektor anlegen
			BasicPtr lhs;
			if (PyList_Check(lhs_Object))
			{
				size_t size = PyList_Size(lhs_Object);
				Matrix *mat = new Matrix( Shape( size ) );
				for (size_t i=0; i<size; ++i)
					mat->set(i, toBasic( PyList_GetItem(lhs_Object, i), self->m_graph) );
				lhs = BasicPtr( mat );
			}
			else
				// sonst ganz normal
				if (lhs_Object!=Py_None)
					lhs = toBasic(lhs_Object, self->m_graph);

			BasicPtr rhs;
			if (PyList_Check(rhs_Object))
			{
				Matrix *mat;

				size_t m = PyList_Size(rhs_Object);
				if (m < 1) throw InternalError("Listsize must not be 0!");

				// Ist es ein Vektor (einfache Liste) oder eine Matrix (Liste in Liste)?
				PyObject *firstElement = PyList_GetItem(rhs_Object, 0);
				if (PyList_Check(firstElement))
				{
					// Es ist eine Matrix
					size_t n = PyList_Size(firstElement);
					if (n < 1) throw InternalError("Columncount must not be 0!");
					mat = new Matrix( Shape( m, n ) );
					for (size_t i=0; i<m; ++i)
					{
						PyObject *curr_Row = PyList_GetItem(rhs_Object, i);
						size_t colCount = PyList_Size(curr_Row);
						if (colCount != n) throw InternalError("Columncount must be constant!");
						for (size_t j=0; j<n; ++j)
							mat->set(i, j, toBasic(PyList_GetItem(curr_Row, j), self->m_graph) );
					}
					rhs = BasicPtr( mat );
				}
				else
				{
					// Es ist ein Vektor
					mat = new Matrix( Shape( m ) );
					for (size_t i=0; i<m; ++i)
						mat->set(i, toBasic( PyList_GetItem(rhs_Object, i), self->m_graph) );
					rhs = BasicPtr( mat );
				}
			}
			else
				// sonst ganz normal
				rhs = toBasic(rhs_Object, self->m_graph);
			// Graphen aufrufen
			self->m_graph->addExpression( lhs, rhs, implicit );
		}
	}
	STD_ERROR_HANDLER(NULL);

	// Refcount vorher erhoehen
	Py_IncRef(Py_None);
	return Py_None;
}
/*****************************************************************************/


/*****************************************************************************/
static PyObject* CGraph_getShape(CGraphObject *self, PyObject *args)
	/*****************************************************************************/
{
	try
	{
		// Expression extrahieren
		PyObject *o;
		// Argumente parsen
		if (!PyArg_ParseTuple(args, "O", &o))
			return NULL;
		// In ConstBasic aendern
		BasicPtr basic = toBasic(o, self->m_graph);
		// Shape abrufen und zurueckgeben
		PyObject *shape = fromShape(basic->getShape());
		return shape;
	}
	STD_ERROR_HANDLER(NULL);

	// Refcount vorher erhoehen
	Py_IncRef(Py_None);
	return Py_None;
}
/*****************************************************************************/


/*****************************************************************************/
static PyObject* CGraph_buildGraph(CGraphObject *self, PyObject *args)
	/*****************************************************************************/
{
	try
	{
		// Expression extrahieren
		PyObject *o;

		// Argumente parsen
		if (!PyArg_ParseTuple(args, "O", &o))
			return NULL;

		// Optimierung an oder aus?
		bool optimize = (o == Py_True);

		// Graphen aufbauen
		double t = self->m_graph->buildGraph( optimize );
		return PyFloat_FromDouble(t);
	}
	STD_ERROR_HANDLER(NULL);
	// Refcount vorher erhoehen
	Py_IncRef(Py_None);
	return Py_None;
}
/*****************************************************************************/


/*****************************************************************************/
std::map<std::string, std::string> parseKeywords(PyObject *kwds)
	/*****************************************************************************/
{
	// Leere map
	std::map<std::string, std::string> keywords;

	// Stimmt der Datentyp
	if (!PyDict_Check(kwds)) return keywords;

	// Keys holen
	PyObject *keys = PyDict_Keys(kwds);
	if (keys == NULL) return keywords;
	if (!PyList_Check(keys)) return keywords;

	// Liste mit Keys durchlaufen
	const size_t n = PyList_Size(keys);
	for (size_t i=0; i<n; ++i)
	{
		// Key holen
		PyObject *key = PyList_GetItem(keys, i);
		if (key == NULL) continue;

		// Item zum Key holen
		PyObject *item = PyDict_GetItem(kwds, key);
		if (item == NULL) continue;

		// Key, Item als String speichern
		std::string strKey = PyUnicode_AsUTF8(key);
		PyObject *str = PyObject_Str(item);
		if (str == NULL) continue;
		std::string strItem = PyUnicode_AsUTF8(str);
		keywords[strKey] = strItem;
	}

	// Fertig
	return keywords;
}
/*****************************************************************************/


/*****************************************************************************/
static PyObject* CGraph_writeOutput(CGraphObject *self, PyObject *args, PyObject *kwds)
	/*****************************************************************************/
{
	try
	{
		// Argumente extrahieren
		char *typeString = NULL;
		char *name = NULL;
		char *path = NULL;

		double t;
		Writer *writer;

		// Argumente parsen
		if (!PyArg_ParseTuple(args, "sss", &typeString, &name, &path))
			return NULL;

		// Keywords parsen
		std::map<std::string, std::string> keywords = parseKeywords(kwds);

		// Target emitteln
		if (!strcmp(typeString, "mo"))
			writer = new ModelicaWriter(keywords);

		if (!strcmp(typeString, "py"))
			writer = new PythonWriter(keywords);

		if (!strcmp(typeString, "m"))
			writer = new MatlabWriter(keywords);

		if(!strcmp(typeString, "f90"))
			writer = new FortranWriter(keywords);

		if(!strcmp(typeString, "c"))
			writer = new CWriter(keywords);

		if(!strcmp(typeString, "fmu"))
			writer = new FMUWriter(keywords);

		// Code schreiben
		t = writer->generateTarget(name, path, *(self->m_graph), true);
		return PyFloat_FromDouble(t);
	}
	STD_ERROR_HANDLER(NULL);

	// Refcount vorher erhoehen
	Py_IncRef(Py_None);
	return Py_None;
}
/*****************************************************************************/

#pragma endregion
