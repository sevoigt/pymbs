#include "CBasic.h"
#include "PythonPrinter.h"
#include "SymbolicsError.h"
#include "convert.h"
#include "PyFactory.h"

using namespace Symbolics::Python;

namespace Symbolics
{
	namespace Python
	{
		extern PyTypeObject CMatrixObjectType;
	};
};

#pragma region CBasic

// Konstruktor
static int CBasic_init(CBasicObject *self, PyObject *args, PyObject *kwds);
static void CBasic_del(CBasicObject *self);

// Number Funktionen
static PyObject* CBasic_str(CBasicObject *self);
static PyObject* CBasic_add(PyObject *lhs, PyObject *rhs);
static PyObject* CBasic_sub(PyObject *lhs, PyObject *rhs);
static PyObject* CBasic_mul(PyObject *lhs, PyObject *rhs);
static PyObject* CBasic_div(PyObject *lhs, PyObject *rhs);
static PyObject* CBasic_pow(PyObject *self, PyObject *other, PyObject *modulo);
static PyObject* CBasic_abs(PyObject *arg);
static PyObject* CBasic_neg(PyObject *arg);

// Mapping Funktionen
static Py_ssize_t CBasic_len(CBasicObject *self);
static PyObject* CBasic_get(CBasicObject *self, PyObject *index);
static int CBasic_set(CBasicObject *self, PyObject *index, PyObject *value);

// Funktionen
static PyObject* CBasic_shape(CBasicObject *self, PyObject *args);
static PyObject* CBasic_simplify(CBasicObject *self, PyObject *args);
static PyObject* CBasic_atoms(CBasicObject *self, PyObject *args);
static PyObject* CBasic_norm(CBasicObject *self, PyObject *args);

static PyObject* CBasic_cmp(CBasicObject *self, PyObject *other, int op);

// Number Methods
static PyNumberMethods CBasic_NumberMethods = {
    CBasic_add,                   /* nb_add */
    CBasic_sub,                   /* nb_subtract */
    CBasic_mul,                   /* nb_multiply */
    0,                            /* nb_remainder */
    0,                            /* nb_divmod */
    CBasic_pow,                   /* nb_power */
    CBasic_neg,                   /* nb_negative */
    0,                            /* nb_positive */
    CBasic_abs,                   /* nb_absolute */
    0,                            /* nb_bool */
    0,                            /* nb_invert */
    0,                            /* nb_lshift */
    0,                            /* nb_rshift */
    0,                            /* nb_and */
    0,                            /* nb_xor */
    0,                            /* nb_or */
    0,                            /* nb_int */
    0,                            /* nb_reserved*/
    0,                            /* nb_float */

    /* Added in release 2.0 */
    0,                            /* nb_inplace_add */
    0,                            /* nb_inplace_subtract */
    0,                            /* nb_inplace_multiply */
    0,                            /* nb_inplace_remainder */
    0,                            /* nb_inplace_power */
    0,                            /* nb_inplace_lshift */
    0,                            /* nb_inplace_rshift */
    0,                            /* nb_inplace_and */
    0,                            /* nb_inplace_xor */
    0,                            /* nb_inplace_or */

    /* Added in release 2.2 */
    /* The following require the Py_TPFLAGS_HAVE_CLASS flag */
    0,                            /* nb_floor_divide */
    CBasic_div,                   /* nb_true_divide */
    0,                            /* nb_inplace_floor_divide */
    0,                            /* nb_inplace_true_divide */

    /* Added in release 2.5 */
    0,                            /* nb_index */

	0,							  /* nb_matrix_multiply*/
	0,							  /* nb_inplace_matrix_multiply*/
};


// MappingMethods
static PyMappingMethods CBasic_MappingMethods = {
    (lenfunc)CBasic_len, /* lenfunc mp_length */
    (binaryfunc)CBasic_get, /* binaryfunc mp_subscript */
    (objobjargproc)CBasic_set, /* objobjargproc mp_ass_subscript */
};


// Tabelle mit allen Funktionen
static PyMethodDef CBasic_methods[] = {
    {"shape",                (PyCFunction)CBasic_shape,                METH_NOARGS, "Return the shape of this expression"},
    {"simplify",             (PyCFunction)CBasic_simplify,             METH_NOARGS, "try to simplify this expression"},
    {"atoms",             (PyCFunction)CBasic_atoms,             METH_NOARGS, "returns all atoms of this expression"},
    {"norm",             (PyCFunction)CBasic_norm,             METH_NOARGS, "returns the norm of a vector"},
    {NULL}
};

// Dokumentation
static char CBasic_doc[] = 
    "CBasic: Base Class for All Symbols and Functions";

// TypeObject
PyTypeObject Symbolics::Python::CBasicObjectType = {
	PyVarObject_HEAD_INIT(NULL, 0)
    "symbolics.CBasic",           /* tp_name           */
    sizeof(CBasicObject),         /* tp_basicsize      */
    0,                            /* tp_itemsize       */
    0,                            /* tp_dealloc        */
    0,                            /* tp_print          */
    0,                            /* tp_getattr        */
    0,                            /* tp_setattr        */
    0,					          /* tp_as_async        */    
	(reprfunc)CBasic_str,         /* tp_repr           */
    &CBasic_NumberMethods,        /* tp_as_number      */
    0,                            /* tp_as_sequence    */
    &CBasic_MappingMethods,       /* tp_as_mapping     */
    0,                            /* tp_hash           */
    0,                            /* tp_call           */
    (reprfunc)CBasic_str,         /* tp_str            */
    0,                            /* tp_getattro       */
    0,                            /* tp_setattro       */
    0,                            /* tp_as_buffer      */
	Py_TPFLAGS_DEFAULT,	          /* tp_flags          */
    CBasic_doc,                   /* tp_doc            */
    0,                            /* tp_traverse       */
    0,                            /* tp_clear          */
    (richcmpfunc)CBasic_cmp,      /* tp_richcompare    */
    0,                            /* tp_weaklistoffset */
    0,                            /* tp_iter           */
    0,                            /* tp_iternext       */
    CBasic_methods,               /* tp_methods        */
    0,                            /* tp_members        */
    0,                            /* tp_getset         */
    0,                            /* tp_base           */
    0,                            /* tp_dict           */
    0,                            /* tp_descr_get      */
    0,                            /* tp_descr_set      */
    0,                            /* tp_dictoffset     */
    (initproc)CBasic_init,        /* tp_init           */
    0,                            /* tp_alloc          */
    0,                            /* tp_new            */
    0,                            /* tp_free           */
    0,                            /* tp_is_gc          */
    0,                            /* tp_bases          */
    0,                            /* tp_mro            */
    0,                            /* tp_cache          */
    0,                            /* tp_subclasses     */
    0,                            /* tp_weaklist       */
    (destructor)CBasic_del,       /* tp_del            */
};
#pragma endregion

#pragma region Konstruktor
/*****************************************************************************/
// Konstruktor - C++
/*****************************************************************************/
Symbolics::Python::_CBasic_Object_::_CBasic_Object_()
/*****************************************************************************/
{
	// PyObject_Init(this, &CBasicObjectType);
}
/*****************************************************************************/


/*****************************************************************************/
// Konstruktor - Python
/*****************************************************************************/
static int CBasic_init(CBasicObject *self, PyObject *args, PyObject *kwds)
/*****************************************************************************/
{
    PyErr_SetString(SymbolicsError, "Direct creation of instances of CBasic is not permitted!");
    return -1;
}
/*****************************************************************************/

/*****************************************************************************/
// Destruktor - Python
/*****************************************************************************/
static void CBasic_del(CBasicObject *self)
/*****************************************************************************/
{
    // m_basic freigeben, intrusive_ptr kuemmert sich um den Rest
    self->m_basic = NULL;
}
/*****************************************************************************/
#pragma endregion


#pragma region Buchhaltung
// Funktionen
/*****************************************************************************/
static PyObject* CBasic_str(CBasicObject *self)
/*****************************************************************************/
{
    try
    {
        // Printer aufrufen
        Symbolics::PythonPrinter p; //PythonPrinter ist hier bestimmt am sinnvollsten...
        const std::string s = p.print( self->m_basic );
        return Py_BuildValue("s", s.c_str());
    }
	STD_ERROR_HANDLER(NULL);
}
/*****************************************************************************/
#pragma endregion


#pragma region Operatoren
// Andere Funktionen
/*****************************************************************************/
template< typename T >
inline PyObject* Unary(PyObject* arg)
/*****************************************************************************/
{
    try
    {
        // Type pruefen
        BasicPtr a( getBasic(arg) );
        if (a.get() == NULL) 
        {
            PyErr_SetString(SymbolicsError, "argument must be a CBasic Type!");
            return NULL;
        }

        // neues T anlegen
        BasicPtr op( T::New( a ) );
        
        // RefCount erhoehen und zurueckgeben
		PyObject *obj = PyFactory::New( op );
		Py_IncRef(obj);
        return obj;
    }
	STD_ERROR_HANDLER(NULL);
}
/*****************************************************************************/


/*****************************************************************************/
template< typename T >
inline PyObject* Binary(PyObject* lhs, PyObject *rhs)
/*****************************************************************************/
{
    try
    {
        // ConstBasics extrahieren
        BasicPtr left( getBasic(lhs) );
        if (left.get() == NULL) 
        {
            PyErr_SetString(SymbolicsError, "lhs argument must be a CBasic Type!");
            return NULL;
        }
        BasicPtr right( getBasic(rhs) );
        if (right.get() == NULL) 
        {
            PyErr_SetString(SymbolicsError, "rhs argument must be a CBasic Type!");
            return NULL;
        }

        // neues T anlegen
		BasicPtr op( T::New( left, right ) );		
        
        // RefCount erhoehen und zurueckgeben
		PyObject *obj = PyFactory::New( op );
		Py_IncRef(obj);
        return obj;
    }
	STD_ERROR_HANDLER(NULL);
}
/*****************************************************************************/


// Andere Funktionen
/*****************************************************************************/
static PyObject* CBasic_add(PyObject *lhs, PyObject *rhs)
/*****************************************************************************/
{
    return Binary<Add>(lhs, rhs);
}
/*****************************************************************************/


/*****************************************************************************/
static PyObject* CBasic_sub(PyObject *lhs, PyObject *rhs)
/*****************************************************************************/
{
    try
    {
        // ConstBasics extrahieren
        BasicPtr left( getBasic(lhs) );
        if (left.get() == NULL) 
        {
            PyErr_SetString(SymbolicsError, "lhs argument must be a CBasic Type!");
            return NULL;
        }
        BasicPtr right( getBasic(rhs) );
        if (right.get() == NULL) 
        {
            PyErr_SetString(SymbolicsError, "rhs argument must be a CBasic Type!");
            return NULL;
        }

        // neues Sub und Neg anlegen
        BasicPtr negRight = Neg::New(right);
        BasicPtr sub = Add::New( left, negRight ) ;
        
        // RefCount erhoehen und zurueckgeben
		PyObject *obj = PyFactory::New( sub );
		Py_IncRef(obj);
        return obj;
    }
	STD_ERROR_HANDLER(NULL);
}
/*****************************************************************************/


/*****************************************************************************/
static PyObject* CBasic_mul(PyObject *lhs, PyObject *rhs)
/*****************************************************************************/
{
    return Binary<Mul>(lhs, rhs);
}
/*****************************************************************************/


/*****************************************************************************/
static PyObject* CBasic_div(PyObject *lhs, PyObject *rhs)
/*****************************************************************************/
{
    try
    {
        // ConstBasics extrahieren
        BasicPtr left( getBasic(lhs) );
        if (left.get() == NULL) 
        {
            PyErr_SetString(SymbolicsError, "lhs argument must be a CBasic Type!");
            return NULL;
        }
        BasicPtr right( getBasic(rhs) );
        if (right.get() == NULL) 
        {
            PyErr_SetString(SymbolicsError, "rhs argument must be a CBasic Type!");
            return NULL;
        }

        // neues Mul und Pow anlegen
        BasicPtr div = Util::div(left,right);
        
        // RefCount erhoehen und zurueckgeben
		PyObject *obj = PyFactory::New( div );
		Py_IncRef(obj);
        return obj;
    }
	STD_ERROR_HANDLER(NULL);
}
/*****************************************************************************/


/*****************************************************************************/
static PyObject* CBasic_pow(PyObject *self, PyObject *other, PyObject *modulo)
/*****************************************************************************/
{
    try
    {
        // Drittes Argument darf nicht existieren
        if (modulo != Py_None)
        {
            PyErr_SetString(SymbolicsError, "Pow is not supported for modulo!");
            return NULL;
        }

        // ConstBasic extrahieren
        BasicPtr left( getBasic(self) );
        if (left.get() == NULL) 
        {
            PyErr_SetString(SymbolicsError, "lhs argument must be a CBasic Type!");
            return NULL;
        }
        // Pow muss ein int oder float haben
        double exponent = 0.0;
        if (PyFloat_Check(other))
            exponent = PyFloat_AsDouble(other);
        else if (PyLong_Check(other))
            exponent = PyLong_AsLong(other);
        else
        {
            PyErr_SetString(SymbolicsError, "Exponent must be a number!");
            return NULL;
        }

        // neues Pow anlegen
        BasicPtr pow = Pow::New( left, Real::New(exponent) );
        
        // RefCount erhoehen und zurueckgeben
		PyObject *obj = PyFactory::New( pow );
		Py_IncRef(obj);
        return obj;
    }
	STD_ERROR_HANDLER(NULL);
}
/*****************************************************************************/


/*****************************************************************************/
static PyObject* CBasic_abs(PyObject *arg)
/*****************************************************************************/
{
    return Unary<Abs>(arg);
}
/*****************************************************************************/

/*****************************************************************************/
static PyObject* CBasic_neg(PyObject *arg)
/*****************************************************************************/
{
    return Unary<Neg>(arg);
}
/*****************************************************************************/
#pragma endregion


#pragma region Funktionen
/*****************************************************************************/
static PyObject* CBasic_shape(CBasicObject *self, PyObject *args)
/*****************************************************************************/
{
    try
    {
        // Shape holen
        const Shape s = self->m_basic->getShape();
        PyObject *shapeObj = fromShape(s);        
        // RefCount erhoehen und zurueckgeben
        return shapeObj;
    }
	STD_ERROR_HANDLER(NULL);
}
/*****************************************************************************/


/*****************************************************************************/
static PyObject* CBasic_simplify(CBasicObject *self, PyObject *args)
/*****************************************************************************/
{
    try
    {
		// Simplify ausfuehren
		self->m_basic = self->m_basic->simplify();

    self->ob_type = PyFactory::ObjectType(self->m_basic);
    
		Py_IncRef(Py_None);
		return Py_None;
    }
	STD_ERROR_HANDLER(NULL);
}
/*****************************************************************************/
#pragma endregion


// Mapping Funktionen
/*****************************************************************************/
static Py_ssize_t CBasic_len(CBasicObject *self)
/*****************************************************************************/
{
	// Shape zurueckgeben
	Shape const& s = self->m_basic->getShape();
	return s.getNumEl();
}
/*****************************************************************************/

// Get Index
/*****************************************************************************/
bool getIndex( PyObject *index, Shape const& s, std::vector<int> &m, std::vector<int> &n )
/*****************************************************************************/
{
	// Beruecksichtigt:
	// - int
	// - Slice
	// - (int)
	// - (Slice)
	// - (int, int)
	// - (Slice, int)
	// - (int, Slice)
	// - (Slice, Slice)

	// Vektoren leeren
	m.clear(); n.clear();

	// Index = Int
	if (PyLong_Check(index))
		m.push_back(PyLong_AsSsize_t(index));
	else if (PySlice_Check(index))
	{
		// Slice "auspacken"
		Py_ssize_t start, stop, step;
		PySlice_GetIndices(index, s.getDimension(1)*s.getDimension(2), &start, &stop, &step);
		if (start > stop)
      throw ShapeError("Slice seems strange start>stop?!");
		for(int i=(int)start; i<(int)stop; i+=(int)step)
			m.push_back(i);
	}
	else if (PyTuple_Check(index))
	{
		// Index = Tuple
		const size_t len = PyTuple_Size(index);
		if (len == 0)
		{
			PyErr_SetString(SymbolicsError, "Got empty tuple of ints!");
			return false;
		}
		if (len > 2)
		{
			PyErr_SetString(SymbolicsError, "Index tuple has more than two elements!");
			return false;
		}
		// Ersten Int holen
		PyObject *row = PyTuple_GetItem(index, 0);
		if (PyLong_Check(row))
			m.push_back(PyLong_AsSsize_t(row));
		else if (PySlice_Check(row))
		{
			// Slice "auspacken"
			Py_ssize_t start, stop, step;
			if (len == 1)
				PySlice_GetIndices(row, s.getDimension(1)*s.getDimension(2), &start, &stop, &step);
			else
				PySlice_GetIndices(row, s.getDimension(1), &start, &stop, &step);
			if (start > stop) 
        throw ShapeError("Slice seems strange start>stop?!");
			for(int i=(int)start; i<(int)stop; i+=(int)step)
				m.push_back(i);
		}
		else
		{
			PyErr_SetString(SymbolicsError, "First element in index tuple is no int!");
			return false;
		}

		// Zweiten Int holen
		if (len == 2)
		{
			// Zweiten Int holen
			PyObject *col = PyTuple_GetItem(index, 1);
			if (PyLong_Check(col))
				n.push_back(PyLong_AsSsize_t(col));
			else if (PySlice_Check(col))
			{
				// Slice "auspacken"
				Py_ssize_t start, stop, step;
				PySlice_GetIndices(col, s.getDimension(2), &start, &stop, &step);
				if (start > stop) 
          throw ShapeError("Slice seems strange start>stop?!");
				for(int i=(int)start; i<(int)stop; i+=(int)step)
					n.push_back(i);
			}
			else	
			{
				PyErr_SetString(SymbolicsError, "Second element in index tuple is no int!");
				return false;
			}
		}
	}
	else
	{
		PyErr_SetString(SymbolicsError, "Index must be either int or a tuple of ints!");
		return false;
	}

	// Shape vergleichen
	if (m.size() < 1)
	{
		PyErr_SetString(SymbolicsError, "No first index specified!");
		return false;
	}
	if (n.size() == 0)	// Als Vektor
	{
		// Letztes, und damit groesstes m vergleichen
		if (m[m.size()-1] >= static_cast<int>(s.getDimension(1)*s.getDimension(2)))
		{
			PyErr_SetString(SymbolicsError, "First index out of bounds!");
			return false;
		}
	}
	else		// Als Matrix
	{
		// Letztes, und damit groesstes m vergleichen
		if (m[m.size()-1] >= static_cast<int>(s.getDimension(1)))
		{
			PyErr_SetString(SymbolicsError, "First index out of bounds!");
			return false;
		}
	}

	if (n.size() > 0)
		if (n[n.size()-1] >= static_cast<int>(s.getDimension(2)))
		{
			std::string msg = "Second index out of bounds (" + str(n[n.size()-1]) + ">=" + str(s.getDimension(2)) + ")!";
			PyErr_SetString(SymbolicsError, msg.c_str());
			return false;
		}

	// Alles ok
	return true;
}
/*****************************************************************************/


/*****************************************************************************/
inline bool slicing(std::vector<int> const &m, std::vector<int> const &n)
/*****************************************************************************/
{
	return ((m.size() != 1) || (n.size() > 1));
}
/*****************************************************************************/

/*****************************************************************************/
Shape getShape(std::vector<int> const &m, std::vector<int> const &n)
/*****************************************************************************/
{
	const int rows = m.size();
	const int cols = n.size();
	
	// r | c | Result
	// --------------
	// 1 | 0 | Scalar
	// + | 0 | Vector
	// 1 | 1 | Scalar
	// + | 1 | Vector
	// 1 | + | Vector
	// + | + | Matrix

	//if (m.size() == 1) // 1 Zeile
	//{
	//	if (n.size() <= 1)	// 0-1 Spalte => Skalar
	//		s = Shape();
	//	else				// mehrere Spalten => Vektor
	//		s = Shape(1,1,n.size());
	//}
	//else	// mehrere Zeilen
	//{
	//	if (n.size() <= 1)	// 0-1 Spalte => Vektor
	//		s = Shape( m.size() );
	//	else				// mehrere Spalten => Matrix
	//		s = Shape( m.size(), n.size() );
	//}
	if (cols == 0)	// Single Index
	{
		if (rows == 1)	// 1 Row => Skalar
			return Shape();
		else
			return Shape(rows);	// mehrere Zeilen => Vektor
	}
	else if (cols == 1)
	{
		if (rows <= 1)
			return Shape();
		else
			return Shape(rows);
	}
	else	// cols > 1
	{
		if (rows == 1) 
			return Shape(1, 1,cols);
		else
			return Shape(rows,cols);	// Matrix
	}

}
/*****************************************************************************/


// Get Item
/*****************************************************************************/
static PyObject* CBasic_get(CBasicObject *self, PyObject *index)
/*****************************************************************************/
{
	try
	{
		BasicPtr &basic = self->m_basic;
		BasicPtr element;

		// Index holen
		std::vector<int> m;
		std::vector<int> n;
		if (!getIndex(index, basic->getShape(), m, n))
			return NULL;

		// Abfrage
		switch(basic->getType())
		{
		case Type_Matrix:
			{
				const Matrix* mat = Util::getAsConstPtr<Matrix>(basic);
				if (slicing(m,n))	// Haben wir Slicing
				{
					// Shape holen
					Shape s = getShape(m,n);
					Matrix *tmp = new Matrix(s);

					// Alle Elemente durchlaufen
					for(size_t i=0; i<m.size(); ++i)
					{
						if (n.size() == 0)	// keine Spalte angegeben
							tmp->set(i, mat->get(m[i]));
						else
							for(size_t j=0; j<n.size(); ++j)
								tmp->set(i,j, mat->get(m[i], n[j]));
					}

					// Fertig
					element = BasicPtr(tmp);
				}
				else	// kein Slicing
				{
					if (n.size() == 1)
						element = mat->get(m[0],n[0]);
					else
						element = mat->get(m[0]);
				}
				break;
			}
		case Type_Zero:
			{
				// Shape bestimmen
				Shape s = getShape(m,n);
				// Null erstellen
				element = Zero::getZero(s);
				break;
			}
		case Type_Eye:
			{
				// Slicing ist nur im Falle von Matrizen-aehnlichen Typen erlaubt
				if (slicing(m,n)) 
          throw ShapeError("Slicing is currently not supported for Identity matrices!");
				element = (m == n) ? Int::getOne() : Int::getZero();

				break;
			}
		default:
			{
				if (!slicing(m,n)) // no slicing
				{
					//row und col
					int row = m[0];
					int col = (n.size() == 1) ? n[0] : 0;
					element = Element::New(basic, row, col);
				}
				else // slicing
				{
					// Matrix anlegen
					Shape s = getShape(m,n);
					Matrix *tmp = new Matrix(s);

					// Matrix mit Elements befuellen
					for(size_t i=0; i<m.size(); ++i)
					{
						if (n.size() == 0)	// keine Spalte angegeben
							tmp->set(i, Element::New(basic, m[i],0));
						else
							for(size_t j=0; j<n.size(); ++j)
								tmp->set(i,j, Element::New(basic, m[i],n[j]));
					}

					// Fertig
					element = BasicPtr(tmp);
				}
				break;
			}
		}
		
		// Element in korrektes PythonObjekt casten
		PyObject* obj = PyFactory::New( element );
		Py_IncRef(obj);
		return obj;
	}
	STD_ERROR_HANDLER(NULL);
}
/*****************************************************************************/


// Set Item
/*****************************************************************************/
static int CBasic_set(CBasicObject *self, PyObject *index, PyObject *value)
/*****************************************************************************/
{
	try
	{
		BasicPtr &basic = self->m_basic;

		// Index holen
		std::vector<int> m;
		std::vector<int> n;
		if (!getIndex(index, basic->getShape(), m, n))
			return -1;

		// Ist es eine Matrix?
		if ((basic->getType() != Type_Matrix) && 
			(basic->getType() != Type_Zero) &&
			(basic->getType() != Type_Eye))
		{	
			PyErr_SetString(SymbolicsError, "Only elements of matrices can be set!");
			return -1;
		}

		// Value in Basic casten
		BasicPtr b( getBasic(value) );

		// Shape holen und vergleichen
		Shape s = getShape(m,n);
		Shape sb = b->getShape();
		// Shape "weich" vergleichen
		if ( (s.getDimension(1) != sb.getDimension(1)) || (s.getDimension(2) != sb.getDimension(2)) )
		{
			std::string msg = "Shape of lhs " + b->getShape().toString() + " does not match shape of rhs " + s.toString() + "!";
			throw ShapeError(msg.c_str());
		}

		// Zero in Matrix wandeln
		if (basic->getType() == Type_Zero)
		{
			// Casten des Objektes fuer Python
			self->ob_type = &CMatrixObjectType;
			Py_INCREF(&CMatrixObjectType);
			self->m_basic = BasicPtr( new Matrix( basic->getShape() ) );
			basic = self->m_basic;
		}

		// Eye in Matrix wandeln
		if (basic->getType() == Type_Eye)
		{
			// Casten des Objektes fuer Python
			self->ob_type = &CMatrixObjectType;
			Py_INCREF(&CMatrixObjectType);
			Matrix *mat = new Matrix( sb );
			self->m_basic = BasicPtr( mat );
			basic = self->m_basic;
			size_t min = sb.getDimension(1);
			if (sb.getDimension(2) < min) min = sb.getDimension(2);
			for(size_t i=0; i<min; ++i)
				mat->set(i,i, Int::getOne());
		}

		// Matrix holen
		Matrix* mat = Util::getAsPtr<Matrix>(self->m_basic);

		// Wert(e) setzen
		if (slicing(m,n))	// Slicing
		{
			for (size_t i=0; i<m.size(); ++i)
				if (n.size() == 0)
					mat->set(m[i], Element::New(b,i,0));
				else
					for (size_t j=0; j<n.size(); ++j)
						mat->set(m[i],n[j], Element::New(b,i,j));
		}
		else				// Kein Slicing
		{
			if (n.size() > 0)
				mat->set(m[0],n[0], b);
			else
				mat->set(m[0], b);
		}

		// Alles ok
		return 0;
	}
	STD_ERROR_HANDLER(-1);
}
/*****************************************************************************/



/*****************************************************************************/
static PyObject* CBasic_cmp(CBasicObject *self, PyObject *other, int op)
/*****************************************************************************/
{
	try
	{
		// lhs und rhs holen
		BasicPtr lhs( self->m_basic );
		BasicPtr rhs( getBasic(other) );

		// op auswerten
		bool result;
		switch(op)
		{
    case Py_LT:
      if (((lhs->getType() == Type_Real) || (lhs->getType() == Type_Int) || (lhs->getType() == Type_Zero)) &&
         ((rhs->getType() == Type_Real) || (rhs->getType() == Type_Int) || (rhs->getType() == Type_Zero)))
      {
        result = (lhs < rhs) ; break;
      }
      else
      {
			  PyErr_BadArgument();
  			return NULL;
      }
		case Py_EQ:
			result = (lhs == rhs) ; break;
		case Py_NE:
			result = (lhs != rhs) ; break;
    case Py_GT:
      if (((lhs->getType() == Type_Real) || (lhs->getType() == Type_Int) || (lhs->getType() == Type_Zero)) &&
         ((rhs->getType() == Type_Real) || (rhs->getType() == Type_Int) || (rhs->getType() == Type_Zero)))
      {
        result = (lhs > rhs) ; break;
      }
      else
      {
			  PyErr_BadArgument();
  			return NULL;
      }
		default:
			PyErr_BadArgument();
			return NULL;
		}

		// Rueckgabe
		if (result)
		{
			Py_IncRef(Py_True);
			return Py_True;
		}
		else
		{
			Py_IncRef(Py_False);
			return Py_False;
		}
	}
	STD_ERROR_HANDLER(NULL);
}
/*****************************************************************************/



/*****************************************************************************/
PyObject* CBasic_atoms(CBasicObject *self, PyObject *args)
/*****************************************************************************/
{
	// Atoms extrahieren
	Basic::BasicSet atoms = self->m_basic->getAtoms();

	// Atoms in Tule verpacken
	PyObject *tuple = PyTuple_New( atoms.size() );
	int i=0;
	for (Basic::BasicSet::const_iterator it=atoms.begin(); it!=atoms.end(); it++)
	{
		// Atom holen
		BasicPtr b(*it);
		// In PyObject wandeln
		PyObject *atom = PyFactory::New( b );
		// RefCount erhoehen
		Py_IncRef(atom);
		// und in Tuple einfuegen
		PyTuple_SetItem(tuple, i++, atom);
	}

	// RefCount erhoehen
	Py_IncRef(tuple);
	return tuple;
}
/*****************************************************************************/


/*****************************************************************************/
PyObject* CBasic_norm(CBasicObject *self, PyObject *args)
/*****************************************************************************/
{
	// Referenz auf Basic holen
	BasicPtr const& basic = self->m_basic;

	// Nach Typ unterscheiden
	switch (basic->getType())
	{
	case Type_Zero:		// Norm(0) = 0
		return Py_BuildValue("i", 0);
	case Type_Matrix:	// Norm(v) = sqrt(Summe(v_i*v_i))
		{
			// Auf Vektor pruefen
			if (!basic->is_Vector())
			{
				PyErr_SetString(SymbolicsError, "norm() is only implemented for vectors!");
				return NULL;
			}

			// Matrix holen
			const Matrix *mat = Util::getAsConstPtr<Matrix>(basic);
			Shape const& s = mat->getShape();
			
			// Los gehts
			BasicPtr normSquared( Int::getZero() );
			for (size_t i=0; i<s.getNumEl(); ++i)
			{
				BasicPtr vi( mat->get(i) );
				normSquared = normSquared + vi*vi;
			}

			// Und die Wurzel ziehen
			BasicPtr norm( Pow::New(normSquared, Real::New(0.5)) );

			// Element zurueckgeben
			PyObject *obj = PyFactory::New(norm);
			Py_IncRef(obj);
			return obj;
		}
	default:			// Da muss man sich mal Gedanken machen
		PyErr_SetString(SymbolicsError, "norm() is only implemented for matrix types!");
		return NULL;
	}
}
/*****************************************************************************/
