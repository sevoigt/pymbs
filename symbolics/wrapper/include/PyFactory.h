#ifndef __PYFACTORY_H_
#define __PYFACTORY_H_

#include <Python.h>
#include "Symbolics.h"

using namespace Symbolics;

namespace Symbolics
{
    namespace Python
    {
		class PyFactory
		{
		public:
			static PyObject* New( BasicPtr &b );
      static PyTypeObject* ObjectType( BasicPtr &b );
		};
    };
};

#endif // __PYFACTORY_H_