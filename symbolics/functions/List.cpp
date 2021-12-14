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

#include "List.h"

using namespace Symbolics;

/*****************************************************************************/
List::List(ScopePtr scope):m_left(NULL),m_right(NULL),m_scope(scope)
/*****************************************************************************/
{
    if (scope->first==NULL)
    {
        scope->first=this;
        scope->last=this;
    }
    else
    {
        scope->last->setRight(this);
        m_left = scope->last;
        scope->last=this;
    }
}
/*****************************************************************************/

/*****************************************************************************/
List::~List()
/*****************************************************************************/
{
    if (m_left)
    {
        if (m_right)
        {
            m_left->setRight(m_right);
            m_right->setLeft(m_left);
        }
        else
        {
            m_left->setRight(NULL);
            m_scope->last = m_left;
        }
    }
    else if (m_right)
    {
        m_right->setLeft(NULL);
        m_scope->first = m_right;
    }
    else
    {
        m_scope->first = NULL;
        m_scope->last = NULL;
    }
}
/*****************************************************************************/