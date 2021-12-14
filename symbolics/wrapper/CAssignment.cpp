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

#include "CAssignment.h"
#include "CBasic.h"
#include "PythonPrinter.h"
#include "SymbolicsError.h"
#include "convert.h"
#include "PyFactory.h"

using namespace Symbolics::Python;

#pragma region CAssignment

// Konstruktor
static int CAssignment_init(CAssignmentObject *self, PyObject *args, PyObject *kwds);
static void CAssignment_del(CAssignmentObject *self);

static PyObject* CAssignment_str(CAssignmentObject *self);

static PyObject* CAssignment_getLhs(CAssignmentObject *self, void* data);
static PyObject* CAssignment_getRhs(CAssignmentObject *self, void* data);

// Tabelle mit allen Attributen
static PyGetSetDef CAssignment_getset[] = {
    {"lhs",				(getter)CAssignment_getLhs,		NULL,	"get lhs of assignment", NULL},
    {"rhs",				(getter)CAssignment_getRhs,		NULL,	"get rhs of assignment", NULL},
    {NULL}
};

// Dokumentation
static char CAssignment_doc[] = 
    "CAssignment: Class representing a single assignment";

// TypeObject
PyTypeObject Symbolics::Python::CAssignmentObjectType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"symbolics.CAssignment",      /* tp_name           */
    sizeof(CAssignmentObject),    /* tp_basicsize      */
    0,                            /* tp_itemsize       */
    0,                            /* tp_dealloc        */
    0,                            /* tp_print          */
    0,							  /* tp_getattr        */
    0,                            /* tp_setattr        */
    0,					          /* tp_compare        */
    0,                            /* tp_repr           */
    0,                            /* tp_as_number      */
    0,                            /* tp_as_sequence    */
    0,                            /* tp_as_mapping     */
    0,                            /* tp_hash           */
    0,                            /* tp_call           */
    0,                            /* tp_str            */
    0,                            /* tp_getattro       */
    0,                            /* tp_setattro       */
    0,                            /* tp_as_buffer      */
    Py_TPFLAGS_DEFAULT,           /* tp_flags          */
    CAssignment_doc,              /* tp_doc            */
    0,                            /* tp_traverse       */
    0,                            /* tp_clear          */
    0,                            /* tp_richcompare    */
    0,                            /* tp_weaklistoffset */
    0,                            /* tp_iter           */
    0,                            /* tp_iternext       */
    0,							  /* tp_methods        */
    0,                            /* tp_members        */
    CAssignment_getset,           /* tp_getset         */
    0,                            /* tp_base           */
    0,                            /* tp_dict           */
    0,                            /* tp_descr_get      */
    0,                            /* tp_descr_set      */
    0,                            /* tp_dictoffset     */
    (initproc)CAssignment_init,   /* tp_init           */
    0,                            /* tp_alloc          */
    0,                            /* tp_new            */
    0,                            /* tp_free           */
    0,                            /* tp_is_gc          */
    0,                            /* tp_bases          */
    0,                            /* tp_mro            */
    0,                            /* tp_cache          */
    0,                            /* tp_subclasses     */
    0,                            /* tp_weaklist       */
    (destructor)CAssignment_del,  /* tp_del            */
};
#pragma endregion

#pragma region Konstruktor

/*****************************************************************************/
// Konstruktor - C++
/*****************************************************************************/
_CAssignment_Object_::_CAssignment_Object_(Symbolics::Graph::Assignment ass): m_Assignment(ass)
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
// Konstruktor - Python
/*****************************************************************************/
static int CAssignment_init(CAssignmentObject *self, PyObject *args, PyObject *kwds)
/*****************************************************************************/
{
    PyErr_SetString(SymbolicsError, "Direct creation of instances of CAssignment is not permitted!");
    return -1;
}
/*****************************************************************************/

/*****************************************************************************/
// Destruktor - Python
/*****************************************************************************/
static void CAssignment_del(CAssignmentObject *self)
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
// Helperfunction
/*****************************************************************************/
static PyObject* returnBasicPtrVec(Symbolics::BasicPtrVec &basicVec)
/*****************************************************************************/
{
	// Wir haben kein Element
	if (basicVec.size() == 0)
		return Py_None;

	// Wir haben genau ein Element
	if (basicVec.size() == 1)
	{
		// Wenn wir bis hier her gekommen sind, dann Symbol als CSymbol zurueckgeben
		CBasicObject* basic = new CBasicObject();
		basic->m_basic = basicVec[0];
		PyObject *obj = PyObject_Init( basic, &CBasicObjectType );
	
		// RefCount erhoehen und zurueckgeben
		Py_IncRef(obj);
		return obj;
	}
	else
	{
	    PyObject* pylist = PyList_New(basicVec.size());

		for (size_t i=0;i<basicVec.size();i++)
		{
			CBasicObject* basic = new CBasicObject();
			basic->m_basic = basicVec[0];
			PyObject *obj = PyObject_Init( basic, &CBasicObjectType );
			PyList_SetItem(pylist,i,obj);
		}
  
		// RefCount erhoehen und zurueckgeben
		Py_IncRef(pylist);
		return pylist;
	}
}
/*****************************************************************************/

/*****************************************************************************/
// LHS
/*****************************************************************************/
static PyObject* CAssignment_getLhs(CAssignmentObject *self, void* data)
/*****************************************************************************/
{
	// Linke Seite auslesen
	BasicPtrVec lhs = self->m_Assignment.lhs;
	return returnBasicPtrVec(lhs);
}
/*****************************************************************************/

/*****************************************************************************/
// RHS
/*****************************************************************************/
static PyObject* CAssignment_getRhs(CAssignmentObject *self, void* data)
/*****************************************************************************/
{
	// Rechte Seite auslesen
	BasicPtrVec rhs = self->m_Assignment.rhs;
	return returnBasicPtrVec(rhs);
}
/*****************************************************************************/



#pragma endregion


