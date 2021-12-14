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

#include "PySymbolics.h"
#include "SymbolicsError.h"
#include "CBasic.h"
#include "CSymbol.h"
#include "CMatrix.h"
#include "CNumber.h"
#include "CGraph.h"
#include "CAcos.h"
#include "CAsin.h"
#include "CAtan.h"
#include "CAtan2.h"
#include "CCos.h"
#include "CDer.h"
#include "CElement.h"
#include "CScalar.h"
#include "CSin.h"
#include "CSkew.h"
#include "CSolve.h"
#include "CTan.h"
#include "CTranspose.h"
#include "CZeros.h"
#include "CEye.h"
#include "CIf.h"
#include "CSign.h"
#include "COuter.h"
#include "CJacobian.h"
#include "CInverse.h"
#include "CLess.h"
#include "CGreater.h"
#include "CEqual.h"
#include "CAssignment.h"
#include <Python.h>

using namespace Symbolics::Python;

/*****************************************************************************/
bool registerObject( PyTypeObject *typeObj,  std::string const& name, PyObject *module )
/*****************************************************************************/
{
    // Typen vervollstaendigen
    typeObj->tp_new = PyType_GenericNew;
    if (PyType_Ready(typeObj) < 0)
        return false;
    
    // Typen registrieren
    Py_INCREF(typeObj);
    PyModule_AddObject(module, name.c_str(), reinterpret_cast<PyObject*>(typeObj));

    // Alles ok
    return true;
}
/*****************************************************************************/


#pragma region Symbolics_Module
/*****************************************************************************/
PyMODINIT_FUNC PyInit_symbolics(void)
/*****************************************************************************/
{
    PyObject *m;

    // Modul registrieren
	static struct PyModuleDef symbolics_module_definition = {
		PyModuleDef_HEAD_INIT,
		"symbolics",
		"Symbolic math toolbox in C.",
		-1,
		NULL
	};

	m = PyModule_Create(&symbolics_module_definition);
    if (m == NULL)
        return NULL;

    // Objekte registrieren
    // CBasic
    if (!registerObject( &CBasicObjectType, "CBasic", m))
        return NULL;
    // CSymbol
    if (!registerObject( &CSymbolObjectType, "CSymbol", m))
        return NULL;
    // CMatrix
    if (!registerObject( &CMatrixObjectType, "CMatrix", m))
        return NULL;
    // CNumber
    if (!registerObject( &CNumberObjectType, "CNumber", m))
        return NULL;
    // CZeros
    if (!registerObject( &CZerosObjectType, "CZeros", m))
        return NULL;
    // CEye
    if (!registerObject( &CEyeObjectType, "CEye", m))
        return NULL;
    // CAcos
    if (!registerObject( &CAcosObjectType, "acos", m))
        return NULL;
    // CAsin
    if (!registerObject( &CAsinObjectType, "asin", m))
        return NULL;
    // CAtan
    if (!registerObject( &CAtanObjectType, "atan", m))
        return NULL;
    // CAtan2
    if (!registerObject( &CAtan2ObjectType, "atan2", m))
        return NULL;
    // CCos
    if (!registerObject( &CCosObjectType, "cos", m))
        return NULL;
    // CDer
    if (!registerObject( &CDerObjectType, "der", m))
        return NULL;
    // CElement
    if (!registerObject( &CElementObjectType, "element", m))
        return NULL;
    // CScalar
    if (!registerObject( &CScalarObjectType, "scalar", m))
        return NULL;
    // CSin
    if (!registerObject( &CSinObjectType, "sin", m))
        return NULL;
    // CSkew
    if (!registerObject( &CSkewObjectType, "skew", m))
        return NULL;
    // CSolve
    if (!registerObject( &CSolveObjectType, "solve", m))
        return NULL;
    // CTan
    if (!registerObject( &CTanObjectType, "tan", m))
        return NULL;
    // CTranspose
    if (!registerObject( &CTransposeObjectType, "transpose", m))
        return NULL;
    // COuter
    if (!registerObject( &COuterObjectType, "outer", m))
        return NULL;
    // CIf
    if (!registerObject( &CIfObjectType, "If", m))
        return NULL;
    // CSign
    if (!registerObject( &CSignObjectType, "sign", m))
        return NULL;
    // CJacobian
    if (!registerObject( &CJacobianObjectType, "jacobian", m))
        return NULL;
    // CInverse
    if (!registerObject( &CInverseObjectType, "inv", m))
        return NULL;
    // CLess
    if (!registerObject( &CLessObjectType, "Less", m))
        return NULL;
    // CGreater
    if (!registerObject( &CGreaterObjectType, "Greater", m))
        return NULL;
    // CEqual
    if (!registerObject( &CLessObjectType, "Equal", m))
        return NULL;
    // CGraph
    if (!registerObject( &CGraphObjectType, "CGraph", m))
        return NULL;
    // CAssignment
    if (!registerObject( &CAssignmentObjectType, "CAssignment", m))
        return NULL;


    // Error(s) registrieren
    SymbolicsError = PyErr_NewException((char*)"symbolics.Error", NULL, NULL);
    Py_INCREF(SymbolicsError);
    PyModule_AddObject(m, "Error", SymbolicsError);

	return m;
}
/*****************************************************************************/
#pragma endregion
