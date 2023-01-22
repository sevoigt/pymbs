#include "CIf.h"
#include "CBasic.h"
#include "SymbolicsError.h"
#include "convert.h"

using namespace Symbolics::Python;

#pragma region CIf

// Konstruktor
static int CIf_init(CIfObject *self, PyObject *args, PyObject *kwds);
static void CIf_del(CIfObject *self);

// Dokumentation
static char CIf_doc[] = 
    "Function If(cond,then,else), directly implemented in C++";

// TypeObject
PyTypeObject Symbolics::Python::CIfObjectType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"symbolics.If",        /* tp_name           */
    sizeof(CIfObject),        /* tp_basicsize      */
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
    CIf_doc,                /* tp_doc            */
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
    (initproc)CIf_init,        /* tp_init           */
    0,                            /* tp_alloc          */
    0,                            /* tp_new            */
    0,                            /* tp_free           */
    0,                            /* tp_is_gc          */
    0,                            /* tp_bases          */
    0,                            /* tp_mro            */
    0,                            /* tp_cache          */
    0,                            /* tp_subclasses     */
    0,                            /* tp_weaklist       */
    (destructor)CIf_del,    /* tp_del            */
};
#pragma endregion

#pragma region CIf
// Konstruktor
/*****************************************************************************/
static int CIf_init(CIfObject *self, PyObject *args, PyObject *kwds)
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
        if (nArgs > 3)
        {
            PyErr_SetString(SymbolicsError, "len(args) must not exceed three, i.e. If(condition,then,else)!");
            return -1;
        }
        if (nArgs < 3)
        {
            PyErr_SetString(SymbolicsError, "len(args) must be at least three, i.e. If(condition,then,else)!");
            return -1;
        }
       // Expression extrahieren
        PyObject *ocond;
        PyObject *othen;
        PyObject *oelse;
        // Argumente parsen
        if (!PyArg_ParseTuple(args, "OOO", &ocond,&othen,&oelse))
            return 0;
        BasicPtr cond( getBasic(ocond) );
        BasicPtr ethen( getBasic(othen) );
        BasicPtr eelse( getBasic(oelse) );
        // Konstruktor aufrufen und damit neues If erstellen
        self->m_basic = Symbolics::If::New(cond,ethen,eelse);
    }
	STD_ERROR_HANDLER(-1);

    return 0;
}
/*****************************************************************************/

// Destruktor
/*****************************************************************************/
static void CIf_del(CIfObject *self)
/*****************************************************************************/
{
    // Referenz loeschen, boost::intrusive_ptr kuemmert sich um den Rest
    self->m_basic = NULL;
}
/*****************************************************************************/

#pragma endregion

