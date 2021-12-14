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

#include "Factory.h"
#include "str.h"

using namespace Symbolics;

/*****************************************************************************/
Factory::Factory()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
Factory::~Factory()
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Factory::newBasic( Basic_Type type,  BasicPtr &arg, Shape const& shape)
/*****************************************************************************/
{
    switch (type)
    {
    case Type_Symbol:
        throw InternalError("Symbol is not supported by Factory!");
    case Type_Int:
        throw InternalError("Int is not supported by Factory!");
    case Type_Real:
        throw InternalError("Real is not supported by Factory!");
    case Type_Neg:
        return Neg::New(arg);
    case Type_Add:
        return arg;
    case Type_Mul:
        return arg;
    case Type_Sin:
        return Sin::New(arg);
    case Type_Cos:
        return Cos::New(arg);
    case Type_Der:
        return Der::New(arg);
    case Type_Atan:
        return BasicPtr( new Atan(arg) );
    case Type_Abs:
        return BasicPtr( new Abs(arg) );
    case Type_Acos:
        return BasicPtr( new Acos(arg) );
    case Type_Asin:
        return BasicPtr( new Asin(arg) );
    case Type_Scalar:
        return BasicPtr( new Scalar(arg) );
    case Type_Skew:
        return Skew::New(arg);
    case Type_Transpose:
        return BasicPtr( new Transpose(arg) );
    case Type_Unknown:
        throw InternalError("Unknown is not supported by Factory!");
    case Type_Tan:
        return BasicPtr( new Tan(arg) );
    default:
        throw InternalError("Unknown Type: " + str(type) + "!");
    };
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Factory::newBasic( Basic_Type type,  BasicPtr &arg1, BasicPtr &arg2, Shape const& shape)
/*****************************************************************************/
{
    switch (type)
    {
    case Type_Symbol:
        throw InternalError("Symbol is not supported by Factory!");
    case Type_Int:
        throw InternalError("Int is not supported by Factory!");
    case Type_Real:
        throw InternalError("Real is not supported by Factory!");
    case Type_Add:
        return Add::New(arg1,arg2);
    case Type_Mul:
        return Mul::New(arg1,arg2);
    case Type_Pow:
        return BasicPtr( new Pow(arg1,arg2) );
    case Type_Solve:
        return BasicPtr( new Solve(arg1,arg2) );
    case Type_Atan2:
        return BasicPtr( new Atan2(arg1,arg2) );
    case Type_Less:
        return BasicPtr( new Less(arg1,arg2) );
    case Type_Greater:
        return BasicPtr( new Greater(arg1,arg2) );
    case Type_Equal:
        return BasicPtr( new Equal(arg1,arg2) );
    case Type_Unknown:
        throw InternalError("Unknown is not supported by Factory!");
    default:
        throw InternalError("Unknown Type: " + str(type) + "!");
    };
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Factory::newBasic( Basic_Type type,  BasicPtrVec &args, Shape const& shape)
/*****************************************************************************/
{
    switch (type)
    {
    case Type_Symbol:
        throw InternalError("Symbol is not supported by Factory!");
    case Type_Int:
        throw InternalError("Int is not supported by Factory!");
    case Type_Real:
        throw InternalError("Real is not supported by Factory!");
    case Type_Matrix:
        return BasicPtr( new Matrix( args, shape ) );
    case Type_Neg:
        return BasicPtr( new Neg(args) );
    case Type_Add:
        return BasicPtr( new Add(args) );//->simplify();
    case Type_Mul:
        return BasicPtr( new Mul(args) );//->simplify();
    case Type_Pow:
        return BasicPtr( new Pow(args) );
    case Type_Sin:
        return BasicPtr( new Sin(args) );
    case Type_Cos:
        return BasicPtr( new Cos(args) );
    case Type_Der:
        return BasicPtr( new Der(args) );
    case Type_Element:
        return BasicPtr( new Element(args) );
    case Type_Atan:
        return BasicPtr( new Atan(args) );
    case Type_Solve:
        return BasicPtr( new Solve(args) );
    case Type_Atan2:
        return BasicPtr( new Atan2(args) );
    case Type_Abs:
        return BasicPtr( new Abs(args) );
    case Type_Acos:
        return BasicPtr( new Acos(args) );
    case Type_Asin:
        return BasicPtr( new Asin(args) );
    case Type_Scalar:
        return BasicPtr( new Scalar(args) );
    case Type_Skew:
        return BasicPtr( new Skew(args) );
    case Type_Transpose:
        return BasicPtr( new Transpose(args) );
    case Type_Less:
        return BasicPtr( new Less(args) );
    case Type_Greater:
        return BasicPtr( new Greater(args) );
    case Type_Equal:
        return BasicPtr( new Equal(args) );
    case Type_If:
        return BasicPtr( new If(args) );
    case Type_Unknown:
        throw InternalError("Unknown is not supported by Factory!");
    case Type_Tan:
        return BasicPtr( new Tan(args) );
    default:
        throw InternalError("Unknown Type: " + str(type) + "!");
    };
}
/*****************************************************************************/
