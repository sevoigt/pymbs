#include "CDer.h"
#include "CBasic.h"
#include "SymbolicsError.h"
#include "convert.h"

using namespace Symbolics::Python;

#pragma region CDer

// Konstruktor
static int CDer_init(CDerObject *self, PyObject *args, PyObject *kwds);
static void CDer_del(CDerObject *self);

// Dokumentation
static char CDer_doc[] = 
    "Function der, directly implemented in C++";

// TypeObject
PyTypeObject Symbolics::Python::CDerObjectType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"symbolics.der",        /* tp_name           */
    sizeof(CDerObject),        /* tp_basicsize      */
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
    Py_TPFLAGS_DEFAULT,            /* tp_flags          */
    CDer_doc,                /* tp_doc            */
    0,                            /* tp_traverse       */
    0,                            /* tp_clear          */
    0,                            /* tp_richcompare    */
    0,                            /* tp_weaklistoffset */
    0,                            /* tp_iter           */
    0,                            /* tp_iternext       */
    0,                             /* tp_methods        */
    0,                            /* tp_members        */
    0,                            /* tp_getset         */
    &CBasicObjectType,            /* tp_base           */
    0,                            /* tp_dict           */
    0,                            /* tp_descr_get      */
    0,                            /* tp_descr_set      */
    0,                            /* tp_dictoffset     */
    (initproc)CDer_init,        /* tp_init           */
    0,                            /* tp_alloc          */
    0,                            /* tp_new            */
    0,                            /* tp_free           */
    0,                            /* tp_is_gc          */
    0,                            /* tp_bases          */
    0,                            /* tp_mro            */
    0,                            /* tp_cache          */
    0,                            /* tp_subclasses     */
    0,                            /* tp_weaklist       */
    (destructor)CDer_del,    /* tp_del            */
};
#pragma endregion

#pragma region CDer
// Konstruktor
/*****************************************************************************/
static int CDer_init(CDerObject *self, PyObject *args, PyObject *kwds)
/*****************************************************************************/
{
    try
    {
        // Args = (exp)
        if (!PyTuple_Check(args))
        {
            PyErr_SetString(SymbolicsError, "args must be a tuple!");
            return -1;
        }
         size_t nArgs = PyTuple_Size(args);
        if (nArgs > 2)
        {
            PyErr_SetString(SymbolicsError, "len(args) must not exceed one, i.e. der(expression) or der(expression,symbol)!");
            return -1;
        }
        if (nArgs < 1)
        {
            PyErr_SetString(SymbolicsError, "len(args) must be at least one, i.e. der(expression) or der(expression,symbol)!");
            return -1;
        }
        switch (nArgs)
        {
        case 1:
            {
               // Expression extrahieren
                PyObject *o;
                // Argumente parsen
                if (!PyArg_ParseTuple(args, "O", &o))
                    return 0;
                BasicPtr exp( getBasic(o) );
                // Konstruktor aufrufen und damit neues Der erstellen
                self->m_basic = exp->der();
                break;
            }
        case 2:
            {
               // Expression extrahieren
                PyObject *o;
                PyObject *s;
                // Argumente parsen
                if (!PyArg_ParseTuple(args, "OO", &o,&s))
                    return 0;
                BasicPtr exp( getBasic(o) );
                BasicPtr sym( getBasic(s) );
                // Konstruktor aufrufen und damit neues Der erstellen
                self->m_basic = exp->der(sym);
                break;
            }
        }
    }
	STD_ERROR_HANDLER(-1);

    return 0;
}
/*****************************************************************************/

// Destruktor
/*****************************************************************************/
static void CDer_del(CDerObject *self)
/*****************************************************************************/
{
    // Referenz loeschen, boost::intrusive_ptr kuemmert sich um den Rest
    self->m_basic = NULL;
}
/*****************************************************************************/

#pragma endregion

