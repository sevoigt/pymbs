#include "CSymbol.h"
#include "CBasic.h"
#include "SymbolicsError.h"
#include "convert.h"

using namespace Symbolics::Python;

#pragma region CSymbol

// Konstruktor
static int CSymbol_init(CSymbolObject *self, PyObject *args, PyObject *kwds);
static void CSymbol_del(CSymbolObject *self);

static long CSymbol_hash(CSymbolObject *self);

// Dokumentation
static char CSymbol_doc[] = 
    "Symbol, directly implemented in C++";

// TypeObject
PyTypeObject Symbolics::Python::CSymbolObjectType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"symbolics.CSymbol",        /* tp_name           */
    sizeof(CSymbolObject),        /* tp_basicsize      */
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
    (hashfunc)CSymbol_hash,       /* tp_hash           */
    0,                            /* tp_call           */
    0,                            /* tp_str            */
    0,                            /* tp_getattro       */
    0,                            /* tp_setattro       */
    0,                            /* tp_as_buffer      */
    Py_TPFLAGS_DEFAULT,            /* tp_flags          */
    CSymbol_doc,                /* tp_doc            */
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
    (initproc)CSymbol_init,        /* tp_init           */
    0,                            /* tp_alloc          */
    0,                            /* tp_new            */
    0,                            /* tp_free           */
    0,                            /* tp_is_gc          */
    0,                            /* tp_bases          */
    0,                            /* tp_mro            */
    0,                            /* tp_cache          */
    0,                            /* tp_subclasses     */
    0,                            /* tp_weaklist       */
    (destructor)CSymbol_del,    /* tp_del            */
};
#pragma endregion

#pragma region CSymbol
// Konstruktor
/*****************************************************************************/
static int CSymbol_init(CSymbolObject *self, PyObject *args, PyObject *kwds)
/*****************************************************************************/
{
    try
    {
        // Args = (name, shape)
        PyObject *nameObj = NULL;
        PyObject *shapeObj = NULL;
        if (!PyTuple_Check(args))
        {
            PyErr_SetString(SymbolicsError, "args must be a tuple!");
            return -1;
        }
         size_t nArgs = PyTuple_Size(args);
        if (nArgs > 2)
        {
            PyErr_SetString(SymbolicsError, "len(args) must not exceed two, i.e. CSymbol(name, shape)!");
            return -1;
        }
        if (nArgs < 1)
        {
            PyErr_SetString(SymbolicsError, "len(args) must be at least one, i.e. CSymbol(name, shape)!");
            return -1;
        }
        nameObj = PyTuple_GetItem(args, 0);
        const char *name = PyUnicode_AsUTF8(nameObj);
        if (nArgs > 1)
            shapeObj = PyTuple_GetItem(args, 1);

        // Shape holen
        Shape s;
        if (shapeObj != NULL)
            s = toShape(shapeObj);

        // Konstruktor aufrufen und damit neues Symbol erstellen
        self->m_basic = SymbolPtr( new Symbolics::Symbol(name, s) );
    }
	STD_ERROR_HANDLER(-1);

    return 0;
}
/*****************************************************************************/

// Destruktor
/*****************************************************************************/
static void CSymbol_del(CSymbolObject *self)
/*****************************************************************************/
{
    // Referenz loeschen, boost::intrusive_ptr kuemmert sich um den Rest
    self->m_basic = NULL;
}
/*****************************************************************************/

#pragma endregion

/*****************************************************************************/
static long CSymbol_hash(CSymbolObject *self)
/*****************************************************************************/
{
	return self->m_basic->getHash();
}
/*****************************************************************************/

