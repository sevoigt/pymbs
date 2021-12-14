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

#include "PyFactory.h"
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

using namespace Symbolics::Python;

/*****************************************************************************/
PyObject* PyFactory::New( BasicPtr &b )
/*****************************************************************************/
{
	// Neuen CBasic anlegen, sind eh alle gleich ...
	CBasicObject* basic = new CBasicObject();
	basic->m_basic = b;
	PyObject *obj = NULL;

	// Typinformationen anhaengen
	switch(b->getType())
	{
    case Type_Int:
		{
			const Int *i = Util::getAsConstPtr<Int>(b);
			obj = PyLong_FromLong(i->getValue());
			break;
		}
    case Type_Real:
		{
			const Real *r = Util::getAsConstPtr<Real>(b);
			obj = PyFloat_FromDouble(r->getValue());
			break;
		}
    default:
		obj = PyObject_Init( basic, ObjectType(b) );
		break;
	}
    
	// Objekt zurueckgeben, nicht vergessen RefCount zu erhoehen!
	return obj;
}
/*****************************************************************************/

/*****************************************************************************/
PyTypeObject* PyFactory::ObjectType( BasicPtr &b )
/*****************************************************************************/
{
	// Typinformationen anhaengen
	switch(b->getType())
	{
  case Type_Symbol:
		return &CSymbolObjectType;
		break;
  case Type_Int:
    return &PyLong_Type;
  	break;
  case Type_Real:
    return &PyFloat_Type;
  	break;
	case Type_Matrix:
		return &CMatrixObjectType;
		break;
  //  case Type_Neg:
		//obj = PyObject_Init( basic, &CSymbolObjectType );
		//break;
  //  case Type_Add:
		//obj = PyObject_Init( basic, &CSymbolObjectType );
		//break;
  //  case Type_Mul:
		//obj = PyObject_Init( basic, &CSymbolObjectType );
		//break;
  case Type_Sin:
		return &CSinObjectType;
		break;
  case Type_Cos:
		return &CCosObjectType;
		break;
  case Type_Der:
		return &CDerObjectType ;
		break;
  case Type_Atan:
		return &CAtanObjectType;
		break;
  //  case Type_Abs:
		//obj = PyObject_Init( basic, &CAbsObjectType );
		//break;
  case Type_Acos:
		return &CAcosObjectType;
		break;
  case Type_Asin:
		return &CAsinObjectType;
		break;
  case Type_Scalar:
		return &CScalarObjectType;
		break;
  case Type_Skew:
		return &CSkewObjectType;
		break;
  case Type_Transpose:
		return &CTransposeObjectType;
		break;
  case Type_Unknown:
		return &CSymbolObjectType;
		break;
  case Type_Tan:
		return &CTanObjectType;
		break;
  default:
		return &CBasicObjectType;
		break;
	}
    
	// Objekt zurueckgeben, nicht vergessen RefCount zu erhoehen!
	return &CBasicObjectType;
}
/*****************************************************************************/