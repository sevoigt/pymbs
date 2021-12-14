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

#ifdef WIN32
  #define _WIN32_WINNT 0x0501        // = Windows XP, sonst gibt es kein "GetSystemTimes"

  #pragma warning(disable: 4251)
  #define _CRT_SECURE_NO_DEPRECATE
  #include <direct.h>
  #include <io.h>
  #include <sys/timeb.h>
  #include <shlwapi.h>
  #include <windows.h>
#else
  #include <sys/time.h>
  #include <sys/stat.h>
  #include <sys/types.h>
  #include <sys/sysinfo.h>
  #include <sys/wait.h>
  #include <unistd.h>
  #include <iostream>
  #include <dirent.h>
  #include <stdio.h>
  #include <dlfcn.h>
#endif


#include <math.h>

#include "Util.h"
#include "Neg.h"
#include "Int.h"
#include "Matrix.h"
#include "Mul.h"
#include "Pow.h"

#include "Acos.h"
#include "Asin.h"
#include "Atan.h"
#include "Atan2.h"
#include "Cos.h"
#include "Sin.h"
#include "Tan.h"

using namespace Symbolics;


/*****************************************************************************/
double Util::getTime()
/*****************************************************************************/
{
#ifdef WIN32
    LARGE_INTEGER freq, time;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&time);
    return (double)(time.QuadPart)/(double)freq.QuadPart;
#else
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME,&tp);
    return (double)(tp.tv_sec)+1e-9*(double)(tp.tv_nsec);  
#endif
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Util::intDbl( BasicPtr const& cint)
/*****************************************************************************/
{
    const Basic *b = cint.get();
    if (cint->getType() == Type_Real)
    {
        return cint;
    }
    if (cint->getType() == Type_Int)
    {
        const Int *c = Util::getAsConstPtr<const Int>(cint);
        BasicPtr cdbl( new Real(*c) );
        return cdbl;
    }
    throw InternalError("Symbolics::Util::intDbl no Int!");
    return BasicPtr();
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Util::pow( BasicPtr const& lhs,  BasicPtr const& rhs)
/*****************************************************************************/
{
    return Pow::New(lhs,rhs);
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Util::pow( BasicPtr const& lhs,  double const& rhs)
/*****************************************************************************/
{
    return Pow::New(lhs,Real::New(rhs));
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Util::pow( BasicPtr const& lhs,  int const& rhs)
/*****************************************************************************/
{
    return Pow::New(lhs,Int::New(rhs));
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Util::div( BasicPtr const& lhs,  BasicPtr const& rhs)
/*****************************************************************************/
{
    BasicPtr div = Mul::New(lhs,Pow::New(rhs,Int::getMinusOne()));
    return div->simplify();
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Util::sqrt( BasicPtr const& arg)
/*****************************************************************************/
{
    BasicPtr sqrt = Pow::New(arg,BasicPtr(new Real(0.5)));
    return sqrt->simplify();
}
/*****************************************************************************/

/*****************************************************************************/
bool Util::is_Zero( BasicPtr const& e)
/*****************************************************************************/
{
    switch (e->getType())
    {
    case Type_Zero:
        return true;
    // constant
    case Type_Int:
    case Type_Real:
        return e == Zero::getZero();
    // matrix
    case Type_Matrix:
    {
        size_t argssize = e->getArgsSize();
        for (size_t i=0;i<argssize;++i)
          if (!(e->getArg(i) == Zero::getZero()))
                return false;
        return true;
    }
    // -(-(a)) = a
    case Type_Neg :
    {
        const Basic *b = e.get();
        const Neg *c = dynamic_cast<const Neg*>(b);
        return is_Zero(c->getArg());
    }
    default:
        return false;
    }
}
/*****************************************************************************/

/*****************************************************************************/
bool Util::is_One( BasicPtr const& e)
/*****************************************************************************/
{
    switch (e->getType())
    {
    // Constant
    case Type_Int:
    case Type_Real:
        return e == Int::getOne();
    // matrix 
    case Type_Matrix:
    {
        if (e->is_Vector())
        {
          size_t argssize = e->getArgsSize();
          for (size_t i=0;i<argssize;++i)
            if (!(e->getArg(i) == Int::getOne()))
              return false;
            return true;
        }
        size_t dim1 = e->getShape().getDimension(1);
        if (dim1 != e->getShape().getDimension(2))
            return false;
        BasicPtr eye = Util::eye(e->getShape());
        return eye == e;
    }
    // -(-(1)) = 1
    case Type_Neg:
    {
        const Basic *b = e.get();
        const Neg *c = dynamic_cast<const Neg*>(b);
        return is_One(c->getArg());
    }
    default:
        return false;
    }
}
/*****************************************************************************/

/*****************************************************************************/
bool Util::is_Const( BasicPtr const& arg)
/*****************************************************************************/
{
    Basic::BasicSet atoms = arg->getAtoms();
    return atoms.size() == 0;
}
/*****************************************************************************/

/*****************************************************************************/
bool Util::has_Function( BasicPtr const& exp, Basic_Type const& type)
/*****************************************************************************/
{
    if (exp.get() == NULL) throw InternalError("Symbolics::Util::has_Function(1)");
    if (exp->getType() == type)
        return true;
    size_t argssize = exp->getArgsSize();
    for (size_t i=0;i<argssize;++i)
    {
      if (has_Function(exp->getArg(i),type)) 
            return true;
    }

    return false;
}
/*****************************************************************************/


/*****************************************************************************/
bool Util::is_Int(BasicPtr const& basic, int &i_Out)
/*****************************************************************************/
{
    switch(basic->getType())
    {
    case Type_Int:
        {
            const Int *i = Util::getAsConstPtr<Int>(basic);
            i_Out = i->getValue();
            return true;
        }
    case Type_Real:
        {
            const Real *r = Util::getAsConstPtr<Real>(basic);
            double dbl = r->getValue();
            if (dbl >= 0)
                i_Out = static_cast<int>(dbl+0.5);
            else
                i_Out = static_cast<int>(dbl-0.5);
            return (fabs(dbl-(double)i_Out) < 1e-13);
        }
    default:
        i_Out = 0;
        return false;
    }
}
/*****************************************************************************/


/*****************************************************************************/
BasicPtr Util::eye( Shape const& s)
/*****************************************************************************/
{
    if ( (s.getNrDimensions() != 2) && (s.getNrDimensions() != 0))
        throw InternalError("Util:: cannot generate Eye from Vector Shape!");
    if (s.getDimension(1) != s.getDimension(2))
        throw InternalError("Util:: cannot generate Eye from Shape with different Dimensions!");

    if (s.getNrDimensions() == 0)
      return Int::getOne();

    Matrix *mat = new Matrix(s);
    size_t dim1 = s.getDimension(1);
    for (size_t i=0; i<dim1; ++i)
        mat->set(i,i,Int::getOne());

    return BasicPtr(mat);
}
/*****************************************************************************/

/*****************************************************************************/
void Util::newScope()
/*****************************************************************************/
{
    Acos::newScope();
    Asin::newScope();
    Atan::newScope();
    Atan2::newScope();
    Cos::newScope();
    Sin::newScope();
    Tan::newScope();
}
/*****************************************************************************/
