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

#ifndef __CBASIC_H_
#define __CBASIC_H_

#include <Python.h>
#include <stdexcept>
#include "Symbolics.h"

using namespace Symbolics;

namespace Symbolics
{
    namespace Python
    {
        struct _CBasic_Object_: public PyObject    // Hoffentlich kommt das PY_HEAD immer vor dem Rest ..., gibt es dafuer einen Standard?
        {
        public:
            BasicPtr m_basic;
            _CBasic_Object_();

        };
        typedef struct _CBasic_Object_ CBasicObject;

        extern PyTypeObject CBasicObjectType;
    };
};

#define STD_ERROR_HANDLER(x) \
	catch (Symbolics::IndexError &ie) \
	{ \
		std::string msg = "Index Error: "; \
		msg += ie.what(); \
		PyErr_SetString(SymbolicsError, msg.c_str()); \
        return x;\
	} \
	catch (Symbolics::ShapeError &se) \
	{ \
		std::string msg = "Shape Error: "; \
		msg += se.what(); \
		PyErr_SetString(SymbolicsError, msg.c_str()); \
        return x; \
	} \
	catch (Symbolics::InternalError &inte) \
	{ \
		std::string msg = "Internal Error: "; \
		msg += inte.what(); \
		PyErr_SetString(SymbolicsError, msg.c_str()); \
        return x; \
	} \
	catch (Graph::ExpressionSolveForError &esfe) \
	{ \
		std::string msg = "ExpressionSolveForError: "; \
		msg += esfe.what(); \
		PyErr_SetString(SymbolicsError, msg.c_str()); \
        return x; \
	} \
	catch (Graph::InitalValueError &ive) \
	{ \
		std::string msg = "InitalValueError: "; \
		msg += ive.what(); \
		PyErr_SetString(SymbolicsError, msg.c_str()); \
        return x; \
	} \
	catch (Graph::SymbolAlreadyExistsError &sae) \
	{ \
		std::string msg = "SymbolAlreadyExistsError: "; \
		msg += sae.what(); \
		PyErr_SetString(SymbolicsError, msg.c_str()); \
        return x; \
	} \
	catch (Graph::SymbolAlreadySolvedInError &sase) \
	{ \
		std::string msg = "SymbolAlreadySolvedInError: "; \
		msg += sase.what(); \
		PyErr_SetString(SymbolicsError, msg.c_str()); \
        return x; \
	} \
	catch (Graph::SymbolKindError &ske) \
	{ \
		std::string msg = "SymbolKindError: "; \
		msg += ske.what(); \
		PyErr_SetString(SymbolicsError, msg.c_str()); \
        return x; \
	} \
	catch (Graph::VarUnknownError &ve) \
	{ \
		std::string msg = "VarUnknownError: "; \
		msg += ve.what(); \
		PyErr_SetString(SymbolicsError, msg.c_str()); \
        return x; \
	} \
    catch (std::bad_alloc &ba) \
    { \
      std::string msg = "bad_alloc caught: "; \
		  msg += ba.what(); \
		  PyErr_SetString(SymbolicsError, msg.c_str()); \
          return x; \
    } \
	catch (std::out_of_range &oor) \
    { \
      std::string msg = "OutOfRange Error caught: "; \
		  msg += oor.what(); \
		  PyErr_SetString(SymbolicsError, msg.c_str()); \
          return x; \
    } \
    catch (std::exception &e) \
    { \
      std::string msg = "Standard Exception caught: "; \
		  msg += e.what(); \
		  PyErr_SetString(SymbolicsError, msg.c_str()); \
          return x; \
    } \
    catch (...) \
    { \
        PyErr_SetString(SymbolicsError, "Unknown Error!"); \
        return x; \
    }

#endif // __CBASIC_H_