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
#include "If.h"
#include "Bool.h"
#include "Util.h"

using namespace Symbolics;


/*****************************************************************************/
If::If( BasicPtr const& cond, BasicPtr const& arg1, BasicPtr const& arg2): NaryOp(Type_If,BasicPtrVec())
/*****************************************************************************/
{
    // Shape berechnen
    if (arg1->getShape() != arg2->getShape())
        throw InternalError("If then exp has different shape as else exp!");

    m_shape = arg1->getShape();

    addArg(cond);
    addArg(arg1);
    addArg(arg2);
}
/*****************************************************************************/


/*****************************************************************************/
If::If( BasicPtrVec const& args ): NaryOp(Type_If, args)
/*****************************************************************************/
{
    if (args.size() != 3)
        throw InternalError("If needs 3 args If(cond,then,else)!");

    // Shape berechnen
    if (args[1]->getShape() != args[2]->getShape())
        throw InternalError("If then exp has different shape as else exp!");

    m_shape = args[1]->getShape();
}
/*****************************************************************************/


/*****************************************************************************/
If::~If()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
std::string If::toString() const
/*****************************************************************************/
{
    return "If(" + getArg(0)->toString() + " then " + getArg(1)->toString() + " else " + getArg(2)->toString() + ")";
}
/*****************************************************************************/


/*****************************************************************************/
BasicPtr If::simplify()
/*****************************************************************************/
{
    if (m_simplified)
        return BasicPtr(this);
    // call arg
    simplifyArgs();

    // check cond
    if (getArg(0)->getType() == Type_Bool)
    {
        const Bool *b = Util::getAsConstPtr<Bool>(getArg(0));
        if (b->getValue())
            return getArg(1);
        else
            return getArg(2);
    }
    m_simplified = true;
    return BasicPtr(this);
}
/*****************************************************************************/


/*****************************************************************************/
bool If::operator==(  Basic const& rhs ) const
/*****************************************************************************/
{
    // Versuch zu casten
    const If *i = dynamic_cast<const If*>(& rhs);
    if (i == NULL) return false;

    for (size_t l=0;l<3;++l)
        if (getArg(l) != i->getArg(l)) 
            return false;

    // Wenn er hier ankommt, dann sind sie wirklich gleich
    return true;
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr If::New( BasicPtr const& cond, BasicPtr const& arg1, BasicPtr const& arg2 )
/*****************************************************************************/
{
    // check cond
    if (cond->getType() == Type_Bool)
    {
        const Bool *b = Util::getAsConstPtr<Bool>(cond);
        if (b->getValue())
            return arg1;
        else
            return arg2;
    }
    return BasicPtr(new If(cond,arg1,arg2));
}
/*****************************************************************************/
