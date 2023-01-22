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