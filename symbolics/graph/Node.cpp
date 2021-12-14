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

#include <map>
#include "Node.h"
#include "str.h"

using namespace Symbolics;
using namespace Graph;

/*****************************************************************************/
Node::Node(EquationPtr eqn,SymbolPtr time):
m_time(time),m_eqn(eqn),visited(false),m_refCount(0),m_subsed(false)
/*****************************************************************************/
{
  setSolveFor(m_eqn->getSolveFor());
}
/*****************************************************************************/

/*****************************************************************************/
Node::~Node()
/*****************************************************************************/
{
  m_childs.clear();
}
/*****************************************************************************/

/*****************************************************************************/
void Node::setSolveFor(SymbolPtrElemMap& solveFor)
/*****************************************************************************/
{
  bool owndata=false;
  for (SymbolPtrElemMap::iterator ii=solveFor.begin();ii!=solveFor.end();++ii)
  {
    SymbolNodeElement *elem = static_cast<SymbolNodeElement*>(ii->first->getUserData(ID_UD_NODE));
    if (elem == NULL)
    {
      elem = new SymbolNodeElement();
      owndata = true;
    }
    if (ii->second.kind == ALL)
    {
      if (!owndata)
        throw InternalError("Symbol " + ii->first->toString() + " is solved more than once!");
      elem->kind = ALL;
      elem->node = this;
    }
    else
    {
      if (!owndata)
        if (elem->kind == ALL)
          throw InternalError("Symbol " + ii->first->toString() + " is solved more than once!");
      for (SizeTPairSet::iterator ie = ii->second.elements.begin();ie!=ii->second.elements.end();ie++)
      {
        SizeTPair pair(ie->first,ie->second);
        if (elem->elements.find(pair) != elem->elements.end())
          throw InternalError("Symbol " + ii->first->toString() + "[" + str(ie->first) + "," + str(ie->second) +"] is solved more than once!");
        elem->elements[SizeTPair(ie->first,ie->second)] = this;
      }
    }
    ii->first->setUserData(static_cast<void*>(elem),ID_UD_NODE);
  }
}
/*****************************************************************************/

/*****************************************************************************/
void Node::addParent(NodePtr p)
/*****************************************************************************/
{
  if (p.get() == NULL)
    throw InternalError("NodePointer is Null");
  m_parents.insert(p);
}
/*****************************************************************************/

/*****************************************************************************/
void Node::removeParent(NodePtr p)
/*****************************************************************************/
{
  if (p.get() == NULL)
    throw InternalError("NodePointer is Null");
  m_parents.erase(p);
  SymbolPtrElemMap &sp = p->getSolveFor();
  for (SymbolPtrElemMap::iterator ii=sp.begin();ii!=sp.end();++ii)
  {
    if (ii->second.kind == ALL)
      m_parentssymbols.erase(ii->first);
    else
    {
      for (SizeTPairSet::iterator ie = ii->second.elements.begin();ie!=ii->second.elements.end();ie++)
      {
        ii->second.elements.erase(SizeTPair(ie->first,ie->second));
      }
      if (ii->second.elements.size() == 0)
        m_parentssymbols.erase(ii->first);
    }
  }
}
/*****************************************************************************/

/*****************************************************************************/
void Node::addChild(NodePtr c)
/*****************************************************************************/
{
  if (c.get() == NULL)
    throw InternalError("NodePointer is Null");
  m_childs.insert(c);
}
/*****************************************************************************/

/*****************************************************************************/
void Node::removeChild(NodePtr c)
/*****************************************************************************/
{
  if (c.get() == NULL)
    throw InternalError("NodePointer is Null");
  m_childs.erase(c);
}
/*****************************************************************************/

/*****************************************************************************/
void Node::simplify( )
/*****************************************************************************/
{
  if (!m_subsed) return;
  m_eqn->simplify();
  findParentsNodes();
}
/*****************************************************************************/

/*****************************************************************************/
void Node::findParentsSymbols()
/*****************************************************************************/
{
  m_parentssymbols.clear();
  m_eqn->findSymbols();
  SymbolPtrElemSizeTMap& symbols = m_eqn->getSymbols();
  SymbolPtrElemMap& solveFor = m_eqn->getSolveFor();
  /*
    remove from eqn symbols the symbols the eqn solves. 
    Example:
    {a[1],b[2],c} = f(a,b[2],b[3],c)
  */
  for(SymbolPtrElemSizeTMap::iterator ii=symbols.begin();ii!=symbols.end();++ii)
  {
    bool insolveFor = false;
    SymbolPtrElemMap::iterator ik = solveFor.find(ii->first);
    if (ik != solveFor.end())
    {
      insolveFor = true;
      if (ik->second.kind != ALL)
      {
        SymbolElementSizeT em;
        em.counter = 0;
        em.kind = SEVERAL;
        for (SizeTPairSizeTMap::iterator ie = ii->second.elements.begin();ie!=ii->second.elements.end();ie++)
        {
          SizeTPair pair(ie->first.first,ie->first.second);
          if (ik->second.elements.find(pair) == ik->second.elements.end())
            em.elements[pair] = ie->second;
        }
        if (em.elements.size() > 0)
        {
          m_parentssymbols[ii->first] = em;
        }
      }
    }
    if (!insolveFor)
    {
      const Symbol *s = Util::getAsConstPtr<Symbol>((*ii).first);
      m_parentssymbols[ii->first] = ii->second;
    }
  }
}
/*****************************************************************************/

/*****************************************************************************/
void Node::findParentsNodes()
/*****************************************************************************/
{
  findParentsSymbols();
  m_parents.clear();
  SymbolPtrElemSizeTMap::iterator je = m_parentssymbols.end();
  for (SymbolPtrElemSizeTMap::iterator jj=m_parentssymbols.begin();jj!=je;++jj)
  {
    // exclude time
    if (jj->first == m_time)
      continue;
    // exclude states, inputs and controller from parents
    if ((jj->first->getKind() & INPUT))
      continue;
    if ((jj->first->getKind() & CONTROLLER))
      continue;
    if (jj->first->stateKind() == ALL)
    {
      if (jj->first->is_State(0,0) & STATE)
        continue;
    }
    SymbolNodeElement *elem = static_cast<SymbolNodeElement*>(jj->first->getUserData(ID_UD_NODE));
    if (elem == NULL)
    {
      //continue;
      if (jj->first->stateKind() == SEVERAL)
      {
        size_t dim1=jj->first->getShape().getDimension(1);
        size_t dim2=jj->first->getShape().getDimension(2);
        bool nostate=false;
        for(size_t i=0;i<dim1;i++)
        {
          for(size_t j=0;j<dim2;j++)
          {
            if (!(jj->first->is_State(i,j) & STATE))
            {
              nostate=true;
              break;
            }              
          }
          if (nostate)
            break;
        }
        if (!nostate)
        {
          continue;
        }
      }
      throw InternalError("Symbol: " + (*jj).first->getName() + " has no equation!");
    }
    if (elem->kind == ALL)
    {
      addParent(elem->node);
      elem->node->addChild(NodePtr(this));
    }
    else
    {
      if (jj->second.kind == ALL)
      {
        for (SizeTPairNodePtrMap::iterator ie = elem->elements.begin();ie!=elem->elements.end();ie++)
        {
          if (jj->first->stateKind() == SEVERAL)
            if (jj->first->is_State(ie->first.first,ie->first.second) & STATE)
              continue;
          addParent(ie->second);
          ie->second->addChild(NodePtr(this));
        }
      }
      else
      {
        for (SizeTPairNodePtrMap::iterator ie = elem->elements.begin();ie!=elem->elements.end();ie++)
        {
          if (jj->first->stateKind() == SEVERAL)
            if (jj->first->is_State(ie->first.first,ie->first.second) & STATE)
              continue;
          if (jj->second.elements.find(ie->first) == jj->second.elements.end())
            continue;
          addParent(ie->second);
          ie->second->addChild(NodePtr(this));
        }
      }
    }
  }
}
/*****************************************************************************/

/*****************************************************************************/
void Node::clear()
/*****************************************************************************/
{
  for (SymbolPtrElemMap::iterator ii=m_eqn->getSolveFor().begin();ii!=m_eqn->getSolveFor().end();++ii)
  {
    void *nodeelement = ii->first->getUserData(ID_UD_NODE);
    if (nodeelement)
    {
      delete nodeelement;
      ii->first->setUserData(NULL,ID_UD_NODE);
    }
  }
  m_parentssymbols.clear();
  m_parents.clear();
  m_childs.clear();
}
/*****************************************************************************/

/*****************************************************************************/
void Node::subs(ConstBasicPtr const& old_exp, BasicPtr const& new_exp)
/*****************************************************************************/
{
  m_subsed = true;
  m_eqn->subs(old_exp,new_exp);
}
/*****************************************************************************/

/*****************************************************************************/
void Node::node_iterate_depth_first(NodePtr p, Visitor &v)
/*****************************************************************************/
{
  if (p->visited)
    return;
  p->visited = true;
  NodePtrSet parents = p->getParents();
  for (NodePtrSet::const_iterator ii =parents.begin(); ii != parents.end() ;++ii)
  {
    node_iterate_depth_first((*ii),v);
  }
  v.process_Node(p);
}
/*****************************************************************************/
