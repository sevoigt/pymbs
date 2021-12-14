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

#include <stdarg.h>
#include "Atan2.h"
#include "Neg.h"
#include "Int.h"
#include "Real.h"
#include "Pow.h"
#include "Add.h"
#include "Mul.h"
#include "Util.h"
#include <math.h>

using namespace Symbolics;

ScopePtr  Atan2::m_scope = ScopePtr(new Scope()); 

/*****************************************************************************/
Atan2::Atan2( BasicPtrVec const& args ): BinaryOp(Type_Atan2, args), List(m_scope)
/*****************************************************************************/
{
	validate();
}
/*****************************************************************************/


/*****************************************************************************/
Atan2::Atan2( BasicPtr const& arg1, BasicPtr const& arg2 ): BinaryOp(Type_Atan2, arg1, arg2), List(m_scope)
/*****************************************************************************/
{
	validate();
}
/*****************************************************************************/


/*****************************************************************************/
Atan2::~Atan2()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
void Atan2::validate()
/*****************************************************************************/
{
	// Shape
    m_shape = getArg1()->getShape();

    // Argumente muessen skalar sein!
    if (!getArg1()->is_Scalar())
      throw ShapeError("Internal Error: Atan2 - First argument must be scalar!");
    if (!getArg2()->is_Scalar())
      throw ShapeError("Internal Error: Atan2 - Second argument must be scalar!");
}
/*****************************************************************************/


/*****************************************************************************/
std::string Atan2::toString() const
/*****************************************************************************/
{
#ifdef _DEBUG
    return "atan2(" + getArg1()->toString() + "," + getArg1()->toString() + ")";
#else
    return "atan2(" + getArg1()->toString() + "," + getArg1()->toString() + ")";
#endif
}
/*****************************************************************************/


/*****************************************************************************/
BasicPtr Atan2::simplify()
/*****************************************************************************/
{
    if (m_simplified)
        return BasicPtr(this);
    // Argument vereinfachen
    simplifyArgs();


    // fuer Konstanten einfach die Operation ausfuehren
    // Was haben wir denn als Argument?
    // Zero, Zero
    if ((getArg1()->getType() == Type_Zero) && (getArg2()->getType() == Type_Zero))
    {
        return BasicPtr( new Real(atan2(0.0,0.0)) );
    }
    // Zero, Int
    if ((getArg1()->getType() == Type_Zero) && (getArg2()->getType() == Type_Int))
    {
        const Int *src2 = Util::getAsConstPtr<Int>(getArg2());

        return BasicPtr( new Real(atan2(0.0,
                static_cast<double>(src2->getValue()) )) );
    }
    // Int, Zero
    if ((getArg1()->getType() == Type_Int) && (getArg2()->getType() == Type_Zero))
    {
        const Int *src1 = Util::getAsConstPtr<Int>(getArg1());

        return BasicPtr( new Real(atan2(static_cast<double>(src1->getValue()),0.0 )) );
    }
    // Zero, Real
    if ((getArg1()->getType() == Type_Zero) && (getArg2()->getType() == Type_Real))
    {
        const Real *src2 = Util::getAsConstPtr<Real>(getArg2());

        return BasicPtr( new Real(atan2(0.0,src2->getValue()) ) );
    }
    // Real, Zero
    if ((getArg1()->getType() == Type_Real) && (getArg2()->getType() == Type_Zero))
    {
        const Real *src1 = Util::getAsConstPtr<Real>(getArg1());

        return BasicPtr( new Real(atan2(src1->getValue(),0.0 )) );
    }
    // Int, Int
    if ((getArg1()->getType() == Type_Int) && (getArg2()->getType() == Type_Int))
    {
        const Int *src1 = Util::getAsConstPtr<Int>(getArg1());
        const Int *src2 = Util::getAsConstPtr<Int>(getArg2());

        return BasicPtr( new Real(atan2( 
                static_cast<double>(src1->getValue()),
                static_cast<double>(src2->getValue()) )) );
    }
    // Dbl,Int
    if ((getArg1()->getType() == Type_Real) && (getArg2()->getType() == Type_Int))
    {
        const Real *src1 = Util::getAsConstPtr<Real>(getArg1());
        const Int *src2 = Util::getAsConstPtr<Int>(getArg2());

        return BasicPtr( new Real(atan2( 
                                    src1->getValue(),
                static_cast<double>(src2->getValue()) )) );
    }
    // Int, Dbl
    if ((getArg1()->getType() == Type_Int) && (getArg2()->getType() == Type_Real))
    {
        const Int *src1 = Util::getAsConstPtr<Int>(getArg1());
        const Real *src2 = Util::getAsConstPtr<Real>(getArg2());

        return BasicPtr( new Real(atan2( 
                static_cast<double>(src1->getValue()),
                                    src2->getValue() )) );
    }
    // Dbl, Dbl
    if ((getArg1()->getType() == Type_Real) && (getArg2()->getType() == Type_Real))
    {
        const Real *src1 = Util::getAsConstPtr<Real>(getArg1());
        const Real *src2 = Util::getAsConstPtr<Real>(getArg2());

        return BasicPtr( new Real(atan2( src1->getValue(), src2->getValue() )) );
    }
    m_simplified = true;
    return BasicPtr(this);
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Atan2::New( BasicPtr const& arg1, BasicPtr const& arg2)
/*****************************************************************************/
{
    // fuer Konstanten einfach die Operation ausfuehren
    // Was haben wir denn als Argument?
    // Zero, Zero
    if ((arg1->getType() == Type_Zero) && (arg2->getType() == Type_Zero))
    {
        return BasicPtr( new Real(atan2(0.0,0.0)) );
    }
    // Zero, Int
    if ((arg1->getType() == Type_Zero) && (arg2->getType() == Type_Int))
    {
        const Int *src2 = Util::getAsConstPtr<Int>(arg2);

        return BasicPtr( new Real(atan2(0.0,
                static_cast<double>(src2->getValue()) )) );
    }
    // Int, Zero
    if ((arg1->getType() == Type_Int) && (arg2->getType() == Type_Zero))
    {
        const Int *src1 = Util::getAsConstPtr<Int>(arg1);

        return BasicPtr( new Real(atan2(static_cast<double>(src1->getValue()),0.0 )) );
    }
    // Zero, Real
    if ((arg1->getType() == Type_Zero) && (arg2->getType() == Type_Real))
    {
        const Real *src2 = Util::getAsConstPtr<Real>(arg2);

        return BasicPtr( new Real(atan2(0.0,src2->getValue()) ) );
    }
    // Real, Zero
    if ((arg1->getType() == Type_Real) && (arg2->getType() == Type_Zero))
    {
        const Real *src1 = Util::getAsConstPtr<Real>(arg1);

        return BasicPtr( new Real(atan2(src1->getValue(),0.0 )) );
    }
    // Int, Int
    if ((arg1->getType() == Type_Int) && (arg2->getType() == Type_Int))
    {
        const Int *src1 = Util::getAsConstPtr<Int>(arg1);
        const Int *src2 = Util::getAsConstPtr<Int>(arg2);

        return BasicPtr( new Real(atan2( 
                static_cast<double>(src1->getValue()),
                static_cast<double>(src2->getValue()) )) );
    }
    // Dbl,Int
    if ((arg1->getType() == Type_Real) && (arg2->getType() == Type_Int))
    {
        const Real *src1 = Util::getAsConstPtr<Real>(arg1);
        const Int *src2 = Util::getAsConstPtr<Int>(arg2);

        return BasicPtr( new Real(atan2( 
                                    src1->getValue(),
                static_cast<double>(src2->getValue()) )) );
    }
    // Int, Dbl
    if ((arg1->getType() == Type_Int) && (arg2->getType() == Type_Real))
    {
        const Int *src1 = Util::getAsConstPtr<Int>(arg1);
        const Real *src2 = Util::getAsConstPtr<Real>(arg2);

        return BasicPtr( new Real(atan2( 
                static_cast<double>(src1->getValue()),
                                    src2->getValue() )) );
    }
    // Dbl, Dbl
    if ((arg1->getType() == Type_Real) && (arg2->getType() == Type_Real))
    {
        const Real *src1 = Util::getAsConstPtr<Real>(arg1);
        const Real *src2 = Util::getAsConstPtr<Real>(arg2);

        return BasicPtr( new Real(atan2( src1->getValue(), src2->getValue() )) );
    }
    return BasicPtr(new Atan2(arg1,arg2));
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Atan2::der()
/*****************************************************************************/
{   
    // der(atan2(x,y)) = der(y/x) / (1+(y/x)^2 )
  BasicPtr ydx = Util::div(getArg2(),getArg1());
  return Mul::New(ydx->der(),Pow::New(Add::New(Int::getOne(),Pow::New(ydx,Int::New(2))),Int::getMinusOne())); 
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Atan2::der(BasicPtr const& symbol)
/*****************************************************************************/
{   
    // der(atan2(x,y)) = der(y/x) / (1+(y/x)^2 )
  BasicPtr ydx = Util::div(getArg2(),getArg1());
  return Mul::New(ydx->der(symbol),Pow::New(Add::New(Int::getOne(),Pow::New(ydx,Int::New(2))),Int::getMinusOne())); 
}
/*****************************************************************************/