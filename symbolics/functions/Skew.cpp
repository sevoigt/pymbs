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

#include "Skew.h"
#include "Int.h"
#include "Matrix.h"
#include "Neg.h"
#include "Util.h"
#include "Operators.h"

using namespace Symbolics;

/*****************************************************************************/
Skew::Skew( BasicPtrVec const& args ): UnaryOp(Type_Skew, args)
/*****************************************************************************/
{
	validate();
}
/*****************************************************************************/


/*****************************************************************************/
Skew::Skew( BasicPtr const& arg ): UnaryOp(Type_Skew, arg)
/*****************************************************************************/
{
	validate();
}
/*****************************************************************************/


/*****************************************************************************/
Skew::~Skew()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
void Skew::validate()
/*****************************************************************************/
{
    // Skew ist für 0 und einen 3x1 Vektor definiert
    if ( (getArg() != Zero::getZero()) && (getArg()->getShape() != Shape(3)) )
    {
         std::string msg = "Skew is only defined for 3x1 Vectors! Shape of argument is " + getArg()->getShape().toString() + ".";
        throw ShapeError( msg.c_str() );
    }
    // Shape auf 3x3 festsetzen
    m_shape = Shape(3,3);
}
/*****************************************************************************/


/*****************************************************************************/
std::string Skew::toString() const
/*****************************************************************************/
{
#ifdef _DEBUG
    return "Skew(" + getArg()->toString() + ")";
#else
    return "Skew(" + getArg()->toString() + ")";
#endif
}
/*****************************************************************************/


/*****************************************************************************/
BasicPtr Skew::simplify()
/*****************************************************************************/
{
    if (m_simplified)
        return BasicPtr(this);
    // Argument vereinfachen
    simplifyArg();

    // if zero, return zero
    if (Util::is_Zero(getArg())) return Zero::getZero(m_shape);

    // simplify matrices
    switch(getArg()->getType())
    {
    case Type_Matrix:
        {
            const Matrix *v = Util::getAsConstPtr<Matrix>(getArg());
            Matrix *m = new Matrix( Shape(3,3) );
            BasicPtr &Zero = Zero::getZero();
            *m = Zero, -v->get(2), v->get(1),
                 v->get(2), Zero, -v->get(0),
                -v->get(1), v->get(0), Zero;
            return BasicPtr( m );
        }
    case Type_Neg:
        {
            const Neg* neg = Util::getAsConstPtr<Neg>(getArg());
            return Neg::New(New(neg->getArg()));
        }
    }
    m_simplified = true;
    return BasicPtr(this);
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Skew::New( BasicPtr const& arg)
/*****************************************************************************/
{
    // if zero, return zero
    if (Util::is_Zero(arg)) return Zero::getZero(Shape(3,3));

    // simplify matrices
    switch(arg->getType())
    {
    case Type_Matrix:
        {
            const Matrix *v = Util::getAsConstPtr<Matrix>(arg);
            Matrix *m = new Matrix( Shape(3,3) );
            BasicPtr &Zero = Zero::getZero();
            *m = Zero, -(*v)(2), (*v)(1),
                 (*v)(2), Zero, -(*v)(0),
                -(*v)(1), (*v)(0), Zero;
            return BasicPtr( m );
        }
    case Type_Neg:
        {
            const Neg* neg = Util::getAsConstPtr<Neg>(arg);
            return Neg::New(New(neg->getArg()));
        }
    }
    return BasicPtr(new Skew(arg));
}
/*****************************************************************************/