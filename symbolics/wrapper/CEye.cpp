#include "CEye.h"
#include "CBasic.h"
#include "SymbolicsError.h"
#include "convert.h"

using namespace Symbolics::Python;

#pragma region CEye

// Konstruktor
static int CEye_init(CEyeObject *self, PyObject *args, PyObject *kwds);
static void CEye_del(CEyeObject *self);

// Dokumentation
static char CEye_doc[] = 
    "Eye, directly implemented in C++";

// TypeObject
PyTypeObject Symbolics::Python::CEyeObjectType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"symbolics.CEye",			  /* tp_name           */
    sizeof(CEyeObject),			  /* tp_basicsize      */
    0,                            /* tp_itemsize       */
    0,                            /* tp_dealloc        */
    0,                            /* tp_print          */
    0,                            /* tp_getattr        */
    0,                            /* tp_setattr        */
    0,                            /* tp_compare        */
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
    CEye_doc,                     /* tp_doc            */
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
    (initproc)CEye_init,          /* tp_init           */
    0,                            /* tp_alloc          */
    0,                            /* tp_new            */
    0,                            /* tp_free           */
    0,                            /* tp_is_gc          */
    0,                            /* tp_bases          */
    0,                            /* tp_mro            */
    0,                            /* tp_cache          */
    0,                            /* tp_subclasses     */
    0,                            /* tp_weaklist       */
    (destructor)CEye_del,         /* tp_del            */
};
#pragma endregion


#pragma region CEye
// Konstruktor
/*****************************************************************************/
static int CEye_init(CEyeObject *self, PyObject *args, PyObject *kwds)
/*****************************************************************************/
{
    try
    {
        // Args = (shape,)
        PyObject *shapeObj = NULL;
        if (!PyTuple_Check(args))
        {
            PyErr_SetString(SymbolicsError, "args must be a tuple!");
            return -1;
        }
        const size_t nArgs = PyTuple_Size(args);
        if (nArgs > 1)
        {
            PyErr_SetString(SymbolicsError, "len(args) must not exceed one, i.e. CEye(shape)!");
            return -1;
        }
        if (nArgs < 1)
        {
            PyErr_SetString(SymbolicsError, "len(args) must be at least one, i.e. CEye(shape)!");
            return -1;
        }

        // Shape holen
		shapeObj = PyTuple_GetItem(args, 0);
        Shape s = toShape(shapeObj);

		// Shape pruefen
		if ((s.getNrDimensions() != 2) || (s.getDimension(1) != s.getDimension(2)))
		{
			PyErr_SetString(SymbolicsError, "Shape of CEye must be square!");
			return -1;
		}

        // Einheitsmatrix konstruieren
		Symbolics::Matrix *mat = new Symbolics::Matrix( s, true );
		for (size_t i=0; i<s.getDimension(1); ++i)
			mat->set(i,i, Int::getOne());

        self->m_basic = BasicPtr( mat );
    }
	STD_ERROR_HANDLER(-1);

    return 0;
}
/*****************************************************************************/

// Destruktor
/*****************************************************************************/
static void CEye_del(CEyeObject *self)
/*****************************************************************************/
{
    // Referenz loeschen, boost::intrusive_ptr kuemmert sich um den Rest
    self->m_basic = NULL;
}
/*****************************************************************************/

#pragma endregion

