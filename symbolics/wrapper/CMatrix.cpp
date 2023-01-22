#include "CMatrix.h"
#include "CBasic.h"
#include "SymbolicsError.h"
#include "Matrix.h"
#include "convert.h"

using namespace Symbolics::Python;

#pragma region CMatrix

// Konstruktor
static int CMatrix_init(CMatrixObject *self, PyObject *args, PyObject *kwds);
static void CMatrix_del(CMatrixObject *self);


// Dokumentation
static char CMatrix_doc[] = 
    "Matrix, directly implemented in C++";


// TypeObject
PyTypeObject Symbolics::Python::CMatrixObjectType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"symbolics.CMatrix",		  /* tp_name           */
    sizeof(CMatrixObject),        /* tp_basicsize      */
    0,                            /* tp_itemsize       */
    0,                            /* tp_dealloc        */
    0,                            /* tp_print          */
    0,                            /* tp_getattr        */
    0,                            /* tp_setattr        */
    0,                            /* tp_compare        */
    0,                            /* tp_repr           */
    0,                            /* tp_as_number      */
    0,                            /* tp_as_sequence    */
    0,							  /* tp_as_mapping     */
    0,                            /* tp_hash           */
    0,                            /* tp_call           */
    0,                            /* tp_str            */
    0,                            /* tp_getattro       */
    0,                            /* tp_setattro       */
    0,                            /* tp_as_buffer      */
    Py_TPFLAGS_DEFAULT,           /* tp_flags          */
    CMatrix_doc,                  /* tp_doc            */
    0,                            /* tp_traverse       */
    0,                            /* tp_clear          */
    0,                            /* tp_richcompare    */
    0,                            /* tp_weaklistoffset */
    0,                            /* tp_iter           */
    0,                            /* tp_iternext       */
    0,                            /* tp_methods        */
    0,                            /* tp_members        */
    0,                            /* tp_getset         */
    &CBasicObjectType,            /* tp_base           */
    0,                            /* tp_dict           */
    0,                            /* tp_descr_get      */
    0,                            /* tp_descr_set      */
    0,                            /* tp_dictoffset     */
    (initproc)CMatrix_init,       /* tp_init           */
    0,                            /* tp_alloc          */
    0,                            /* tp_new            */
    0,                            /* tp_free           */
    0,                            /* tp_is_gc          */
    0,                            /* tp_bases          */
    0,                            /* tp_mro            */
    0,                            /* tp_cache          */
    0,                            /* tp_subclasses     */
    0,                            /* tp_weaklist       */
    (destructor)CMatrix_del,      /* tp_del            */
};
#pragma endregion

#pragma region CMatrix
/*****************************************************************************/
void getElements( PyObject* arg, int level, int dim[], BasicPtrVec &elements );
/*****************************************************************************/

// Konstruktoren:
// a = CMatrix( tuple ) - erstellt Matrix mit Shape aus tuple
// b = CMatrix( list ) - erstellt Matrix aus Elementen der Liste
/*****************************************************************************/
static int CMatrix_init(CMatrixObject *self, PyObject *args, PyObject *kwds)
/*****************************************************************************/
{
    try
    {
        // Args = (name, shape)
        PyObject *arg = NULL;

		// args sollte immer selbst ein tuple sein
        PyObject *shapeObj = NULL;
        if (!PyTuple_Check(args))
        {
            PyErr_SetString(SymbolicsError, "args must be a tuple!");
            return -1;
        }

		// Anzahl der Argumente in args holen
        const size_t nArgs = PyTuple_Size(args);
        if (nArgs != 1)
        {
            PyErr_SetString(SymbolicsError, "len(args) must equal one, i.e. CMatrix( elements ) or CMatrix( shape )!");
            return -1;
        }
		// arg holen
        arg = PyTuple_GetItem(args, 0);

		// Was haben wir?

		// Es ist ein tuple -> also leere Matrix mit Shape erzeugen
		if (PyTuple_Check(arg))
		{
			// Shape holen
			Shape s = toShape(arg);

			// Konstruktor aufrufen und damit neues Matrix erstellen
			self->m_basic = BasicPtr( new Symbolics::Matrix( s ) );
			
			// fertig
			return 0;		
		}

		// Es ist eine Liste => Liste bzw. Liste mit Liste von Elementen
		if (PyList_Check(arg))
		{
			// Vektor fuer die Matrix Elemente anlegen
			BasicPtrVec elements;
			int dim[2] = {0,0};

			// Elemente aus Liste extrahieren
			getElements(arg, 0, dim, elements);

			// Aufruf Konstruktor vorbereiten
			Shape s;
			// Vektor
			if (dim[1] == 0)
				s = Shape(dim[0]);
			else
				s = Shape(dim[0], dim[1]);

			// Konstruktor aufrufen
			self->m_basic = BasicPtr( new Symbolics::Matrix( elements, s ) );

			// fertig
			return 0;
		}

		// Sonst haben wir einen unbekannten Typen
		PyErr_SetString(SymbolicsError, "argument has got unexpected type, use CMatrix( elements (type: list) ) or CMatrix( shape (type: tuple) )!");
        return -1;
    }
	STD_ERROR_HANDLER(-1);

    return 0;
}
/*****************************************************************************/


/*****************************************************************************/
void getElements( PyObject* arg, int level, int dim[], BasicPtrVec &elements )
/*****************************************************************************/
{
	// Typ von arg pruefen
	if (!PyList_Check(arg))
		throw InternalError("Argument was supposed to be a list!");

	// Laenge der Liste holen
	const size_t n = PyList_Size(arg);
	// Laenge pruefen
	if (dim[level] == 0)
		dim[level] = n;
	else
		if (dim[level] != n)
			throw InternalError("Mismatch of list length!");

	// Alle Listenelemente durchlaufen
	bool hasList = true;
	for( size_t i=0; i<n; ++i)
	{
		// Element aus Liste holen
		PyObject *item = PyList_GetItem(arg, i);

		// Was haben wir da
		// Ist es eine Liste, dann verzweigen
		if (PyList_Check(item))
		{
			// Mix von List und Skalar?
			if (hasList == false) throw InternalError("Elements in list must either be lists or scalar!");
			// Tiefenpruefung
			if (level >= 1) throw InternalError("Only two dimensional matrices are supported!");
			// Eintauchen
			getElements( item, level+1, dim, elements);
			continue;
		}
		else hasList = false;

		// Mix von List und Skalar?
		if (hasList == true) throw InternalError("Elements in list must either be lists or scalar!");

		// Wir haben einen Skalar
        BasicPtr a( getBasic(item) );
		if (a.get() == NULL) throw InternalError("items in list must be in (int, float, CBasic)!");

		// Der Liste hinzufuegen
		elements.push_back(a);
	}
}
/*****************************************************************************/


// Destruktor
/*****************************************************************************/
static void CMatrix_del(CMatrixObject *self)
/*****************************************************************************/
{
    // Referenz loeschen, boost::intrusive_ptr kuemmert sich um den Rest
    self->m_basic = NULL;
}
/*****************************************************************************/

#pragma endregion
