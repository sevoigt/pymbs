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

#include "PastOptimisation.h"
#include "str.h"
#include <iostream>
#include <fstream>

using namespace Symbolics;
using namespace Graph;

/*****************************************************************************/
PastOptimisation::PastOptimisation(EquationSystemPtr eqsys, NodeVec &nodes): 
m_eqsys(eqsys),m_nodes(nodes)
/*****************************************************************************/
{
  m_time = m_eqsys->getSymbol("time");
}
/*****************************************************************************/

/*****************************************************************************/
PastOptimisation::~PastOptimisation()
/*****************************************************************************/
{ 
}
/*****************************************************************************/

/*****************************************************************************/
void PastOptimisation::optimize()
/*****************************************************************************/
{
  NodeOptimizer v(m_time);

  for (size_t i=0;i<m_nodes.size();++i)
  {
    Node::node_iterate_depth_first(m_nodes[i],v);
  }
  // reset visited
  for (size_t i=0;i<m_nodes.size();++i)
  {
    m_nodes[i]->visited = false;
  }
  // remove subsed nodes
  for (size_t i=0;i<v.removed_nodes.size();++i)
  {
    m_eqsys->eraseEquation(v.removed_nodes[i]->getEqn());
  }

}
/*****************************************************************************/


/*****************************************************************************/
PastOptimisation::NodeOptimizer::NodeOptimizer(BasicPtr const& T):
time(T),removed(0),start(0)
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
PastOptimisation::NodeOptimizer::~NodeOptimizer()
/*****************************************************************************/
{}
/*****************************************************************************/

/*****************************************************************************/
void PastOptimisation::NodeOptimizer::process_Node(NodePtr p)
/*****************************************************************************/
{
  //    start = GetTickCount();
  p->simplify();
  if (!p->is_Implicit())
  {
    if(p->get_Category() < PARAMETER)
    {
      if(p->getRhsSize() == 1)
      {
        if (p->getLhs(0)->getType() != Type_Symbol)
        {
          //                    removed = removed + (GetTickCount()-start);
          return;
        }
        BasicPtr exp = p->getRhs(0);
        if (p->getSolveFor().begin()->second.kind != ALL)
          return;
        SymbolPtr symbol = p->getSolveFor().begin()->first;
        // alias
        if (exp->getType() == Type_Symbol)
        {
          subs_Node(p,symbol,exp);
          return;
        }
        if (exp->getType() == Type_Neg)
        {
          const Neg *c = Util::getAsConstPtr<Neg>(exp);
          if (c->getArg()->getType() == Type_Symbol)
          {
            subs_Node(p,symbol,exp);
            return;
          }
        }
        Basic::BasicSet atoms = exp->getAtoms();
        //double t2 = Graph::Util::getTime() - t1;
        //removed = removed>t2?removed:t2;
        // constant 
        if (atoms.size() == 0)
        {
          subs_Node(p,symbol,exp);
          return;
        }
        else if(atoms.size() == 1)
        {
          Basic::BasicSet::iterator t = atoms.find(time);
          if (t != atoms.end())
          {
            subs_Node(p,symbol,exp);
            return;
          }
        }
        // one parent
        if (symbol->getName() == "M_")
          return;
        if (symbol->getName() == "C_")
          return;
        NodePtrSet &childs = p->getChilds();
        NodePtr child;
        size_t validchild=0;
        for (NodePtrSet::iterator ii = childs.begin();ii!=childs.end();++ii)
        {
          if((*ii)->get_Category() >= PARAMETER)
          {
            validchild++;
            if ((*ii)->getChilds().size() > 0)
              child = (*ii);
          }
          else if ((*ii)->getChilds().size() > 0)
          {
            validchild++;
            child = (*ii);
          }
          if (validchild>1)
            break;
        }
        if ((validchild == 1) && (child.get() != NULL))
        {
// CS 17.04.2013 - Do not subs into skew, even if it is a one parent
			Symbolics::Graph::EquationPtr eqn = child->getEqn();
			if ((boost::operator!=(eqn, NULL)) && (eqn.get() != NULL))
			{
				Symbolics::BasicPtr rhs = eqn->getRhs(0);
				if (rhs.get() != NULL)
				{
					if (rhs->getType() == Type_Skew)
						return;
				}
			}
// CS 17.04.2013 - End

          SymbolPtrElemSizeTMap::iterator ii = child->getParentsSymbols().find(symbol);
          if (ii != child->getParentsSymbols().end())
          {
            if ((ii->second.kind == ALL) & (ii->second.counter == 1))
            {
              subs_Node(p,symbol,exp);
              return;
            }
          }
        }
      }
    }
    // alias a = b und b kann rausgeworfen werden
    if(p->getRhsSize() == 1)
    {
      if (p->getLhs(0)->getType() != Type_Symbol)
      {
        return;
      }
      BasicPtr exp = p->getRhs(0);
      if (p->getSolveFor().begin()->second.kind != ALL)
        return;
      BasicPtr symbol = p->getSolveFor().begin()->first;
      // alias
      if (exp->getType() == Type_Symbol)
      {
        SymbolPtr as = Util::getAsPtr<Symbol>(exp);
        SymbolNodeElement *elem = static_cast<SymbolNodeElement*>(as->getUserData(ID_UD_NODE));
        if (elem == NULL)
        {
          //return;
          throw InternalError("Symbol: " + as->getName() + " has no equation!");
        }
        if (elem->kind != ALL)
          return;
        NodePtr ap = elem->node;
        if(ap->get_Category() < PARAMETER)
        {
          // set b = a.exp
          p->subs(exp,ap->getRhs(0));
          // subs a with b
          NodePtrSet childs = ap->getChilds();
          subs_Node(ap,as,symbol);
          for (NodePtrSet::iterator ii = childs.begin();ii!=childs.end();++ii)
          {
            (*ii)->findParentsNodes();
          }
          return;
        }
      }
      else if (exp->getType() == Type_Neg)
      {
        Neg *c = Util::getAsPtr<Neg>(exp);
        if (c->getArg()->getType() == Type_Symbol)
        {
          const Symbol *sconst = Util::getAsConstPtr< Symbol>(c->getArg());
          Symbol *as = const_cast<Symbol*>(sconst);
          SymbolNodeElement *elem = static_cast<SymbolNodeElement*>(as->getUserData(ID_UD_NODE));
          if (elem == NULL)
          {
            //return;
            throw InternalError("Symbol: " + as->getName() + " has no equation!");
          }
          if (elem->kind != ALL)
            return;
          NodePtr ap = elem->node;
          if(ap->get_Category() < PARAMETER)
          {
            // set b = a.exp
            p->subs(exp,Neg::New(ap->getRhs(0)));
            // subs a with b
            NodePtrSet childs = ap->getChilds();
            subs_Node(ap,as,Neg::New(symbol));
            for (NodePtrSet::iterator ii = childs.begin();ii!=childs.end();++ii)
            {
              (*ii)->findParentsNodes();
            }
            return;
          }
        }
      }
    }
  }
  //    removed = removed + (GetTickCount()-start);
}
/*****************************************************************************/

/*****************************************************************************/
void PastOptimisation::NodeOptimizer::subs_Node(NodePtr p,
  SymbolPtr const& symbol,
  BasicPtr const& exp)
/*****************************************************************************/
{
  NodePtrSet::iterator ie = p->getChilds().end();
  NodePtrSet::iterator je = p->getParents().end();
  NodePtrSet::iterator js = p->getParents().begin();
  for (NodePtrSet::iterator ii=p->getChilds().begin();ii!=ie;++ii)
  {
    // subsed setzen
    (*ii)->m_subsed=true;
    (*ii)->removeParent(p);
  }
  BasicPtr bs(symbol);
  bs->subs(exp);
  p->clearChilds();
  p->subs(exp,bs);
  p->subs(p->getRhs(0),exp);
  p->findParentsNodes();

  removed_nodes.push_back(p);
  removed_symbols.push_back(symbol);
}
/*****************************************************************************/

/*****************************************************************************/
bool PastOptimisation::NodeOptimizer::process_Arg(BasicPtr const &p, bool &stop)
/*****************************************************************************/
{
  if (symbolcount > 1)
  {
    stop = true;
    return false;
  }
  if (p->getType() == Type_Symbol)
  {
    if (p == oneparentsymbol)
      symbolcount++;
  }
  if (p->getType() == Type_Solve)
  {
    symbolcount = symbolcount+2;
  }
  if (symbolcount > 1)
  {
    stop = true;
    return false;
  }
  return true;
}
/*****************************************************************************/
