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

#include "UnMatchedSystem.h"
#include "str.h"
#include <iostream>
#include <fstream>

using namespace Symbolics;
using namespace Graph;

/*****************************************************************************/
UnMatchedSystem::UnMatchedSystem(EquationSystemPtr eqsys, NodeVec& nodes):
SystemHandler(eqsys,nodes)
/*****************************************************************************/
{
  m_time = m_eqsys->getSymbol("time");

  // Incidence Matrix
  EquationSystem::StringSymbolMap symbolmap = m_eqsys->getSymbols();
  // fill symbols in incidencematrix
  for (EquationSystem::StringSymbolMap::iterator ii =symbolmap.begin(); ii != symbolmap.end();++ii)
  {
    addSymbolToIncidenceMatrix(ii->second);
  }
  SymbolPtrSet symbolset;
  SymbolRepl repl(symbolreplacemap);
  const EquationPtrSet &eqns = m_eqsys->getEquations();
  for (EquationPtrSet::const_iterator i = eqns.begin();i != eqns.end(); i++)
  {
    bool parameter = false;
    EquationPtr e = *i;
    SymbolPtrElemMap& solveFor = e->getSolveFor();
    for (SymbolPtrElemMap::iterator j=solveFor.begin();j!=solveFor.end();j++)
    {
      if (j->first->getKind() & PARAMETER)
      {
        parameter = true;
      }
    }
    if (parameter)
    {
      NodePtr node = new Node(e,m_time);
      m_nodes.push_back(node);
    }
    else
    {
      MEquationPtrVec meqns;
      for (size_t m=0;m<e->getLhsSize();m++)
      {
        BasicPtr lhs = e->getLhs(m)->iterateExp(repl);
        BasicPtr rhs = e->getRhs(m)->iterateExp(repl);
        if (rhs->getType() == Type_Solve)
        {
          lhs = Mul::New(rhs->getArg(0),lhs);
          rhs = rhs->getArg(1);
        }
        if (e->getLhs(m)->is_Scalar())
        {
          MEquation* eq = new MEquation(e,0,0);
          eq->lhs = lhs->simplify();
          eq->rhs = rhs->simplify();
          SymbolScanner scan(eq,m_time,m_incidencematrixT);
          eq->lhs->scanExp(scan);
          eq->rhs->scanExp(scan);
          m_equations.push_back(eq);
          meqns.push_back(eq);
        }
        else
        {
          size_t dim1 = e->getLhs(m)->getShape().getDimension(1);
          size_t dim2 = e->getLhs(m)->getShape().getDimension(2);
          for (size_t k=0;k<dim1;k++)
            for (size_t l=0;l<dim2;l++)
            {
              MEquation* eq = new MEquation(e,k,l);
              eq->lhs = Element::New(lhs,k,l)->simplify();
              eq->rhs = Element::New(rhs,k,l)->simplify();
              SymbolScanner scan(eq,m_time,m_incidencematrixT);
              eq->lhs->scanExp(scan);
              eq->rhs->scanExp(scan);
              m_equations.push_back(eq);
              meqns.push_back(eq);
            }
        }
        m_eqnptrmeqns[e] = meqns;
      }
    }
  }
}
/*****************************************************************************/

/*****************************************************************************/
UnMatchedSystem::~UnMatchedSystem()
/*****************************************************************************/
{ 
  for (size_t i=0;i<m_equations.size();i++)
  {
    delete m_equations[i];
  }
  for(IncidenceMatrixT::iterator ii=m_incidencematrixT.begin();ii!=m_incidencematrixT.end();++ii)
  {
    delete ii->first;
  }
}
/*****************************************************************************/

/*****************************************************************************/
bool UnMatchedSystem::SymbolScanner::process_Arg(BasicPtr const &p, bool &stop)
/*****************************************************************************/
{
  if (p->getType() == Type_Der)
  {
    if (inder)
      throw InternalError("Error: der(der(exp)) is not allowed!");
    inder = true;
    p->getArg(0)->scanExp(*this);
    inder = false;
    return false;
  }
  if (p->getType() == Type_Symbol)
  {
    Symbol *s = Util::getAsPtr<Symbol>(p);
    if (m_time == static_cast<Basic*>(s))
      return true;
    if (inder)
    {
      Symbol *ds = static_cast<Symbol*>(s->getUserData(ID_UD_STATEDERIVATIVE));
      if (ds != NULL)
      {
        s = ds;
      }
    }
    if (!((s->getKind() & INPUT) || (s->getKind() & CONSTANT) || (s->getKind() & CONTROLLER)))
    {
      if (s->is_Scalar())
      {
        MSymbol *symbol = static_cast<MSymbol*>(s->getUserData(ID_UD_SYMBOL));
        if (symbol == NULL)
          throw InternalError("Symbol " + s->getName() + " without MSymbol");
        m_incidencematrixT[symbol].push_back(m_equation);
        if (symbol->state)
          m_equation->states.insert(symbol);
        else
          m_equation->symbols.insert(symbol);
      }
      else
      {
        MSymbolPtrVec *syms = static_cast<MSymbolPtrVec*>(s->getUserData(ID_UD_SYMBOL));
        if (syms == NULL)
          throw InternalError("Symbol " + s->getName() + " without MSymbols");
        for(size_t u=0;u<syms->size();u++)
        {
          m_incidencematrixT[(*syms)[u]].push_back(m_equation);
          if ((*syms)[u]->state)
            m_equation->states.insert((*syms)[u]);
          else
            m_equation->symbols.insert((*syms)[u]);
        }
      }
    }
  }
  else if (p->getType() == Type_Element)
  {
    if (p->getArg(0)->getType() == Type_Symbol)
    {
      Symbol *s = Util::getAsPtr<Symbol>(p->getArg(0));
      if (m_time == static_cast<Basic*>(s))
        return true;
      if (inder)
      {
        Symbol *ds = static_cast<Symbol*>(s->getUserData(ID_UD_STATEDERIVATIVE));
        if (ds != NULL)
        {
          s = ds;
        }
      }
      if (!((s->getKind() & INPUT) || (s->getKind() & CONSTANT) || (s->getKind() & CONTROLLER)))
      {
        if (s->is_Scalar())
        {
          MSymbol *symbol = static_cast<MSymbol*>(s->getUserData(ID_UD_SYMBOL));
          if (symbol == NULL)
            throw InternalError("Symbol " + s->getName() + " without MSymbol");
          m_incidencematrixT[symbol].push_back(m_equation);
          if (symbol->state)
            m_equation->states.insert(symbol);
          else
            m_equation->symbols.insert(symbol);
        }
        else
        {
          const Int *iconst1 = Util::getAsConstPtr<Int>(p->getArg(1));
          const Int *iconst2 = Util::getAsConstPtr<Int>(p->getArg(2));
          size_t dim1 = iconst1->getValue();
          size_t dim2 = iconst2->getValue();
          MSymbolPtrVec *syms = static_cast<MSymbolPtrVec*>(s->getUserData(ID_UD_SYMBOL));
          if (syms == NULL)
            throw InternalError("Symbol " + s->getName() + " without MSymbols");
          size_t sdim2 = s->getShape().getDimension(2);
          size_t u = sdim2*dim1 + dim2;
          m_incidencematrixT[(*syms)[u]].push_back(m_equation);
          if ((*syms)[u]->state)
            m_equation->states.insert((*syms)[u]);
          else
            m_equation->symbols.insert((*syms)[u]);
        }
      }
      return false;
    }
    else
      throw InternalError("Element " + p->toString());
  }
  return true;
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr UnMatchedSystem::SymbolRepl::process_Arg(BasicPtr const &p)
/*****************************************************************************/
{
  if (p->getType() == Type_Symbol)
  {
    BasicPtrMap::iterator ii = m_symbolmap.find(p);
    if (ii != m_symbolmap.end())
      return ii->second;
  }
  return p;
}
/*****************************************************************************/

/*****************************************************************************/
void UnMatchedSystem::addSymbolToIncidenceMatrix(SymbolPtr s)
/*****************************************************************************/
{
  if (!((s->getKind() & INPUT) || (s->getKind() & CONSTANT) || (s->getKind() & CONTROLLER) || (m_time==s)))
  {
    bool state = false;
    if (s->stateKind() == ALL)
    {
      if (s->is_State(0,0) & STATE)
        state = true;
    }
    bool parameter = false;
    if (s->getKind() == PARAMETER)
        parameter = true;
    if (s->is_Scalar())
    {
      MSymbol *symbol = new MSymbol(s,0,0);
      s->setUserData(static_cast<void*>(symbol),ID_UD_SYMBOL);
      if (state)
        m_states.insert(symbol);
      if (parameter)
        m_parameter.insert(symbol);
      symbol->state = state;
      symbol->parameter = parameter;
      m_incidencematrixT[symbol] = MEquationPtrVec();
    }
    else
    {
      MSymbolPtrVec *symbols = new MSymbolPtrVec();
      size_t dim1 = s->getShape().getDimension(1);
      size_t dim2 = s->getShape().getDimension(2);
      for (size_t i=0;i<dim1;i++)
        for (size_t j=0;j<dim2;j++)
        {
          MSymbol *symbol = new MSymbol(s,i,j);
          if (s->is_State(i,j) & STATE)
          {
            m_states.insert(symbol);
            symbol->state = true;
          }
          else
            symbol->state = false;
          symbol->parameter = parameter;
          m_incidencematrixT[symbol] = MEquationPtrVec();
          symbols->push_back(symbol);
        }
      s->setUserData(static_cast<void*>(symbols),ID_UD_SYMBOL);
    }
  }
  if (s->is_Scalar())
  {
    symbolreplacemap[s] = s;
  }
  else
  {
    size_t dim1 = s->getShape().getDimension(1);
    size_t dim2 = s->getShape().getDimension(2);
    Matrix *mat = new Matrix(s->getShape());
    for(size_t i=0;i<dim1;++i)
    {
      for(size_t j=0;j<dim2;++j)
      {
        mat->set(i,j, Element::New(s,i,j));
      }
    }
    symbolreplacemap[s] = BasicPtr(mat);
  }
}
/*****************************************************************************/

/*****************************************************************************/
void UnMatchedSystem::addEquationToIncidenceMatrix(MEquation* wrapper)
/*****************************************************************************/
{
  SymbolRepl repl(symbolreplacemap);
  EquationPtr e = wrapper->eqn;
  SymbolPtrElemSizeTMap &symbols = e->getSymbols();
  for (SymbolPtrElemSizeTMap::iterator m=symbols.begin();m!=symbols.end();m++)
  {
    if (!((m->first->getKind() & INPUT) || (m->first->getKind() & CONSTANT) || (m->first->getKind() & CONTROLLER) || (m_time==m->first)))
    {
      if (m->first->is_Scalar())
      {
        MSymbol *symbol = static_cast<MSymbol*>(m->first->getUserData(ID_UD_SYMBOL));
        if (symbol == NULL)
          throw InternalError("Symbol " + m->first->getName() + " without MSymbol");
        m_incidencematrixT[symbol].push_back(wrapper);
        if (symbol->state)
          wrapper->states.insert(symbol);
        else
          wrapper->symbols.insert(symbol);
      }
      else
      {
        MSymbolPtrVec *syms = static_cast<MSymbolPtrVec*>(m->first->getUserData(ID_UD_SYMBOL));
        if (syms == NULL)
          throw InternalError("Symbol " + m->first->getName() + " without MSymbols");
        for(size_t u=0;u<syms->size();u++)
        {
          m_incidencematrixT[(*syms)[u]].push_back(wrapper);
          if ((*syms)[u]->state)
            wrapper->states.insert((*syms)[u]);
          else
            wrapper->symbols.insert((*syms)[u]);
        }
      }
    }
  }
}
/*****************************************************************************/

/*****************************************************************************/
double UnMatchedSystem::buildGraph()
/*****************************************************************************/
{
  double t0 = Util::getTime();
  double t1 = matchSystem();
  double t2 = Util::getTime();
  buildNodes();
  double t3 = Util::getTime();
  return t1-t0;
}
/*****************************************************************************/


/*****************************************************************************/
void UnMatchedSystem::buildNodes()
/*****************************************************************************/
{
  // generate nodes
  for (size_t e=0;e<m_comps.size();++e)
  {
    if (m_comps[e].size() == 1)
    {
      if (m_comps[e][0]->eqn->getNumEl() != 1)
      {
        /*
        - erase equation from equation system
        - use scalarized equations from MEquations
        - update all mequations of the equation with the scalar versions
        */
        EquationPtr eqn = m_comps[e][0]->eqn;
        m_eqsys->eraseEquation(eqn);
        EquationPtrMEquationMap::iterator imeq =  m_eqnptrmeqns.find(eqn);
        if (imeq == m_eqnptrmeqns.end())
          throw InternalError("Equation withoud MEquations");
        for (size_t i=0;i<imeq->second.size();i++)
        {
          EquationPtr seqn(new Equation(SymbolPtrElemMap(),imeq->second[i]->lhs,imeq->second[i]->rhs,true));
          m_eqsys->addEquation(seqn);
          imeq->second[i]->eqn = seqn;
          MEquationPtrVec mevec;
          mevec.push_back(imeq->second[i]);
          m_eqnptrmeqns[seqn] = mevec;
        }
        m_eqnptrmeqns.erase(eqn);
      }
      if (m_comps[e][0]->symbol->symbol->is_Scalar())
      {
        // scalar case
        m_comps[e][0]->eqn->solve(m_comps[e][0]->symbol->symbol);
      }
      else
      {
        m_comps[e][0]->eqn->solve(Element::New(m_comps[e][0]->symbol->symbol,m_comps[e][0]->symbol->dim1,m_comps[e][0]->symbol->dim2));
      }
      NodePtr node = new Node(m_comps[e][0]->eqn,m_time);
      m_nodes.push_back(node);
    }
    else
    {
      /*
      - collect equations
      - if necesary split a equation
      - do not split a equation if it is solved compled
      - combine equations
      */
      EquationPtrMEquationSetMap eqnchecker;
      EquationPtrVec eqns;
      for(size_t i=0;i<m_comps[e].size();i++)
      {
        if (m_comps[e][i]->eqn->getNumEl() == 1)
        {
          eqns.push_back(m_comps[e][i]->eqn);
          if (m_comps[e][i]->symbol->symbol->is_Scalar())
          {
            // scalar case
            m_comps[e][i]->eqn->solve(m_comps[e][i]->symbol->symbol);
          }
          else
          {
            m_comps[e][i]->eqn->solve(Element::New(m_comps[e][i]->symbol->symbol,m_comps[e][i]->symbol->dim1,m_comps[e][i]->symbol->dim2));
          }
        }
        else
        {
          EquationPtrMEquationSetMap::iterator ie = eqnchecker.find(m_comps[e][i]->eqn);
          if (ie != eqnchecker.end())
          {
            std::pair<MEquationPtrSet::iterator,bool> res = ie->second.insert(m_comps[e][i]);
            if (!res.second)
              throw InternalError("Part of the Equation is solved twice!");
          }
          else
          {
            MEquationPtrSet s;
            s.insert(m_comps[e][i]);
            eqnchecker[m_comps[e][i]->eqn] = s;
          }
        }
      }
      for(EquationPtrMEquationSetMap::iterator ii=eqnchecker.begin();ii!=eqnchecker.end();ii++)
      {
        if (ii->second.size() == ii->first->getNumEl())
        {
          MEquationPtrSet::iterator im = ii->second.begin();
          bool onesymbol=true;
          for (MEquationPtrSet::iterator in=ii->second.begin();in!=ii->second.end();in++)
          {
            if ((*in)->symbol->symbol != (*im)->symbol->symbol)
            {
              onesymbol = false;
              break;
            }
          }
          if (onesymbol)
          {
            /* full multidim equation */
            ii->first->solve((*im)->symbol->symbol);
            eqns.push_back(ii->first);
            continue;
          }
        }
        /*
          split Equation
          - erase equation from equation system
          - use scalarized equations from MEquations
          - update all mequations of the equation with the scalar versions
        */
        EquationPtr eqn = ii->first;
        m_eqsys->eraseEquation(eqn);
        EquationPtrMEquationMap::iterator imeq =  m_eqnptrmeqns.find(eqn);
        if (imeq == m_eqnptrmeqns.end())
          throw InternalError("Equation withoud MEquations");
        for (size_t i=0;i<imeq->second.size();i++)
        {
          EquationPtr seqn(new Equation(SymbolPtrElemMap(),imeq->second[i]->lhs,imeq->second[i]->rhs,true));
          m_eqsys->addEquation(seqn);
          imeq->second[i]->eqn = seqn;
          MEquationPtrVec mevec;
          mevec.push_back(imeq->second[i]);
          m_eqnptrmeqns[seqn] = mevec;
        }
        m_eqnptrmeqns.erase(eqn);
        for (MEquationPtrSet::iterator in=ii->second.begin();in!=ii->second.end();in++)
        {
          if ((*in)->symbol->symbol->is_Scalar())
            (*in)->eqn->solve((*in)->symbol->symbol);
          else
            (*in)->eqn->solve(Element::New((*in)->symbol->symbol,(*in)->symbol->dim1,(*in)->symbol->dim2));
          eqns.push_back((*in)->eqn);
        }
      }
      EquationPtr eqn;
      if (eqns.size() > 1)
        eqn = m_eqsys->combineEquations(eqns);
      else
        eqn = eqns[0];
      NodePtr node = new Node(eqn,m_time);
      m_nodes.push_back(node);
    }
  }

  // find parents nodes
  for (size_t i=0;i<m_nodes.size();++i)
  {
    m_nodes[i]->findParentsNodes();
  }
}
/*****************************************************************************/

/*****************************************************************************/
double UnMatchedSystem::matchSystem()
/*****************************************************************************/
{
  bool debugmode=false;
  if (debugmode)
    toGraphML("d:/Temp/PendulumDAE.graphml");
  // check size
  if (m_equations.size() != (m_incidencematrixT.size() - m_states.size() - m_parameter.size()))
  {
    toGraphML("IncidenceMatrix.graphml");
    return -1;
  }
  //
  if (debugmode)
    m_eqsys->dumpSystem("D:/Temp/model.mo");
  double t1 = Util::getTime();
  EquationPtrMap eqndiffeqnmap;
  EquationPtrSizeTSet deqnchecker;
  SymbolPtrSizeTSet dummysymbols;
  SymbolPtrSet dummy_der;
  SizeTSet diffeqns;
  DerRepl dr;
  Equation::DerRepl drder;
  size_t fname=1;
  SymbolRepl repl(symbolreplacemap);
  // for node in nodes
  for (size_t i=0;i<m_equations.size();++i)
  {
    SizeTSet eqn_marks;
    MSymbolPtrSet sym_marks;
    if (m_equations[i]->symbol != NULL)
    {
      m_equations[i]->symbol->eqn = i+1;
      continue;
    }
    //double t5 = Util::getTime();
    if (!pathFound(i,eqn_marks,sym_marks,m_equations[i]->c))
    {
      // reduce index or make eqn scalar
      MSymbolPtrSet states;
      // get marged equations (eqn_marks)
      for (SizeTSet::iterator jj=eqn_marks.begin();jj!=eqn_marks.end();jj++)
      {
        // states in eqn
        for (MSymbolPtrSet::iterator s=m_equations[*jj]->states.begin();s!= m_equations[*jj]->states.end();s++)
          states.insert(*s);
        if (diffeqns.find(*jj) == diffeqns.end())
        {
          // differentiate equation
          BasicPtr dlhs = m_equations[*jj]->lhs->der()->iterateExp(dr)->simplify();
          BasicPtr drhs = m_equations[*jj]->rhs->der()->iterateExp(dr)->simplify();
          EquationPtr deq(new Equation(SymbolPtrElemMap(),dlhs,drhs,false));
          if (dr.newsyms.size() > 0)
          {
            // add dummy_der symbols
            for (SymbolPtrSet::iterator kk = dr.newsyms.begin();kk!=dr.newsyms.end();kk++)
            {
              addSymbolToIncidenceMatrix(*kk);
              dummy_der.insert(*kk);
            }
            dr.newsyms.clear();
          }
          MEquation* eq = new MEquation(deq,0,0);
          // add diff eqn
          eq->lhs = deq->getLhs(0)->iterateExp(repl)->simplify();
          eq->rhs = deq->getRhs(0)->iterateExp(repl)->simplify();
          SymbolScanner scan(eq,m_time,m_incidencematrixT);
          eq->lhs->scanExp(scan);
          eq->rhs->scanExp(scan);
          m_equations.push_back(eq);
          deqnchecker[deq].insert(SizeTPair(eq->dim1,eq->dim2));
          diffeqns.insert(*jj);
        }
      }
      if (states.size() == 0)
      {
        std::string eqnsstr;
        for (SizeTSet::iterator jj=diffeqns.begin();jj!=diffeqns.end();jj++)
        {
          eqnsstr += m_equations[*jj]->eqn->toString() + "[" + str(m_equations[*jj]->dim1) + "," + str(m_equations[*jj]->dim2)+ "]/n";
        }
        throw InternalError("No States to select in Eqns " + eqnsstr);
      }
      // select dummy state
      SymbolEqnMap dummystates;
      for (MSymbolPtrSet::iterator ii=states.begin();ii!=states.end();ii++)
      {
        SymbolPtrSizeTSet::iterator jk = dummysymbols.find((*ii)->symbol);
        if (jk != dummysymbols.end())
        {
          dummystates[(*ii)] = jk->second.size();
        }
        else
        {
          dummystates[(*ii)] = 0;
        }
      }
      SymbolEqnMap::iterator ik=dummystates.begin();
      MSymbolPtr dummystate = ik->first;
      for (SymbolEqnMap::iterator ii=++dummystates.begin();ii!=dummystates.end();ii++)
      {
        if (ik->second > ii->second)
          dummystate = ii->first;
      }
      // add new dummy der var
      SymbolPtrSizeTSet::iterator jk = dummysymbols.find(dummystate->symbol);
      if (jk != dummysymbols.end())
        jk->second.insert(SizeTPair(dummystate->dim1,dummystate->dim2));
      else
      {
        SizeTPairSet dim;
        dim.insert(SizeTPair(dummystate->dim1,dummystate->dim2));
        dummysymbols[dummystate->symbol] = dim;
      }
      dummystate->state = false;
      m_eqsys->setVariable(dummystate->symbol,dummystate->dim1,dummystate->dim2);
      std::map<SymbolPtr, DerRepl::BasicPtrKindPair >::iterator ds = dr.dummy_der.find(dummystate->symbol);
      if (ds == dr.dummy_der.end())
      {
        SymbolPtr ders(dummystate->symbol->getDerivative());
        if (dummystate->symbol->is_Scalar())
        {
            dr.dummy_der[dummystate->symbol] = DerRepl::BasicPtrKindPair(ders,ALL); 
        }
        else
        {
            size_t dim1 = dummystate->symbol->getShape().getDimension(1);
            size_t dim2 = dummystate->symbol->getShape().getDimension(2);
            Matrix *mat = new Matrix(dummystate->symbol->getShape());
            mat->set(dummystate->dim1,dummystate->dim2, ders);
            dr.dummy_der[dummystate->symbol] = DerRepl::BasicPtrKindPair(BasicPtr(mat),SEVERAL);
        }
      }
      else
      {
        // update replacement map
        if (!ds->first->is_Scalar())
        {
          SymbolPtr ders = dummystate->symbol->getDerivative();
          Matrix *mat = Util::getAsPtr<Matrix>(ds->second.first);
          if (mat->get(dummystate->dim1,dummystate->dim2) != Zero::getZero())
          {
            mat->set(dummystate->dim1,dummystate->dim2,Element::New(ders,dummystate->dim1,dummystate->dim2));
          }
        }
      }
      m_states.erase(dummystate);
      MEquationPtrVec &dummystateeqns = m_incidencematrixT[dummystate];
      for (MEquationPtrVec::iterator ik=dummystateeqns.begin();ik!=dummystateeqns.end();ik++)
      {
        (*ik)->states.erase(dummystate);
        (*ik)->symbols.insert(dummystate);
      }
      i--;
      if (debugmode)
        toGraphML("d:/Temp/PendulumDAE" + str(fname) + ".graphml");
      fname++;
    }
    //m_equations[i]->t = Util::getTime() - t5;
  }
  if (debugmode)
  {
    std::ofstream f;
    f.open("d:/Temp/incidence.txt");
    for (IncidenceMatrixT::iterator ii = m_incidencematrixT.begin();ii!=m_incidencematrixT.end();ii++)
    {
      f << ii->first->symbol->toString() << "[" << ii->first->dim1 << ", " << ii->first->dim2 << "] " << ii->first->state  << " : " << ii->first->eqn << std::endl;
    }
    f.close();
  }
  // add symbols and equations to equation system
  for (SymbolPtrSet::iterator ii=dummy_der.begin();ii!=dummy_der.end();ii++)
  {
    // check if all wraper have equations
    if ((*ii)->is_Scalar())
    {
      MSymbol *symbol = static_cast<MSymbol*>((*ii)->getUserData(ID_UD_SYMBOL));
      if (symbol == NULL)
        throw InternalError("Symbol " + (*ii)->getName() + " without MSymbol");
      IncidenceMatrixT::iterator kk = m_incidencematrixT.find(symbol);
      if (kk == m_incidencematrixT.end())
        throw InternalError("Symbol " + (*ii)->getName() + " not in  IncidenceMatrixT");
      if (kk->second.size() != 0)
        m_eqsys->addSymbol(*ii);
    }
    else
    {
      BasicPtrVec zerosymbols;
      MSymbolPtrVec *syms = static_cast<MSymbolPtrVec*>((*ii)->getUserData(ID_UD_SYMBOL));
      if (syms == NULL)
        throw InternalError("Symbol " + (*ii)->getName() + " without MSymbols");
      for (size_t k=0;k<syms->size();k++)
      {
        IncidenceMatrixT::iterator kk = m_incidencematrixT.find((*syms)[k]);
        if (kk == m_incidencematrixT.end())
          throw InternalError("Symbol " + (*ii)->getName() + " not in  IncidenceMatrixT");
        if (kk->second.size() == 0)
        {
          zerosymbols.push_back(Element::New((*syms)[k]->symbol,(*syms)[k]->dim1,(*syms)[k]->dim2));
        }
      }
      if (zerosymbols.size() > 0)
      {
        m_eqsys->addSymbol(*ii);
        if (zerosymbols.size() == 1)
          m_eqsys->addEquation(BasicPtr(zerosymbols[0]),Zero::getZero());
        else
        m_eqsys->addEquation(BasicPtr(new Matrix(zerosymbols,Shape(zerosymbols.size()))),Zero::getZero(Shape(zerosymbols.size())));
      }
    }
  }
  for (EquationPtrSizeTSet::iterator ii=deqnchecker.begin();ii!=deqnchecker.end();ii++)
  {
    if (ii->first->getNumEl() == ii->second.size())
    {
      m_eqsys->addEquation(ii->first);
    }
    else
      throw InternalError("Cannot add Equation to EquationSystem because not all Elements needed! " + ii->first->toString());
  }
  for (SymbolPtrSizeTSet::iterator ii=dummysymbols.begin();ii!=dummysymbols.end();ii++)
  {
    if (ii->first->is_Scalar())
    {
      MSymbol *symbol = static_cast<MSymbol*>(ii->first->getUserData(ID_UD_SYMBOL));
      MEquationPtrVec eqns = m_incidencematrixT[symbol];
      for (size_t i=0;i<eqns.size();i++)
      {
        eqns[i]->eqn->findSymbols();
      }
      Symbol *ds = ii->first->getDerivative();
      symbol = static_cast<MSymbol*>(ds->getUserData(ID_UD_SYMBOL));
      eqns = m_incidencematrixT[symbol];
      for (size_t i=0;i<eqns.size();i++)
      {
        eqns[i]->eqn->findSymbols();
      }
    }
    else
    {
      MSymbolPtrVec *symbols = static_cast<MSymbolPtrVec*>(ii->first->getUserData(ID_UD_SYMBOL));
      for (size_t l=0;l<symbols->size();l++)
      {
        MEquationPtrVec eqns = m_incidencematrixT[(*symbols)[l]];
        for (size_t i=0;i<eqns.size();i++)
        {
          eqns[i]->eqn->findSymbols();
        }
      }
      Symbol *ds = ii->first->getDerivative();
      symbols = static_cast<MSymbolPtrVec*>(ds->getUserData(ID_UD_SYMBOL));
      for (size_t l=0;l<symbols->size();l++)
      {
        MEquationPtrVec eqns = m_incidencematrixT[(*symbols)[l]];
        for (size_t i=0;i<eqns.size();i++)
        {
          eqns[i]->eqn->findSymbols();
        }
      }
    }
  }

  // build nodes from matching
  typedef std::pair< MSymbolPtrSet,MEquationPtrSet> TwoSizeTPairSet;
  typedef std::map<SymbolPtr, TwoSizeTPairSet > SymbolPtr2SizeTPairSetMap;
  typedef std::map<EquationPtr, SymbolPtr2SizeTPairSetMap> EquaionSymbol2SizeTPairSet;
  EquaionSymbol2SizeTPairSet eqsimchecker;
  for (size_t i=0;i<m_equations.size();++i)
  {
    EquaionSymbol2SizeTPairSet::iterator ie = eqsimchecker.find(m_equations[i]->eqn);
    if (ie != eqsimchecker.end())
    {
      SymbolPtr2SizeTPairSetMap::iterator is = ie->second.find(m_equations[i]->symbol->symbol);
      if (is != ie->second.end())
      {
        is->second.first.insert(m_equations[i]->symbol);
        is->second.second.insert(m_equations[i]);
      }
      else
      {
        MSymbolPtrSet sets;
        MEquationPtrSet sete;
        sets.insert(m_equations[i]->symbol);
        sete.insert(m_equations[i]);
        ie->second[m_equations[i]->symbol->symbol] = TwoSizeTPairSet(sets,sete);
      }
    }
    else
    {
      MSymbolPtrSet sets;
      MEquationPtrSet sete;
      sets.insert(m_equations[i]->symbol);
      sete.insert(m_equations[i]);
      SymbolPtr2SizeTPairSetMap map;
      map[m_equations[i]->symbol->symbol] = TwoSizeTPairSet(sets,sete);
      eqsimchecker[m_equations[i]->eqn] = map;
    }
  }
  for (EquaionSymbol2SizeTPairSet::iterator ii = eqsimchecker.begin();ii!=eqsimchecker.end();ii++)
  {
    if ((ii->second.size() == 1) && (ii->second.begin()->first->getShape().getNumEl() == ii->first->getNumEl()))
    {
      ii->first->solve(ii->second.begin()->first);
    }
    else
    {
      m_eqsys->eraseEquation(ii->first);
      m_eqnptrmeqns.erase(ii->first);
      for (SymbolPtr2SizeTPairSetMap::iterator is=ii->second.begin();is!=ii->second.end();is++)
      {
        for (MEquationPtrSet::iterator ise=is->second.second.begin();ise!=is->second.second.end();ise++)
        {
          EquationPtr e(new Equation(SymbolPtrElemMap(),(*ise)->lhs,(*ise)->rhs,false));
          m_eqsys->addEquation(e);
          e->solve(Element::New((*ise)->symbol->symbol,(*ise)->symbol->dim1,(*ise)->symbol->dim2));
          (*ise)->eqn = e;
          MEquationPtrVec mevec;
          mevec.push_back(*ise);
          m_eqnptrmeqns[e] = mevec;
        }
      }
    }
  }
 
  double t2 = Util::getTime();
  if (debugmode)
    toGraphML("d:/Temp/PendulumDAE_matched.graphml");
  // sort equations
  MEquationPtrVec stack;
  m_comps.clear();
  size_t i = 0;
  for (size_t e=0;e<m_equations.size();++e)
  {
    if(m_equations[e]->number == 0)
    {
      strongConnect(i,m_equations[e],stack);
    }
  }
  if (debugmode)
  {
    toGraphML_Sorted("d:/Temp/PendulumDAE_sorted.graphml");
    m_eqsys->dumpSystem("D:/Temp/model_sorted.mo");
    dumpComps("D:/Temp/model_comps.txt");
  }
  return Util::getTime() - t2;
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr UnMatchedSystem::DerRepl::process_Arg(BasicPtr const &p)
/*****************************************************************************/
{
  /*
  der(state) = der_state
  der(der(state) = der.der_state
  der(variable) = der_variable
  der(element(state)) = element(der_state)
  der(der(element(state)) = der.der_element_r_c
  der(element(variable)) = der_variable_r_c

  sollte nicht vorkommen durch simplify:
  element(der(state)) = element(der_state)
  element(der(der(state)) = der.der_state_r_c
  element(der(variable)) = der_variable_r_c
  */
  if (p->getType() == Type_Der)
  {
    if (p->getArg(0)->getType() == Type_Symbol)
    {
      const Symbol *sconst = Util::getAsConstPtr<Symbol>(p->getArg(0));
      // der(Parameter,Constant)
      if (sconst->getKind() == PARAMETER)
        return Zero::getZero(p->getShape());
      if (sconst->getKind() == CONSTANT)
        return Zero::getZero(p->getShape());
      if (sconst->getKind() == INPUT)
        return Zero::getZero(p->getShape());
      if (sconst->getKind() == CONTROLLER)
        return Zero::getZero(p->getShape());
      // der(state)
      if ((sconst->stateKind() == ALL) && (sconst->is_State(0,0) & STATE))
      {
          return p;
      }
      // der(variable)
      else
      {
        SymbolPtr sp(const_cast<Symbol*>(sconst));
        std::map<SymbolPtr,BasicPtrKindPair>::iterator ii = dummy_der.find(sp);
        if (ii != dummy_der.end())
        {
          if (ii->second.second != ALL)
          {
            Matrix *mat = Util::getAsPtr<Matrix>(ii->second.first);
            size_t dim1 = sp->getShape().getDimension(1);
            size_t dim2 = sp->getShape().getDimension(2);
            for(size_t i=0;i<dim1;++i)
            {
              for(size_t j=0;j<dim2;++j)
              {
                if (mat->get(i,j) == Zero::getZero())
                {
                  if (sp->is_State(i,j) & STATE)
                    mat->set(i,j,Der::New(Element::New(sp,i,j)));
                  else if (sp->is_State(i,j) & DUMMY_STATE)
                  {
                    mat->set(i,j,Der::New(Element::New(SymbolPtr(sp->getDerivative()),i,j)));
                  }
                  else
                  {
                    SymbolPtr ders = new Symbol("$DER" + sp->getName() + "_" + str(i) + "_" + str(j),Shape(),VARIABLE);
                    mat->set(i,j, ders);
                    newsyms.insert(ders);
                  }
                }
              }
            }
            ii->second.second = ALL;
          }
          return ii->second.first;
        }
        else
        {
          if (sp->is_Scalar())
          {
            SymbolPtr ders(sp->getDerivative());
            dummy_der[sp] = BasicPtrKindPair(ders,ALL);
            newsyms.insert(ders);
            return ders;
          }
          else
          {
            size_t dim1 = sconst->getShape().getDimension(1);
            size_t dim2 = sconst->getShape().getDimension(2);
            Matrix *mat = new Matrix(sconst->getShape());
            for(size_t i=0;i<dim1;++i)
            {
              for(size_t j=0;j<dim2;++j)
              {
                if (sp->is_State(i,j) & STATE)
                  mat->set(i,j,Der::New(Element::New(sp,i,j)));
                else if (sp->is_State(i,j) & DUMMY_STATE)
                {
                  mat->set(i,j,Der::New(Element::New(SymbolPtr(sp->getDerivative()),i,j)));
                }
                else
                {
                  SymbolPtr ders = new Symbol("$DER" + sconst->getName() + "_" + str(i) + "_" + str(j),Shape(),VARIABLE);
                  mat->set(i,j, ders);
                  newsyms.insert(ders);
                }
              }
            }
            dummy_der[sp] = BasicPtrKindPair(BasicPtr(mat),ALL);
            return BasicPtr(mat);
          }
        }
      }
    }
    // der(der(state))
    else if (p->getArg(0)->getType() == Type_Der)
    {
      const Der *der = Util::getAsConstPtr<Der>(p->getArg(0));
      if (der->getArg()->getType() == Type_Symbol)
      {
        const Symbol *sconst = Util::getAsConstPtr<Symbol>(der->getArg());
        // der(der(state))
        if (sconst->stateKind() == ALL)
        {
          if (sconst->is_State(0,0) & STATE)
          {
            SymbolPtr sp(const_cast<Symbol*>(sconst)->getDerivative());
            std::map<SymbolPtr,BasicPtrKindPair>::iterator ii = dummy_der.find(sp);
            if (ii != dummy_der.end())
            {
              if (ii->second.second != ALL)
              {
                Matrix *mat = Util::getAsPtr<Matrix>(ii->second.first);
                size_t dim1 = ii->first->getShape().getDimension(1);
                size_t dim2 = ii->first->getShape().getDimension(2);
                for(size_t i=0;i<dim1;++i)
                {
                  for(size_t j=0;j<dim2;++j)
                  {
                    if (mat->get(i,j) == Zero::getZero())
                    {
                      SymbolPtr ders = new Symbol("$DER" + ii->first->getName() + "_" + str(i) + "_" + str(j),Shape(),VARIABLE);
                      mat->set(i,j, ders);
                      newsyms.insert(ders);
                    }
                  }
                }
                ii->second.second = ALL;
              }
              return ii->second.first;
            }
            if (sp->is_Scalar())
            {
              SymbolPtr dds(new Symbol("$DER" + sp->getName(),sp->getShape(),VARIABLE));
              dummy_der[sp] = BasicPtrKindPair(dds,ALL);
              newsyms.insert(dds);
              return dds;
            }
            else
            {
              size_t dim1 = sconst->getShape().getDimension(1);
              size_t dim2 = sconst->getShape().getDimension(2);
              Matrix *mat = new Matrix(sconst->getShape());
              for(size_t i=0;i<dim1;++i)
              {
                for(size_t j=0;j<dim2;++j)
                {
                  SymbolPtr ders = new Symbol("$DER" + sp->getName() + "_" + str(i) + "_" + str(j),Shape(),VARIABLE);
                  mat->set(i,j, ders);
                  newsyms.insert(ders);
                }
              }
              dummy_der[sp] = BasicPtrKindPair(BasicPtr(mat),ALL);
              return BasicPtr(mat);
            }
          }
        }
        throw InternalError("der(der(off non full state " + sconst->getName() + "))");
      }
      else if (der->getArg()->getType() == Type_Element)
      {
        const Element *elem = Util::getAsConstPtr<Element>(der->getArg());
        {
          if (elem->getArg(0)->getType() == Type_Symbol)
          {
            const Symbol *sconst = Util::getAsConstPtr<Symbol>(elem->getArg(0));
            // der(der(element(state)))
            if (sconst->is_State(elem->getRow(),elem->getCol()) & STATE)
            {
              SymbolPtr sp(const_cast<Symbol*>(sconst)->getDerivative());
              std::map<SymbolPtr,BasicPtrKindPair>::iterator ii = dummy_der.find(sp);
              if (ii != dummy_der.end())
              {
                Matrix *mat = Util::getAsPtr<Matrix>(ii->second.first);
                if (mat->get(elem->getRow(),elem->getCol()) == Zero::getZero())
                {
                    SymbolPtr ders = new Symbol("$DER" + ii->first->getName() + "_" + str(elem->getRow()) + "_" + str(elem->getCol()),Shape(),VARIABLE);
                    mat->set(elem->getRow(),elem->getCol(), ders);
                    newsyms.insert(ders);
                }
                return mat->get(elem->getRow(),elem->getCol());
              }
              if (sp->is_Scalar())
              {
                SymbolPtr dds(new Symbol("$DER" + sp->getName(),sp->getShape(),VARIABLE));
                dummy_der[sp] = BasicPtrKindPair(dds,ALL);
                newsyms.insert(dds);
                return dds;
              }
              else
              {
                size_t dim1 = sp->getShape().getDimension(1);
                size_t dim2 = sp->getShape().getDimension(2);
                Matrix *mat = new Matrix(sconst->getShape());
                SymbolPtr ders = new Symbol("$DER" + sp->getName() + "_" + str(elem->getRow()) + "_" + str(elem->getCol()),Shape(),VARIABLE);
                mat->set(elem->getRow(),elem->getCol(), ders);
                newsyms.insert(ders);
                dummy_der[sp] = BasicPtrKindPair(BasicPtr(mat),SEVERAL);
                newsyms.insert(ders);
                return ders;
              }
            }
            throw InternalError("der(der(element(non State " + sconst->getName() + ")))!");
          }
        }
      }
    }
    else if (p->getArg(0)->getType() == Type_Element)
    {
      const Element *elem = Util::getAsConstPtr<Element>(p->getArg(0));
      {
        if (elem->getArg(0)->getType() == Type_Symbol)
        {
          const Symbol *sconst = Util::getAsConstPtr<Symbol>(elem->getArg(0));
          // der(Element(Parameter,Constant))
          if (sconst->getKind() == PARAMETER)
            return Zero::getZero();
          if (sconst->getKind() == CONSTANT)
            return Zero::getZero();
          if (sconst->getKind() == INPUT)
            return Zero::getZero();
          if (sconst->getKind() == CONTROLLER)
            return Zero::getZero();
          // der(element(state))
          if (sconst->is_State(elem->getRow(),elem->getCol()) & STATE)
            return p;
          if (sconst->is_State(elem->getRow(),elem->getCol()) & DUMMY_STATE)
            return Element::New(SymbolPtr(const_cast<Symbol*>(sconst)->getDerivative()),elem->getRow(),elem->getCol());
          else
          {
           // der(element(variable))
            SymbolPtr sp(const_cast<Symbol*>(sconst));
            std::map<SymbolPtr,BasicPtrKindPair>::iterator ii = dummy_der.find(sp);
            if (ii != dummy_der.end())
            {
              Matrix *mat = Util::getAsPtr<Matrix>(ii->second.first);
              if (mat->get(elem->getRow(),elem->getCol()) == Zero::getZero())
              {
                SymbolPtr ders = new Symbol("$DER" + ii->first->getName() + "_" + str(elem->getRow()) + "_" + str(elem->getCol()),Shape(),VARIABLE);
                mat->set(elem->getRow(),elem->getCol(), ders);
                newsyms.insert(ders);
              }
              return mat->get(elem->getRow(),elem->getCol());
            }
            else
            {
              if (sp->is_Scalar())
              {
                SymbolPtr ders(sp->getDerivative());
                dummy_der[sp] = BasicPtrKindPair(ders,ALL);
                newsyms.insert(ders);
                return ders;
              }
              else
              {
                Matrix *mat = new Matrix(sconst->getShape());
                SymbolPtr ders = new Symbol("$DER" + sconst->getName() + "_" + str(elem->getRow()) + "_" + str(elem->getCol()),Shape(),VARIABLE);
                mat->set(elem->getRow(),elem->getCol(),ders);
                dummy_der[sp] = BasicPtrKindPair(BasicPtr(mat),SEVERAL);
                newsyms.insert(ders);
                return ders;
              }
            }
          }
        }
      }
    }
    throw InternalError("Der(Element...) ?");
  }
  return p;
}
/*****************************************************************************/


/*****************************************************************************/
void UnMatchedSystem::strongConnect(size_t &i, MEquation* eqn,
  MEquationPtrVec &stack)
/*****************************************************************************/
{
  i = i+1;
  eqn->number = i;
  eqn->lowlink = i;
  eqn->onStack = true;
  stack.push_back(eqn);
  // get all needed equations to calculate this equation
  MSymbolPtrSet &symbols = eqn->symbols;
  for (MSymbolPtrSet::iterator ii=symbols.begin();ii!=symbols.end();++ii)
  {
    if ((*ii) != eqn->symbol)
    {
      size_t userdata = (*ii)->eqn;
      if (userdata != 0)
      {
        MEquation *eq = m_equations[userdata-1];
        if (eq->number == 0)
        {
          strongConnect(i,eq,stack);
          if (eqn->lowlink > eq->lowlink)
            eqn->lowlink = eq->lowlink;
        }
        else
        {
          if (eqn->number > eq->number)
          {
            // on stack?
            if (eq->onStack)
            {
              if (eqn->lowlink > eq->number)
                eqn->lowlink = eq->number;
              break;
            }
          }
        }
      }
    }
  }
  // check root
  if (eqn->number == eqn->lowlink)
  {
    MEquationPtrVec comps;
    while (stack.size() > 0)
    {
      if (stack[stack.size()-1]->number >= eqn->number)
      {
        stack[stack.size()-1]->onStack = false;
        comps.push_back(stack[stack.size()-1]);
        stack.pop_back();
      }
      else
        break;
    }
    if (comps.size() > 0)
      m_comps.push_back(comps);
  }
}
/*****************************************************************************/

/*****************************************************************************/
bool UnMatchedSystem::pathFound(size_t e, SizeTSet &eqn_mark, MSymbolPtrSet &sym_mark,size_t &c)
/*****************************************************************************/
{
  c++;
  // mark eqn
  eqn_mark.insert(e);
  // try to find a free symbol
  MSymbolPtrSet &eqnsymbols = m_equations[e]->symbols;
  for (MSymbolPtrSet::iterator ii=eqnsymbols.begin();ii!=eqnsymbols.end();++ii)
  {
    if ((*ii)->state || (*ii)->parameter)
      continue;
    // is not assigned
    if ((*ii)->eqn == 0)
    {
      // assign
      m_equations[e]->symbol = (*ii);
      (*ii)->eqn = e+1;
      return true;
    }
  }
  // get all not marked symbols of equation
  MSymbolPtrVec notmarkedsymbols;
  notmarkedsymbols.reserve(eqnsymbols.size());
  for (MSymbolPtrSet::iterator ii=eqnsymbols.begin();ii!=eqnsymbols.end();++ii)
  {
   if ((*ii)->state || (*ii)->parameter)
      continue;
    if (sym_mark.find((*ii)) == sym_mark.end())
      notmarkedsymbols.push_back((*ii));
  }
  // try to free a symbol 
  for (size_t j=0;j<notmarkedsymbols.size();++j)
  {
    sym_mark.insert(notmarkedsymbols[j]);
    size_t m = notmarkedsymbols[j]->eqn;
    if (m != 0)
    {
      m_equations[m-1]->c1++;
      if (pathFound(m-1,eqn_mark,sym_mark,c))
      {
        // assign
        m_equations[e]->symbol = notmarkedsymbols[j];
        notmarkedsymbols[j]->eqn = e+1;
        return true;
      }
    }
  }
  // no succes return false
  return false;
}
/*****************************************************************************/

/*****************************************************************************/
void UnMatchedSystem::toGraphML( std::string file )
/*****************************************************************************/
{
  // open file
  std::ofstream f;
  f.open(file.c_str());

  // write header
  f << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>" << std::endl;
  f << "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:y=\"http://www.yworks.com/xml/graphml\" xmlns:yed=\"http://www.yworks.com/xml/yed/3\" xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns http://www.yworks.com/xml/schema/graphml/1.1/ygraphml.xsd\">" << std::endl;
  f << "  <!--Created by PyMbs -->" << std::endl;
  f << "  <key for=\"graphml\" id=\"d0\" yfiles.type=\"resources\"/>" << std::endl;
  f << "  <key for=\"port\" id=\"d1\" yfiles.type=\"portgraphics\"/>" << std::endl;
  f << "  <key for=\"port\" id=\"d2\" yfiles.type=\"portgeometry\"/>" << std::endl;
  f << "  <key for=\"port\" id=\"d3\" yfiles.type=\"portuserdata\"/>" << std::endl;
  f << "  <key attr.name=\"url\" attr.type=\"string\" for=\"node\" id=\"d4\"/>" << std::endl;
  f << "  <key attr.name=\"description\" attr.type=\"string\" for=\"node\" id=\"d5\"/>" << std::endl;
  f << "  <key for=\"node\" id=\"d6\" yfiles.type=\"nodegraphics\"/>" << std::endl;
  f << "  <key attr.name=\"Beschreibung\" attr.type=\"string\" for=\"graph\" id=\"d7\"/>" << std::endl;
  f << "  <key attr.name=\"url\" attr.type=\"string\" for=\"edge\" id=\"d8\"/>" << std::endl;
  f << "  <key attr.name=\"description\" attr.type=\"string\" for=\"edge\" id=\"d9\"/>" << std::endl;;
  f << "  <key for=\"edge\" id=\"d10\" yfiles.type=\"edgegraphics\"/>" << std::endl;
  f << "  <graph edgedefault=\"directed\" id=\"G\">" << std::endl;
  f << "    <data key=\"d7\"/>" << std::endl;
  // write symbols
  size_t id=0;
  std::map<std::string,size_t> symbolidmap;
  for (IncidenceMatrixT::iterator ii =m_incidencematrixT.begin(); ii != m_incidencematrixT.end();++ii)
  {
    f << "    <node id=\"n" << id << "\">" << std::endl;
    f << "      <data key=\"d5\"/>" << std::endl;
    f << "      <data key=\"d6\">" << std::endl;
    f << "        <y:ShapeNode>" << std::endl;
    f << "          <y:Geometry height=\"30.0\" width=\"30.0\" x=\"" << 40*(id+1) << "\" y=\"1.0\"/>" << std::endl;
    std::string color = "#000000";
    size_t cat = ii->first->symbol->getKind();
    if (cat & STATE)
    {
      if (ii->first->state)
        color = "#FF0000";
      else
      color = "#FFFFFF";
    }
    else if (cat & DER_STATE)
      color = "#00CCFF";
    else if (cat & INPUT)
      color = "#99CC00";
    else if (cat & CONTROLLER)
      color = "#99CC00";
    else if (cat & PARAMETER)
      color = "#FF6600";
    else if (cat & CONSTANT)
      color = "#C0C0C0";
    else if (cat & VARIABLE)
      color = "#FFFFFF";
    f << "          <y:Fill color=\"" + color + "\" transparent=\"false\"/>" << std::endl;
    std::string bscolor = "000000";
    f << "          <y:BorderStyle color=\"" + bscolor + "\" type=\"line\" width=\"1.0\"/>" << std::endl;
    f << "          <y:NodeLabel alignment=\"center\" autoSizePolicy=\"content\" fontFamily=\"Dialog\" fontSize=\"12\" fontStyle=\"plain\" hasBackgroundColor=\"false\" hasLineColor=\"false\" height=\"18.701171875\" modelName=\"internal\" modelPosition=\"b\" rotationAngle=\"270.0\" textColor=\"#000000\" visible=\"true\" width=\"30.015625\" x=\"-0.0078125\" y=\"5.6494140625\">";
    std::string name = ii->first->symbol->getName();
    if (!ii->first->symbol->is_Scalar())
      name += "[" + str(ii->first->dim1) +  "," + str(ii->first->dim2) + "]";
    f << name << "</y:NodeLabel>" << std::endl;
    symbolidmap[name] = id;
    f << "          <y:Shape type=\"rectangle\"/>" << std::endl;
    f << "        </y:ShapeNode>" << std::endl;
    f << "      </data>" << std::endl;
    f << "    </node>" << std::endl;
    id++;
  }
  // write equations
  id = 0;
  size_t iid=0;
  for (MEquationPtrVec::iterator ii =m_equations.begin(); ii != m_equations.end();++ii)
  {
    f << "    <node id=\"n" << id + m_incidencematrixT.size() << "\">" << std::endl;
    f << "      <data key=\"d5\"/>" << std::endl;
    f << "      <data key=\"d6\">" << std::endl;
    f << "        <y:ShapeNode>" << std::endl;
    f << "          <y:Geometry height=\"30.0\" width=\"30.0\" x=\"1.0\" y=\"" << 40*(id+1) << "\"/>" << std::endl;
    f << "          <y:Fill color=\"#C0C0C0\" transparent=\"false\"/>" << std::endl;
    f << "          <y:BorderStyle color=\"000000\" type=\"line\" width=\"1.0\"/>" << std::endl;
    f << "          <y:NodeLabel alignment=\"center\" autoSizePolicy=\"content\" fontFamily=\"Dialog\" fontSize=\"12\" fontStyle=\"plain\" hasBackgroundColor=\"false\" hasLineColor=\"false\" height=\"18.701171875\" modelName=\"internal\" modelPosition=\"r\" textColor=\"#000000\" visible=\"true\" width=\"30.015625\" x=\"-0.0078125\" y=\"5.6494140625\">";
    std::string name =  (*ii)->eqn->toString();
    if ((*ii)->eqn->getNumEl() > 1)
      name += " [" + str((*ii)->dim1) + "," + str((*ii)->dim2) + "]";
    f << name << "</y:NodeLabel>" << std::endl;
    f << "          <y:Shape type=\"rectangle\"/>" << std::endl;
    f << "        </y:ShapeNode>" << std::endl;
    f << "      </data>" << std::endl;
    f << "    </node>" << std::endl;
    id++;
    for (MSymbolPtrSet::iterator jj =(*ii)->symbols.begin(); jj != (*ii)->symbols.end();++jj)
    {
      std::string sname = (*jj)->symbol->getName();
      if (!(*jj)->symbol->is_Scalar())
        sname += "[" + str((*jj)->dim1) +  "," + str((*jj)->dim2) + "]";
      std::map<std::string,size_t>::iterator kk =  symbolidmap.find(sname);
      if (kk != symbolidmap.end())
      {
        f << "    <node id=\"n" << iid + m_incidencematrixT.size() + m_equations.size() << "\">" << std::endl;
        f << "      <data key=\"d5\"/>" << std::endl;
        f << "      <data key=\"d6\">" << std::endl;
        f << "        <y:ShapeNode>" << std::endl;
        f << "          <y:Geometry height=\"30.0\" width=\"30.0\" x=\"" << 40*((*kk).second+1) << "\" y=\"" << 40*id << "\"/>" << std::endl;
        f << "          <y:Fill color=\"#000000\" transparent=\"false\"/>" << std::endl;
        f << "          <y:BorderStyle color=\"000000\" type=\"line\" width=\"1.0\"/>" << std::endl;
        f << "          <y:NodeLabel alignment=\"center\" autoSizePolicy=\"content\" fontFamily=\"Dialog\" fontSize=\"12\" fontStyle=\"plain\" hasBackgroundColor=\"false\" hasLineColor=\"false\" height=\"18.701171875\" modelName=\"internal\" modelPosition=\"c\" textColor=\"#000000\" visible=\"true\" width=\"30.015625\" x=\"-0.0078125\" y=\"5.6494140625\">";
        std::string _name = "";
        f << _name << "</y:NodeLabel>" << std::endl;
        f << "          <y:Shape type=\"rectangle\"/>" << std::endl;
        f << "        </y:ShapeNode>" << std::endl;
        f << "      </data>" << std::endl;
        f << "    </node>" << std::endl;
        iid++;
      }
    }
    for (MSymbolPtrSet::iterator jj =(*ii)->states.begin(); jj != (*ii)->states.end();++jj)
    {
      std::string sname = (*jj)->symbol->getName();
      if (!(*jj)->symbol->is_Scalar())
        sname += "[" + str((*jj)->dim1) +  "," + str((*jj)->dim2) + "]";
      std::map<std::string,size_t>::iterator kk =  symbolidmap.find(sname);
      if (kk != symbolidmap.end())
      {
        f << "    <node id=\"n" << iid + m_incidencematrixT.size() + m_equations.size() << "\">" << std::endl;
        f << "      <data key=\"d5\"/>" << std::endl;
        f << "      <data key=\"d6\">" << std::endl;
        f << "        <y:ShapeNode>" << std::endl;
        f << "          <y:Geometry height=\"30.0\" width=\"30.0\" x=\"" << 40*((*kk).second+1) << "\" y=\"" << 40*id << "\"/>" << std::endl;
        f << "          <y:Fill color=\"#993300\" transparent=\"false\"/>" << std::endl;
        f << "          <y:BorderStyle color=\"000000\" type=\"line\" width=\"1.0\"/>" << std::endl;
        f << "          <y:NodeLabel alignment=\"center\" autoSizePolicy=\"content\" fontFamily=\"Dialog\" fontSize=\"12\" fontStyle=\"plain\" hasBackgroundColor=\"false\" hasLineColor=\"false\" height=\"18.701171875\" modelName=\"internal\" modelPosition=\"c\" textColor=\"#000000\" visible=\"true\" width=\"30.015625\" x=\"-0.0078125\" y=\"5.6494140625\">";
        std::string _name = "";
        f << _name << "</y:NodeLabel>" << std::endl;
        f << "          <y:Shape type=\"rectangle\"/>" << std::endl;
        f << "        </y:ShapeNode>" << std::endl;
        f << "      </data>" << std::endl;
        f << "    </node>" << std::endl;
        iid++;
      }
    }
  }
  f << "  </graph>" << std::endl;
  f << "  <data key=\"d0\">" << std::endl;
  f << "    <y:Resources/>" << std::endl;
  f << "  </data>" << std::endl;
  f << "</graphml>" << std::endl;
  // close file
  f.close();
}
/*****************************************************************************/


/*****************************************************************************/
void UnMatchedSystem::toGraphML_Sorted( std::string file )
/*****************************************************************************/
{
  // open file
  std::ofstream f;
  f.open(file.c_str());

  // write header
  f << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>" << std::endl;
  f << "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:y=\"http://www.yworks.com/xml/graphml\" xmlns:yed=\"http://www.yworks.com/xml/yed/3\" xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns http://www.yworks.com/xml/schema/graphml/1.1/ygraphml.xsd\">" << std::endl;
  f << "  <!--Created by PyMbs -->" << std::endl;
  f << "  <key for=\"graphml\" id=\"d0\" yfiles.type=\"resources\"/>" << std::endl;
  f << "  <key for=\"port\" id=\"d1\" yfiles.type=\"portgraphics\"/>" << std::endl;
  f << "  <key for=\"port\" id=\"d2\" yfiles.type=\"portgeometry\"/>" << std::endl;
  f << "  <key for=\"port\" id=\"d3\" yfiles.type=\"portuserdata\"/>" << std::endl;
  f << "  <key attr.name=\"url\" attr.type=\"string\" for=\"node\" id=\"d4\"/>" << std::endl;
  f << "  <key attr.name=\"description\" attr.type=\"string\" for=\"node\" id=\"d5\"/>" << std::endl;
  f << "  <key for=\"node\" id=\"d6\" yfiles.type=\"nodegraphics\"/>" << std::endl;
  f << "  <key attr.name=\"Beschreibung\" attr.type=\"string\" for=\"graph\" id=\"d7\"/>" << std::endl;
  f << "  <key attr.name=\"url\" attr.type=\"string\" for=\"edge\" id=\"d8\"/>" << std::endl;
  f << "  <key attr.name=\"description\" attr.type=\"string\" for=\"edge\" id=\"d9\"/>" << std::endl;;
  f << "  <key for=\"edge\" id=\"d10\" yfiles.type=\"edgegraphics\"/>" << std::endl;
  f << "  <graph edgedefault=\"directed\" id=\"G\">" << std::endl;
  f << "    <data key=\"d7\"/>" << std::endl;
  // write symbols
  size_t id=0;
  std::map<std::string,size_t> symbolidmap;
  for (MEquationPtrVecVec::iterator ia = m_comps.begin();ia!=m_comps.end();ia++)
  {
    for (MEquationPtrVec::iterator ii =ia->begin(); ii != ia->end();++ii)
    {
      f << "    <node id=\"n" << id << "\">" << std::endl;
      f << "      <data key=\"d5\"/>" << std::endl;
      f << "      <data key=\"d6\">" << std::endl;
      f << "        <y:ShapeNode>" << std::endl;
      f << "          <y:Geometry height=\"30.0\" width=\"30.0\" x=\"" << 40*(id+1) << "\" y=\"1.0\"/>" << std::endl;
      std::string color = "#000000";
      size_t cat = (*ii)->symbol->symbol->getKind();
      if (cat & STATE)
      {
        if ((*ii)->symbol->state)
          color = "#FF0000";
        else
        color = "#FFFFFF";
      }
      else if (cat & DER_STATE)
        color = "#00CCFF";
      else if (cat & INPUT)
        color = "#99CC00";
      else if (cat & CONTROLLER)
        color = "#99CC00";
      else if (cat & PARAMETER)
        color = "#FF6600";
      else if (cat & CONSTANT)
        color = "#C0C0C0";
      else if (cat & VARIABLE)
        color = "#FFFFFF";
      f << "          <y:Fill color=\"" + color + "\" transparent=\"false\"/>" << std::endl;
      std::string bscolor = "000000";
      f << "          <y:BorderStyle color=\"" + bscolor + "\" type=\"line\" width=\"1.0\"/>" << std::endl;
      f << "          <y:NodeLabel alignment=\"center\" autoSizePolicy=\"content\" fontFamily=\"Dialog\" fontSize=\"12\" fontStyle=\"plain\" hasBackgroundColor=\"false\" hasLineColor=\"false\" height=\"18.701171875\" modelName=\"internal\" modelPosition=\"b\" rotationAngle=\"270.0\" textColor=\"#000000\" visible=\"true\" width=\"30.015625\" x=\"-0.0078125\" y=\"5.6494140625\">";
      std::string name = (*ii)->symbol->symbol->getName();
      if (!(*ii)->symbol->symbol->is_Scalar())
        name += "[" + str((*ii)->symbol->dim1) +  "," + str((*ii)->symbol->dim2) + "]";
      f << name << "</y:NodeLabel>" << std::endl;
      symbolidmap[name] = id;
      f << "          <y:Shape type=\"rectangle\"/>" << std::endl;
      f << "        </y:ShapeNode>" << std::endl;
      f << "      </data>" << std::endl;
      f << "    </node>" << std::endl;
      id++;
    }
  }
  // write equations
  id = 0;
  size_t iid=0;
  for (MEquationPtrVecVec::iterator ia = m_comps.begin();ia!=m_comps.end();ia++)
  {
    for (MEquationPtrVec::iterator ii =ia->begin(); ii != ia->end();++ii)
    {
      f << "    <node id=\"n" << id + m_incidencematrixT.size() << "\">" << std::endl;
      f << "      <data key=\"d5\"/>" << std::endl;
      f << "      <data key=\"d6\">" << std::endl;
      f << "        <y:ShapeNode>" << std::endl;
      f << "          <y:Geometry height=\"30.0\" width=\"30.0\" x=\"1.0\" y=\"" << 40*(id+1) << "\"/>" << std::endl;
      f << "          <y:Fill color=\"#C0C0C0\" transparent=\"false\"/>" << std::endl;
      f << "          <y:BorderStyle color=\"000000\" type=\"line\" width=\"1.0\"/>" << std::endl;
      f << "          <y:NodeLabel alignment=\"center\" autoSizePolicy=\"content\" fontFamily=\"Dialog\" fontSize=\"12\" fontStyle=\"plain\" hasBackgroundColor=\"false\" hasLineColor=\"false\" height=\"18.701171875\" modelName=\"internal\" modelPosition=\"r\" textColor=\"#000000\" visible=\"true\" width=\"30.015625\" x=\"-0.0078125\" y=\"5.6494140625\">";
      std::string name =  (*ii)->eqn->toString();
      if ((*ii)->eqn->getNumEl() > 1)
        name += " [" + str((*ii)->dim1) + "," + str((*ii)->dim2) + "]";
      f << name << "</y:NodeLabel>" << std::endl;
      f << "          <y:Shape type=\"rectangle\"/>" << std::endl;
      f << "        </y:ShapeNode>" << std::endl;
      f << "      </data>" << std::endl;
      f << "    </node>" << std::endl;
      id++;
      for (MSymbolPtrSet::iterator jj =(*ii)->symbols.begin(); jj != (*ii)->symbols.end();++jj)
      {
        std::string sname = (*jj)->symbol->getName();
        if (!(*jj)->symbol->is_Scalar())
          sname += "[" + str((*jj)->dim1) +  "," + str((*jj)->dim2) + "]";
        std::map<std::string,size_t>::iterator kk =  symbolidmap.find(sname);
        if (kk != symbolidmap.end())
        {
          f << "    <node id=\"n" << iid + m_incidencematrixT.size() + m_equations.size() << "\">" << std::endl;
          f << "      <data key=\"d5\"/>" << std::endl;
          f << "      <data key=\"d6\">" << std::endl;
          f << "        <y:ShapeNode>" << std::endl;
          f << "          <y:Geometry height=\"30.0\" width=\"30.0\" x=\"" << 40*((*kk).second+1) << "\" y=\"" << 40*id << "\"/>" << std::endl;
          f << "          <y:Fill color=\"#000000\" transparent=\"false\"/>" << std::endl;
          f << "          <y:BorderStyle color=\"000000\" type=\"line\" width=\"1.0\"/>" << std::endl;
          f << "          <y:NodeLabel alignment=\"center\" autoSizePolicy=\"content\" fontFamily=\"Dialog\" fontSize=\"12\" fontStyle=\"plain\" hasBackgroundColor=\"false\" hasLineColor=\"false\" height=\"18.701171875\" modelName=\"internal\" modelPosition=\"c\" textColor=\"#000000\" visible=\"true\" width=\"30.015625\" x=\"-0.0078125\" y=\"5.6494140625\">";
          std::string _name = "";
          f << _name << "</y:NodeLabel>" << std::endl;
          f << "          <y:Shape type=\"rectangle\"/>" << std::endl;
          f << "        </y:ShapeNode>" << std::endl;
          f << "      </data>" << std::endl;
          f << "    </node>" << std::endl;
          iid++;
        }
      }
      for (MSymbolPtrSet::iterator jj =(*ii)->states.begin(); jj != (*ii)->states.end();++jj)
      {
        std::string sname = (*jj)->symbol->getName();
        if (!(*jj)->symbol->is_Scalar())
          sname += "[" + str((*jj)->dim1) +  "," + str((*jj)->dim2) + "]";
        std::map<std::string,size_t>::iterator kk =  symbolidmap.find(sname);
        if (kk != symbolidmap.end())
        {
          f << "    <node id=\"n" << iid + m_incidencematrixT.size() + m_equations.size() << "\">" << std::endl;
          f << "      <data key=\"d5\"/>" << std::endl;
          f << "      <data key=\"d6\">" << std::endl;
          f << "        <y:ShapeNode>" << std::endl;
          f << "          <y:Geometry height=\"30.0\" width=\"30.0\" x=\"" << 40*((*kk).second+1) << "\" y=\"" << 40*id << "\"/>" << std::endl;
          f << "          <y:Fill color=\"#993300\" transparent=\"false\"/>" << std::endl;
          f << "          <y:BorderStyle color=\"000000\" type=\"line\" width=\"1.0\"/>" << std::endl;
          f << "          <y:NodeLabel alignment=\"center\" autoSizePolicy=\"content\" fontFamily=\"Dialog\" fontSize=\"12\" fontStyle=\"plain\" hasBackgroundColor=\"false\" hasLineColor=\"false\" height=\"18.701171875\" modelName=\"internal\" modelPosition=\"c\" textColor=\"#000000\" visible=\"true\" width=\"30.015625\" x=\"-0.0078125\" y=\"5.6494140625\">";
          std::string _name = "";
          f << _name << "</y:NodeLabel>" << std::endl;
          f << "          <y:Shape type=\"rectangle\"/>" << std::endl;
          f << "        </y:ShapeNode>" << std::endl;
          f << "      </data>" << std::endl;
          f << "    </node>" << std::endl;
          iid++;
        }
      }
    }
  }
  f << "  </graph>" << std::endl;
  f << "  <data key=\"d0\">" << std::endl;
  f << "    <y:Resources/>" << std::endl;
  f << "  </data>" << std::endl;
  f << "</graphml>" << std::endl;
  // close file
  f.close();
}
/*****************************************************************************/

/*****************************************************************************/
void UnMatchedSystem::dumpComps( std::string file )
/*****************************************************************************/
{
  // open file
  std::ofstream f;
  f.open(file.c_str());
  if (m_comps.size() == 0)
  {
    f << "No Comps!" << std::endl;
    return;
  }

  for (size_t i=0;i<m_comps.size();i++)
  {
    f << "<Comp>" << std::endl;
    for (size_t j=0;j<m_comps[i].size();j++)
    { 
      f << m_comps[i][j]->symbol->symbol->toString() << "[" << m_comps[i][j]->symbol->dim1 << ", " << m_comps[i][j]->symbol->dim2 << "] := ";

      f << m_comps[i][j]->eqn->toString() << " [" << m_comps[i][j]->dim1 << ", " << m_comps[i][j]->dim2 << std::endl;
    }
    f << "<Comp/>" << std::endl;
  }
  // close file
  f.close();
}
/*****************************************************************************/
