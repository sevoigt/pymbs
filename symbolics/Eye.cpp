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

#include "Eye.h"
#include "Zero.h"
#include "Int.h"
#include "Real.h"
#include "Util.h"
#include "Operators.h"
#include "Matrix.h"

using namespace Symbolics;

/*****************************************************************************/
BasicPtr Eye::eye = BasicPtr( new Eye() );
BasicPtr Eye::getArgres;
/*****************************************************************************/

/*****************************************************************************/
Eye::Eye(): Basic(Type_Eye)
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
Eye::Eye(Shape const& s): Basic(Type_Eye,s)
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
Eye::~Eye()
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
 BasicPtr Eye::iterateExp(Symbolics::Basic::Iterator &v)
/*****************************************************************************/
{
    return v.process_Arg( BasicPtr(this) );
}
/*****************************************************************************/

/*****************************************************************************/
bool Eye::operator==(  Basic const& rhs ) const
/*****************************************************************************/
{
    // Typ pruefen
    switch (rhs.getType())
    {
    case Type_Eye: 
        return true;
    case Type_Int:
        {
            const Int &i = dynamic_cast<const Int&>(rhs);
            return 0 == i.getValue();
        }
    case Type_Real:
        {
            const Real &r = dynamic_cast<const Real&>(rhs);
            return 0 == r.getValue();
        }
    case Type_Matrix:
      {
        const Matrix &mat = dynamic_cast<const Matrix&>(rhs);
        if (mat.is_Vector())
        {
          size_t elem = mat.getShape().getNumEl();
          for (size_t i=0; i<elem; ++i)
            if (mat.get(i) != Int::getOne())
              return false;
            return true;
        }
        size_t dim1 = mat.getShape().getDimension(1);
        size_t dim2 = mat.getShape().getDimension(2);
        if (dim1 != dim2)
            return false;
        for (size_t i=0; i<dim1; ++i)
          if (mat.get(i,i) != Int::getOne())
            return false;
        for (size_t i=0; i<dim1; ++i)
          for (size_t j=0; j<dim2; ++j)
            if (i!=j)
              if (mat.get(i,i) != Int::getZero())
                return false;
        return true;
      }
    default:
        return false;
    }
}
/*****************************************************************************/
 
/*****************************************************************************/
 BasicPtr Eye::subs( ConstBasicPtr const& old_exp, BasicPtr const& new_exp)
/*****************************************************************************/
{
    // Sind wir es selbst, die ersetzt werden sollen?
    if (*this == *old_exp.get())
        return new_exp;

    // uns selbs zurueckgeben
    return BasicPtr(this);
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Eye::getEye(Shape const& s)
/*****************************************************************************/
{
    if (s.getNumEl() == 1)
        return eye;
    return BasicPtr(new Eye(s));
}
/*****************************************************************************/

 /*****************************************************************************/
bool Symbolics::operator== (Eye const& lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    if (rhs->getType() == Type_Eye)
    {
        return true;

    }
    if (rhs->getType() == Type_Int)
    {
        const Int *crhs = Util::getAsConstPtr<Int>(rhs);
        return 1 == (*crhs).getValue();

    }
    if (rhs->getType() == Type_Real)
    {
        const Real *crhs = Util::getAsConstPtr<Real>(rhs);
        return 1 == (*crhs).getValue();

    }
    if(rhs->getType() ==  Type_Matrix)
    {
        const Matrix *mat = Util::getAsConstPtr<Matrix>(rhs);
        if (mat->is_Vector())
        {
            size_t elem = mat->getShape().getNumEl();
            for (size_t i=0; i<elem; ++i)
            if (mat->get(i) != Int::getOne())
                return false;
            return true;
        }
        size_t dim1 = mat->getShape().getDimension(1);
        size_t dim2 = mat->getShape().getDimension(2);
        if (dim1 != dim2)
            return false;
        for (size_t i=0; i<dim1; ++i)
            if (mat->get(i,i) != Int::getOne())
            return false;
        for (size_t i=0; i<dim1; ++i)
            for (size_t j=0; j<dim2; ++j)
            if (i!=j)
                if (mat->get(i,i) != Int::getZero())
                return false;
        return true;
    }
    return false;
}
/*****************************************************************************/

/*****************************************************************************/
bool Symbolics::operator== (BasicPtr const& lhs, Eye  const& rhs)
/*****************************************************************************/
{
    if (lhs->getType() == Type_Eye)
    {
        return true;

    }
    if (lhs->getType() == Type_Int)
    {
        const Int *clhs = Util::getAsConstPtr<Int>(lhs);
        return 1 == (*clhs).getValue();

    }
    if (lhs->getType() == Type_Real)
    {
        const Real *clhs = Util::getAsConstPtr<Real>(lhs);
        return 1 == (*clhs).getValue();

    }
    if(lhs->getType() ==  Type_Matrix)
    {
        const Matrix *mat = Util::getAsConstPtr<Matrix>(lhs);
        if (mat->is_Vector())
        {
            size_t elem = mat->getShape().getNumEl();
            for (size_t i=0; i<elem; ++i)
            if (mat->get(i) != Int::getOne())
                return false;
            return true;
        }
        size_t dim1 = mat->getShape().getDimension(1);
        size_t dim2 = mat->getShape().getDimension(2);
        if (dim1 != dim2)
            return false;
        for (size_t i=0; i<dim1; ++i)
            if (mat->get(i,i) != Int::getOne())
            return false;
        for (size_t i=0; i<dim1; ++i)
            for (size_t j=0; j<dim2; ++j)
            if (i!=j)
                if (mat->get(i,i) != Int::getZero())
                return false;
        return true;
    }
    return false;
}
/*****************************************************************************/

/*****************************************************************************/
bool Symbolics::operator!= (Eye const& lhs, BasicPtr  const& rhs)
/*****************************************************************************/
{
    if (rhs->getType() == Type_Eye)
    {
        return false;

    }
    if (rhs->getType() == Type_Int)
    {
        const Int *crhs = Util::getAsConstPtr<Int>(rhs);
        return 1 != (*crhs).getValue();

    }
    if (rhs->getType() == Type_Real)
    {
        const Real *crhs = Util::getAsConstPtr<Real>(rhs);
        return 1 != (*crhs).getValue();

    }
    if(rhs->getType() ==  Type_Matrix)
    {
        const Matrix *mat = Util::getAsConstPtr<Matrix>(rhs);
        if (mat->is_Vector())
        {
            size_t elem = mat->getShape().getNumEl();
            for (size_t i=0; i<elem; ++i)
            if (mat->get(i) != Int::getOne())
                return true;
            return false;
        }
        size_t dim1 = mat->getShape().getDimension(1);
        size_t dim2 = mat->getShape().getDimension(2);
        if (dim1 != dim2)
            return true;
        for (size_t i=0; i<dim1; ++i)
            if (mat->get(i,i) != Int::getOne())
                return true;
        for (size_t i=0; i<dim1; ++i)
            for (size_t j=0; j<dim2; ++j)
            if (i!=j)
                if (mat->get(i,i) != Int::getZero())
                    return true;
        return false;
    }
    return true;
}
/*****************************************************************************/

/*****************************************************************************/
bool Symbolics::operator!= (BasicPtr const& lhs, Eye  const& rhs)
/*****************************************************************************/
{
    if (lhs->getType() == Type_Eye)
    {
        return false;

    }
    if (lhs->getType() == Type_Int)
    {
        const Int *clhs = Util::getAsConstPtr<Int>(lhs);
        return 1 != (*clhs).getValue();

    }
    if (lhs->getType() == Type_Real)
    {
        const Real *clhs = Util::getAsConstPtr<Real>(lhs);
        return 1 != (*clhs).getValue();

    }
    if(lhs->getType() ==  Type_Matrix)
    {
        const Matrix *mat = Util::getAsConstPtr<Matrix>(lhs);
        if (mat->is_Vector())
        {
            size_t elem = mat->getShape().getNumEl();
            for (size_t i=0; i<elem; ++i)
            if (mat->get(i) != Int::getOne())
                return true;
            return false;
        }
        size_t dim1 = mat->getShape().getDimension(1);
        size_t dim2 = mat->getShape().getDimension(2);
        if (dim1 != dim2)
            return true;
        for (size_t i=0; i<dim1; ++i)
            if (mat->get(i,i) != Int::getOne())
                return true;
        for (size_t i=0; i<dim1; ++i)
            for (size_t j=0; j<dim2; ++j)
            if (i!=j)
                if (mat->get(i,i) != Int::getZero())
                    return true;
        return false;
    }
    return true;
}
/*****************************************************************************/
