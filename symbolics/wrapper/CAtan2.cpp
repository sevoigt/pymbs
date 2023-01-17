#include "CAtan2.h"
#include "CBasic.h"
#include "SymbolicsError.h"
#include "convert.h"

using namespace Symbolics::Python;

#pragma region CAtan2

// Konstruktor
static int CAtan2_init(CAtan2Object *self, PyObject *args, PyObject *kwds);
static void CAtan2_del(CAtan2Object *self);

// Dokumentation
static char CAtan2_doc[] = 
    "Function atan2, directly implemented in C++";

// TypeObject
PyTypeObject Symbolics::Python::CAtan2ObjectType = {
	PyVarObject_HEAD_INIT(NULL, 0)
    "symbolics.atan2",        /* tp_name           */
    sizeof(CAtan2Object),        /* tp_basicsize      */
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
    CAtan2_doc,                /* tp_doc            */
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
    (initproc)CAtan2_init,        /* tp_init           */
    0,                            /* tp_alloc          */
    0,                            /* tp_new            */
    0,                            /* tp_free           */
    0,                            /* tp_is_gc          */
    0,                            /* tp_bases          */
    0,                            /* tp_mro            */
    0,                            /* tp_cache          */
    0,                            /* tp_subclasses     */
    0,                            /* tp_weaklist       */
    (destructor)CAtan2_del,    /* tp_del            */
};
#pragma endregion

#pragma region CAtan2
// Konstruktor
/*****************************************************************************/
static int CAtan2_init(CAtan2Object *self, PyObject *args, PyObject *kwds)
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
            PyErr_SetString(SymbolicsError, "len(args) must not exceed two, i.e. atan2(x,y)!");
            return -1;
        }
        if (nArgs < 2)
        {
            PyErr_SetString(SymbolicsError, "len(args) must be at least two, i.e. atan2(x,y)!");
            return -1;
        }
       // Expression extrahieren
        PyObject *ox;
        PyObject *oy;
        // Argumente parsen
        if (!PyArg_ParseTuple(args, "OO", &ox,&oy))
            return 0;
        BasicPtr x( getBasic(ox) );
        BasicPtr y( getBasic(oy) );
        // Konstruktor aufrufen und damit neues Atan2 erstellen
        self->m_basic = BasicPtr( new Symbolics::Atan2(x,y) );
    }
	STD_ERROR_HANDLER(-1);

    return 0;
}
/*****************************************************************************/

// Destruktor
/*****************************************************************************/
static void CAtan2_del(CAtan2Object *self)
/*****************************************************************************/
{
    // Referenz loeschen, boost::intrusive_ptr kuemmert sich um den Rest
    self->m_basic = NULL;
}
/*****************************************************************************/

#pragma endregion

