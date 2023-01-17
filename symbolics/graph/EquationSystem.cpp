#include "EquationSystem.h"
#include "str.h"
#include <iostream>
#include <fstream>
#include "ModelicaPrinter.h"

using namespace Symbolics;
using namespace Graph;

/*****************************************************************************/
EquationSystem::EquationSystem(): m_balanced(true),m_refCount(0) 
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
EquationSystem::~EquationSystem()
/*****************************************************************************/
{ 
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr EquationSystem::addSymbol( SymbolPtr const& s , Symbolics::Basic *initalValue, std::string comment)
/*****************************************************************************/
{
  // check if already there
  const Symbol *sym = Util::getAsConstPtr<Symbol>(s);
  std::string name = sym->getName();
  StringSymbolMap::iterator kk = m_SymbolMap.find(name);
  if (kk != m_SymbolMap.end())
  {
    // fehler werfen
    throw SymbolAlreadyExistsError(name);
  }
  return addSymbol_Internal(s, initalValue, comment);
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr EquationSystem::addSymbol_Internal( SymbolPtr const& s,
  Symbolics::Basic *initalValue, std::string comment)
/*****************************************************************************/
{
  StringSymbolMap::iterator ii = m_SymbolMap.find(s->getName());
  if (ii != m_SymbolMap.end())
    return ii->second;
  // add
  m_SymbolMap[s->getName()] = s;

  if (initalValue == NULL)
  {
    m_symbolsvaluemap[s] = Zero::getExactZero(s->getShape());
  }
  else
  {
    if (s->getShape() != initalValue->getShape())
      throw ShapeError("Could not use Initvalue " + initalValue->toString() + " for Symbol " + s->toString() + "!");
    BasicPtr init = BasicPtr(initalValue->simplify());
    InitValueScanner sc;
    init->scanExp(sc);
    if (sc.variable)
      throw InitalValueError("Initvalue is not constant: " + init->toString() + "!");
    m_symbolsvaluemap[s] = init;
  }

  m_symbolscommentmap[s] = comment;

  switch( s->getKind())
  {
  case PARAMETER:
    m_parameter.insert(s);
    break;
  case INPUT:
    m_inputs.insert(s);
    break;
  case CONTROLLER:
    m_controller.insert(s);
    break;
  case VARIABLE:
    if (s->stateKind() == ALL)
    {
      if (s->is_State(0,0) & STATE)
      {
        m_states[s] = SymbolElement();
        SymbolPtr sd = const_cast<Symbol*>(Util::getAsConstPtr<Symbol>(s))->getDerivative();
        m_derstates[sd] = SymbolElement();
      }
      else
      {
        m_variables[s] = SymbolElement();
      }
    }
    else
    {
      size_t dim1 = s->getShape().getDimension(1);
      size_t dim2 = s->getShape().getDimension(2);
      SymbolElement stateelem;
      stateelem.kind = SEVERAL;
      SymbolElement varelem;
      varelem.kind = SEVERAL;
      for(size_t i=0;i<dim1;i++)
      {
        for(size_t j=0;j<dim2;j++)
        {
          if (s->is_State(i,j) & STATE)
          {
            stateelem.elements.insert(SizeTPair(i,j));
          }
          else
          {
            varelem.elements.insert(SizeTPair(i,j));
          }
        }
      }
      if (stateelem.elements.size() > 0)
      {
        m_states[s] = stateelem;
        SymbolPtr sd = const_cast<Symbol*>(Util::getAsConstPtr<Symbol>(s))->getDerivative();
        m_derstates[sd] = stateelem;
      }
      if (varelem.elements.size() > 0)
        m_variables[s] = varelem;
    }
    break;
  case CONSTANT:
    m_constants.insert(s);
    break;
  }
  return BasicPtr(s);
}
/*****************************************************************************/

/*****************************************************************************/
void EquationSystem::addEquation(BasicPtr const& solveFor,
  BasicPtr const& exp, bool implicit)
/*****************************************************************************/
{
  BasicPtr simpleexp = exp->simplify();

  // wenn der(x)=... dann x wird zu State und auch stateder hinzufuegen
  SymbolPtrElemMap symbols;
  getSymbolOrDer(symbols,solveFor);
  // get all other derivatives
  DerivativeScanner scanner;
  simpleexp->scanExp(scanner);

    // check shape
  if (!implicit)
  {
    if (solveFor.get() != NULL)
    {
      if (solveFor->getShape() != simpleexp->getShape())
      {
        // Shapes sind verschieden
        // Ignorieren, wenn rechts eine Zahl steht
        if (((simpleexp->is_Scalar()) && ((simpleexp->getType() == Type_Int) || (simpleexp->getType() == Type_Real) || (simpleexp->getType() == Type_Zero))))
        {
          Matrix *mat = new Matrix(solveFor->getShape());
          for (size_t i=0; i<mat->getShape().getNumEl(); ++i)
            mat->set(i, simpleexp);
          simpleexp = mat;
        }
        else
          throw ShapeError("Could not solve Var " + solveFor->toString() + " with this Expression " + simpleexp->toString() + "!");
      }
    }
  }

  // Symbols of eqn
  checkEquation(&scanner,simpleexp,BasicPtrVec(),implicit,symbols);

  // add
  EquationPtr eqn;
  if (implicit || (solveFor.get() == NULL))
  {
    eqn = new Equation(symbols,Zero::getZero(simpleexp->getShape()),simpleexp,implicit);
  }
  else
    eqn = new Equation(symbols,solveFor,simpleexp,implicit);

  // add
  assignEqnstoSymbols(symbols,eqn);
  m_equations.insert(eqn);
}
/*****************************************************************************/

/*****************************************************************************/
void EquationSystem::assignEqnstoSymbols(SymbolPtrElemMap &symbols, EquationPtr eqn)
/*****************************************************************************/
{
  for (SymbolPtrElemMap::iterator is=symbols.begin();is!=symbols.end();is++)
  {
    if (is->second.kind == ALL)
      m_symbolsequationmap[is->first] = SymbolEquationElement(eqn);
    else
    {
      SymbolEquationMap::iterator ii = m_symbolsequationmap.find(is->first);
      if (ii != m_symbolsequationmap.end())
      {
        if (ii->second.kind == ALL)
          throw InternalError("Equation " + ii->second.eqn->toString() + " is allready solved and not solvable for " + is->first->toString());
        else
        {
          for (SizeTPairSet::iterator ie = is->second.elements.begin();ie!=is->second.elements.end();ie++)
          {
            ii->second.elements[SizeTPair(ie->first,ie->second)] = eqn;
          }
        }
      }
      else
      {
        SymbolEquationElement elem;
        for (SizeTPairSet::iterator ie = is->second.elements.begin();ie!=is->second.elements.end();ie++)
        {
          elem.elements[SizeTPair(ie->first,ie->second)] = eqn;
        }
        m_symbolsequationmap[is->first] = elem;
      }
    }
  }
}
/*****************************************************************************/

/*****************************************************************************/
void EquationSystem::checkEquation(DerivativeScanner *scanner, 
  BasicPtr const& exp, BasicPtrVec const& expvec, bool implicit, SymbolPtrElemMap &symbols)
/*****************************************************************************/
{
  // Symbols of eqn
  SymbolPtrElemMap &atoms = scanner->atoms;
  // add StateDerivative
  for (SymbolPtrElemMap::iterator ii=scanner->states.begin();ii!=scanner->states.end();ii++)
  {
    Symbol *s = const_cast<Symbol*>(Util::getAsConstPtr<Symbol>(ii->first));
    SymbolPtr sp(s);
    SymbolPtr ders = ii->first->getDerivative();
    SymbolPtrElemMap::iterator is = m_states.find(ii->first);
    if (is == m_states.end())
    {
      m_states[ii->first] = ii->second;
      addSymbol_Internal(ders, NULL, "");
      m_derstates[ders] = ii->second;
      if (ii->second.kind == ALL)
        m_variables.erase(s);
      else
      {
        SymbolPtrElemMap::iterator iv = m_variables.find(ii->first);
        if (iv != m_variables.end())
        {
          for (SizeTPairSet::iterator ie=ii->second.elements.begin();ie!=ii->second.elements.end();ie++)
          {
            iv->second.elements.erase(SizeTPair(ie->first,ie->second));
          }
          if (iv->second.elements.size() == 0)
            m_variables.erase(iv);
        }
      }
    }
    else
    {
      if (is->second.kind != ALL)
      {
        if (ii->second.kind != ALL)
        {
          SymbolPtrElemMap::iterator iv = m_variables.find(ii->first);
          for (SizeTPairSet::iterator ie=ii->second.elements.begin();ie!=ii->second.elements.end();ie++)
          {
            is->second.elements.insert(SizeTPair(ie->first,ie->second));
            m_derstates[ders].elements.insert(SizeTPair(ie->first,ie->second));
            if (iv != m_variables.end())
              iv->second.elements.erase(SizeTPair(ie->first,ie->second));
          }
        }
      }
    }
  }
  // symbol in exp dann Fehler
  if (!implicit)
  {
    for (SymbolPtrElemMap::iterator ii=symbols.begin();ii!=symbols.end();ii++)
    {
      SymbolPtrElemMap::iterator ik = atoms.find(ii->first);
      if (ik != atoms.end())
      {
        if ((ik->second.kind == ALL) | (ii->second.kind == ALL))
        {
          std::string expstr;
          if (exp.get() != NULL)
            expstr = exp->toString();
          else
            for(size_t j=0;j<expvec.size();++j)
            {
              expstr += expvec[j]->toString() + "\n";
            }
          throw ExpressionSolveForError("solve " + expstr + " for " + ii->first->getName() + " not possible, use implicit = true!");
        }
        else
        {
          for (SizeTPairSet::iterator ie=ik->second.elements.begin();ie!=ik->second.elements.end();ie++)
          {
            if (ii->second.elements.find(SizeTPair(ie->first,ie->second)) != ii->second.elements.end())
            {
              std::string expstr;
              if (exp.get() != NULL)
                expstr = exp->toString();
              else
                for(size_t j=0;j<expvec.size();++j)
                {
                  expstr += expvec[j]->toString() + "\n";
                }
              throw ExpressionSolveForError("solve " + expstr + " fpr " + ii->first->getName() + " not possible, use implicit = true!");
            }
          }
        }
      }
    }
  }
  // wenn symbol schon da dann Fehler 
  for (SymbolPtrElemMap::iterator is=symbols.begin();is!=symbols.end();is++)
  {
    SymbolEquationMap::iterator ii = m_symbolsequationmap.find(is->first);
    if (ii != m_symbolsequationmap.end())
    {
      std::string name = is->first->getName();
      if (ii->second.kind == ALL) 
      {
        std::string msg = name + " is already solved in ";
        for (size_t k=0;k<ii->second.eqn->getRhsSize();++k)
        {
          msg += ii->second.eqn->getRhs(k)->toString();
        }
        throw SymbolAlreadySolvedInError(msg);
      }
      else
      {
        for (SizeTPairSet::iterator ie=is->second.elements.begin();ie!=is->second.elements.end();ie++)
        {
          if (ii->second.elements.find(SizeTPair(ie->first,ie->second)) != ii->second.elements.end())
          {
            std::string msg = name + "[" + str(ie->first) + "," + str(ie->second) +  "] is already solved in ";
            for (size_t k=0;k<ii->second.eqn->getRhsSize();++k)
            {
              msg += ii->second.eqn->getRhs(k)->toString();
            }
            throw SymbolAlreadySolvedInError(msg);
          }
        }
      }
    }
  }
  // symbol nicht angelegt dann Fehler
  for (SymbolPtrElemMap::iterator ii = atoms.begin();ii!=atoms.end();++ii)
  {
    StringSymbolMap::iterator is = m_SymbolMap.find(ii->first->getName());
    if (is == m_SymbolMap.end())
      throw VarUnknownError(ii->first->getName());
  }
  // balanced ?
  if (symbols.size() == 0)
    m_balanced = false;


}
/*****************************************************************************/

/*****************************************************************************/
void EquationSystem::addEquation( BasicPtrVec const& solveFor, 
  BasicPtrVec const& exp, bool implicit)
/*****************************************************************************/
{
  // size equal
  if (solveFor.size() != exp.size()) 
  {
    std::string msg = "Symbolics::  addExpression: size of solveFor (" + str(solveFor.size()) + ") and exp (" + str(exp.size()) + ") not equal!";
    throw InternalError(msg);
  }
  // if not implicit us addEquation
  if (!implicit)
  {
    for (size_t i=0;i<exp.size();++i)
      addEquation(solveFor[i],exp[i],implicit);
    return;
  }

  // Versuchen lhs in Symbols zu casten
  SymbolPtrElemMap symbols;
  for(size_t j=0;j<solveFor.size();++j)
  {
    // wenn der(x)=... dann x wird zu State und auch stateder hinzufuegen
    getSymbolOrDer(symbols, solveFor[j]);
  }

  // get all other derivatives
  DerivativeScanner scanner;
  BasicPtrVec simpleexp;
  simpleexp.reserve(exp.size());
  for(size_t j=0;j<exp.size();++j)
  {
    simpleexp.push_back(exp[j]->simplify());
    simpleexp[j]->scanExp(scanner);
  }
  // Symbols of eqn
  checkEquation(&scanner,BasicPtr(),exp,implicit,symbols);

  // add
  BasicPtrVec lhs;
  for(size_t j=0;j<solveFor.size();++j)
  {
    lhs.push_back(Zero::getZero(solveFor[j]->getShape()));
  }
  // add
  EquationPtr eqn = new Equation(symbols,lhs,simpleexp,implicit);
  assignEqnstoSymbols(symbols,eqn);
  m_equations.insert(eqn);
}
/*****************************************************************************/

/*****************************************************************************/
void EquationSystem::addEquation( EquationPtr eqn)
/*****************************************************************************/
{
  // check for unknown symbols
  SymbolPtrElemSizeTMap &symbols = eqn->getSymbols();
  for (SymbolPtrElemSizeTMap::iterator m=symbols.begin();m!=symbols.end();m++)
  {
    StringSymbolMap::iterator ii = m_SymbolMap.find(m->first->getName());
    if (ii==m_SymbolMap.end())
      throw  VarUnknownError(m->first->getName());
  }
  SymbolPtrElemMap& solveFor = eqn->getSolveFor();
  // wenn symbol schon da dann Fehler 
  for (SymbolPtrElemMap::iterator is=solveFor.begin();is!=solveFor.end();is++)
  {
    SymbolEquationMap::iterator ii = m_symbolsequationmap.find(is->first);
    if (ii != m_symbolsequationmap.end())
    {
      std::string name = is->first->getName();
      if (ii->second.kind == ALL) 
      {
        std::string msg = name + " is already solved in ";
        for (size_t k=0;k<ii->second.eqn->getRhsSize();++k)
        {
          msg += ii->second.eqn->getRhs(k)->toString();
        }
        throw SymbolAlreadySolvedInError(msg);
      }
      else
      {
        for (SizeTPairSet::iterator ie=is->second.elements.begin();ie!=is->second.elements.end();ie++)
        {
          if (ii->second.elements.find(SizeTPair(ie->first,ie->second)) != ii->second.elements.end())
          {
            std::string msg = name + "[" + str(ie->first) + "," + str(ie->second) +  "] is already solved in ";
            for (size_t k=0;k<ii->second.eqn->getRhsSize();++k)
            {
              msg += ii->second.eqn->getRhs(k)->toString();
            }
            throw SymbolAlreadySolvedInError(msg);
          }
        }
      }
    }
  }
  // check if already there 
  EquationPtrSet::iterator ii = m_equations.find(eqn);
  if (ii != m_equations.end())
    throw InternalError("Equation already there! " + eqn->toString());

  assignEqnstoSymbols(solveFor,eqn);
  m_equations.insert(eqn);
}
/*****************************************************************************/

/*****************************************************************************/
void EquationSystem::eraseEquation( EquationPtr eqn)
/*****************************************************************************/
{
  EquationPtrSet::iterator ii = m_equations.find(eqn);
  if (ii == m_equations.end())
    throw InternalError("Equation not found! " + eqn->toString());

  m_equations.erase(ii);

  SymbolPtrElemMap& solveFor =  eqn->getSolveFor();
  for (SymbolPtrElemMap::iterator ii =solveFor.begin();ii!=solveFor.end();ii++)
  {
    if (ii->second.kind == ALL)
      m_symbolsequationmap.erase(ii->first);
    else
    {
      SymbolEquationMap::iterator is = m_symbolsequationmap.find(ii->first);
      if (is == m_symbolsequationmap.end())
        continue;
      for (SizeTPairSet::iterator ip = ii->second.elements.begin();ip!=ii->second.elements.end();ip++)
      {
        is->second.elements.erase(SizeTPair(ip->first,ip->second));
      }
      if (is->second.elements.size() == 0)
        m_symbolsequationmap.erase(is);
    }
  }
}
/*****************************************************************************/


/*****************************************************************************/
EquationSystemPtr EquationSystem::makeScalar()
/*****************************************************************************/
{
  // neuen Graphen Aufbauen
  EquationSystemPtr eqsys = new EquationSystem();
  BasicPtrMap symbolreplacemap;
  // erst alle Variablen uebertragen 
  for (SymbolStartValueMap::iterator ii=m_symbolsvaluemap.begin();ii!=m_symbolsvaluemap.end();++ii)
  {
    SymbolPtr s = (*ii).first;
    BasicPtr initval = (*ii).second;
    std::string comment = m_symbolscommentmap[s];
    s->setUserData(NULL,ID_UD_NODE);
    eqsys->addSymbol_Internal(s,initval.get(),comment);

    if (!s->is_Scalar())
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
  eqsys->m_states = m_states;
  eqsys->m_derstates = m_derstates;
  /* 
    ToDo:
      alle Gleichungen durchlaufen und Funktionen solve(Das stirbt sowieso), inverse, unknown ersetzen mit neuer Variable
  */
  SymbolRepl repl(symbolreplacemap);
  // dann equations
  for (EquationPtrSet::iterator e=m_equations.begin();e!=m_equations.end();e++)
  {
    if ((*e)->is_Implicit())
    {
      BasicPtrVec lhsscalar;
      BasicPtrVec rhsscalar;
      for (size_t l=0;l<(*e)->getRhsSize();l++)
      {
        BasicPtr lhs = (*e)->getLhs(l);
        BasicPtr rhs = (*e)->getRhs(l);

        if (lhs == rhs)
          continue;

        lhs = lhs->iterateExp(repl);
        lhs = lhs->simplify();
        rhs = rhs->iterateExp(repl);
        rhs = rhs->simplify();

        if ((lhs->getType() != Type_Matrix) || ((rhs->getType() != Type_Matrix) &&  (rhs->getType() != Type_Zero) &&  (rhs->getType() != Type_Eye)))
        {
          lhsscalar.push_back(lhs);
          rhsscalar.push_back(rhs);
        }
        else
        {
          size_t dim1 = rhs->getShape().getDimension(1);
          size_t dim2 = rhs->getShape().getDimension(2);
          for(size_t i=0;i<dim1;++i)
          {
            for(size_t j=0;j<dim2;++j)
            {
              lhsscalar.push_back(Element::New(lhs,i,j));
              rhsscalar.push_back(Element::New(rhs,i,j));
            }
          }
        }
      }
      if (lhsscalar.size() > 0)
        eqsys->addEquation(lhsscalar,rhsscalar,(*e)->is_Implicit());
    }
    else
    {
      for (size_t l=0;l<(*e)->getRhsSize();l++)
      {
        BasicPtr lhs = (*e)->getLhs(l);
        BasicPtr rhs = (*e)->getRhs(l);

        if (lhs == rhs)
          continue;

        std::string s1 = lhs->toString() + " = " + rhs->toString();

        lhs = lhs->iterateExp(repl);
        lhs = lhs->simplify();
        rhs = rhs->iterateExp(repl);
        rhs = rhs->simplify();

         std::string s2 = lhs->toString() + " = " + rhs->toString();


        if ((lhs->getType() != Type_Matrix) || ((rhs->getType() != Type_Matrix) &&  (rhs->getType() != Type_Zero) &&  (rhs->getType() != Type_Eye)))
        {
          if (lhs->is_Scalar())
            eqsys->addEquation(lhs,rhs,false);
          else
          {
            if (rhs->getType() == Type_Solve)
            {
              const Solve* solve = Util::getAsConstPtr<Solve>(rhs);
			  //geht IMHO so nicht, da die Matrix Multiplikation dann skalarisiert wird - ich wei� nicht wie ich daraus dann wieder matrizen basteln soll...
              //eqsys->addEquation(lhs,Add::New(Mul::New(solve->getArg1(),lhs),Neg::New(solve->getArg2())),true);
			  eqsys->addEquation(lhs,rhs,false);
            }
            else
              eqsys->addEquation(lhs,lhs-rhs,true);
          }
          continue;
        }

        size_t dim1 = rhs->getShape().getDimension(1);
        size_t dim2 = rhs->getShape().getDimension(2);
        for(size_t i=0;i<dim1;++i)
        {
          for(size_t j=0;j<dim2;++j)
          {
            eqsys->addEquation(Element::New(lhs,i,j),Element::New(rhs,i,j),false);
          }
        }
      }
    }
  }
  return eqsys;
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr EquationSystem::SymbolRepl::process_Arg(BasicPtr const &p)
/*****************************************************************************/
{
  if (p->getType() == Type_Symbol)
  {
    if (p->is_Scalar())
      return p;
    BasicPtrMap::iterator ii = m_symbolmap.find(p);
    if (ii != m_symbolmap.end())
      return ii->second;
  }
  return p;
}
/*****************************************************************************/

/*****************************************************************************/
SymbolPtr EquationSystem::getSymbol( std::string const& name)
/*****************************************************************************/
{
  // Symbol suchen
  StringSymbolMap::iterator ii = m_SymbolMap.find(name);
  // Auf Fehler pruefen
  if (ii == m_SymbolMap.end()) throw VarUnknownError(name);
  return ii->second;
}
/*****************************************************************************/

/*****************************************************************************/
SymbolPtrVec EquationSystem::getVariables(Symbol_Kind kind)
/*****************************************************************************/
{
  SymbolPtrVec res;
  if (kind & STATE)
  {
    for (SymbolPtrElemMap::iterator ii=m_states.begin();ii!=m_states.end();++ii)
    {
      if (ii->second.kind == SEVERAL)
        throw InternalError("not Supported feature Variable with several state elements!");
      res.push_back(ii->first);
    }
  }
  if (kind & DER_STATE)
  {
    for (SymbolPtrElemMap::iterator ii=m_derstates.begin();ii!=m_derstates.end();++ii)
    {
      if (ii->second.kind == SEVERAL)
        throw InternalError("not Supported feature Variable with several state elements!");
      res.push_back(ii->first);
    }
  }
  if (kind & PARAMETER)
  {
    for(SymbolPtrSet::iterator ii=m_parameter.begin();ii!=m_parameter.end();++ii)
      res.push_back((*ii));
  }
  if (kind & INPUT)
  {
    for(SymbolPtrSet::iterator ii=m_inputs.begin();ii!=m_inputs.end();++ii)
      res.push_back((*ii));
  }
  if (kind & CONTROLLER)
  {
    for(SymbolPtrSet::iterator ii=m_controller.begin();ii!=m_controller.end();++ii)
      res.push_back((*ii));
  }
  if (kind & VARIABLE)
  {
    for(SymbolPtrElemMap::iterator ii=m_variables.begin();ii!=m_variables.end();++ii)
    {
      if (ii->first->getName() == "time")
        continue;
      if (ii->second.kind == SEVERAL)
        throw InternalError("not Supported feature Variable with several state elements!");
      res.push_back(ii->first);
    }
  }
  if (kind & CONSTANT)
  {
    for(SymbolPtrSet::iterator ii=m_constants.begin();ii!=m_constants.end();++ii)
      res.push_back((*ii));
  }
  for(StringSymbolMap::iterator ii=m_SymbolMap.begin();ii!=m_SymbolMap.end();++ii)
  {
    if (ii->second->getName() == "time")
      continue;
    if (ii->second->getKind() & (CONSTANT | VARIABLE | INPUT | PARAMETER | DER_STATE | STATE | CONTROLLER))
      continue;
    if (ii->second->getKind() & kind)
    {
      res.push_back(ii->second);
    }
  }
  return res;
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr EquationSystem::getinitVal(SymbolPtr const& s)
/*****************************************************************************/
{
  BasicPtr ret = m_symbolsvaluemap[s];
  if (ret.get() == NULL) 
    throw InternalError("Symbol \"" + s->getName() + "\" has no initial value!");
  return ret;
}
/*****************************************************************************/

/*****************************************************************************/
Symbolics::BasicPtr EquationSystem::getEquation(SymbolPtr const& s)
/*****************************************************************************/
{
  SymbolEquationMap::iterator ii = m_symbolsequationmap.find(s);
  if (ii == m_symbolsequationmap.end())
    throw InternalError("Symbol " + s->toString() + " is not solved in a equation!");
  if (ii->second.kind == ALL)
  {
    if (ii->second.eqn.get() == NULL) throw VarUnknownError(s->getName());

    if (ii->second.eqn->getRhsSize() > 1) throw InternalError("getEquation " + s->getName() + " has more than one equation!");
    return ii->second.eqn->getRhs(0);
  }
  else
    throw InternalError("getEquation failed vor " + s->toString() + " because symbols is solved in several equations!");
}
/*****************************************************************************/

/*****************************************************************************/
std::string EquationSystem::getComment(SymbolPtr const& s)
/*****************************************************************************/
{
  return m_symbolscommentmap[s];
}
/*****************************************************************************/

/*****************************************************************************/
EquationPtr EquationSystem::combineEquations(EquationPtrVec equations)
/*****************************************************************************/
{
  SymbolPtrElemMap solveFor;
  BasicPtrVec lhs;
  BasicPtrVec rhs;
  bool implizit=false;
  for (size_t i=0;i<equations.size();i++)
  {
    EquationPtrSet::iterator ii = m_equations.find(equations[i]);
    if (ii == m_equations.end())
      throw InternalError("Equation not found! " + equations[i]->toString());
    SymbolPtrElemMap &esf = (*ii)->getSolveFor();
    for (SymbolPtrElemMap::iterator e=esf.begin();e!=esf.end();e++)
    {
      SymbolPtrElemMap::iterator f = solveFor.find(e->first);
      if (f==solveFor.end())
        solveFor[e->first] = e->second;
      else
      {
        if ((f->second.kind == ALL) || (e->second.kind == ALL))
          throw InternalError("Symbol " + f->first->getName() + " is more than once solved in expression");
        else if(f->second.kind == SEVERAL)
        {
          for (SizeTPairSet::iterator sa=f->second.elements.begin();sa!=f->second.elements.end();sa++)
          {
            std::pair<SizeTPairSet::iterator,bool> ret;
            ret = e->second.elements.insert(SizeTPair(sa->first,sa->second));
            if (ret.second == false)
              throw InternalError("Symbol " + f->first->getName() + " is more than once solved in expression");
          }
        }
      }
    }
    for (size_t e=0;e<(*ii)->getLhsSize();e++)
      lhs.push_back((*ii)->getLhs(e));
    for (size_t e=0;e<(*ii)->getRhsSize();e++)
      rhs.push_back((*ii)->getRhs(e));
  }
  for (size_t i=0;i<equations.size();i++)
  {
    EquationPtrSet::iterator ii = m_equations.find(equations[i]);
    m_equations.erase(ii);
  }
  EquationPtr neweqn(new Equation(solveFor,lhs,rhs,implizit));
  m_equations.insert(neweqn);
  return neweqn;
}
/*****************************************************************************/

/*****************************************************************************/
void EquationSystem::getSymbolOrDer( SymbolPtrElemMap & res,
  Symbolics::BasicPtr const& exp, bool inMatrix, bool inDer,
  bool inElement, size_t dim1, size_t dim2)
/*****************************************************************************/
{
  if (exp.get() == NULL)
  {
    if (inMatrix)
      throw InternalError("Symbolics::    getSymbolOrDer(): empty matrix element!");
    else
      return;
  }
  /* {a,der(b),c[?,?],der(d[?,?])} = exp */
  if (exp->getType() == Type_Matrix)
  {
    if (inElement)
    {
      const Matrix *mat = Util::getAsConstPtr<Matrix>(exp);
      getSymbolOrDer(res,mat->get(dim1,dim2),false,inDer);
    }
    else
    {
      size_t elements = exp->getArgsSize();
      for (size_t i=0;i<elements;i++)
        getSymbolOrDer(res,exp->getArg(i),true,inDer);
    }
    return;
  }
  /* der(a) = exp, der(a[?,?]) = exp */
  if (exp->getType() == Type_Der)
  {
    if (inDer)
      throw InternalError("der(der(x)) is not supported!");
    getSymbolOrDer(res,exp->getArg(0),inMatrix,true,inElement,dim1,dim2);
    return;
  }
  /* a[?,?] = exp */
  if (exp->getType() == Type_Element)
  {
    if (inElement)
      throw InternalError("Element(Element(..)) is not supported!");
    const Element *e = Util::getAsConstPtr< Element>(exp);
    getSymbolOrDer(res,e->getArg(0),inMatrix,inDer,true,e->getRow(),e->getCol());
    return;
  }
  /* a = exp */
  if (exp->getType() == Type_Symbol)
  {
    const Symbol *symbol = Util::getAsConstPtr< Symbol>(exp);
    Symbol *s = const_cast<Symbol*>(symbol);
    SymbolPtr sp( s );
    if (inDer)
    {
      if (inElement)
      {
        s->setState(dim1,dim2);
        SymbolPtr ders = s->getDerivative();
        addSymbol_Internal(ders, NULL, "");
        SymbolPtrElemMap::iterator ii = m_states.find(sp);
        if (ii == m_states.end())
        {
          m_states[sp] = SymbolElement(dim1,dim2);
          m_derstates[ders] = SymbolElement(dim1,dim2);
        }
        else
        {
          SizeTPair pair(dim1,dim2);
          ii->second.elements.insert(pair);
          SymbolPtrElemMap::iterator id = m_derstates.find(ders);
          if (id!=m_derstates.end())
            id->second.elements.insert(pair);
          else
            m_derstates[ders] = ii->second;
        }
        SymbolPtrElemMap::iterator iv = m_variables.find(sp);
        if (iv != m_variables.end())
        {
          if (iv->second.kind == ALL)
          {
            size_t dim1 = iv->first->getShape().getDimension(1);
            size_t dim2 = iv->first->getShape().getDimension(2);
            for(size_t i=0;i<dim1;++i)
            {
              for(size_t j=0;j<dim2;++j)
              {
                iv->second.elements.insert(SizeTPair(i,j));
              }
            }
            iv->second.kind = SEVERAL;
          }
          iv->second.elements.erase(SizeTPair(dim1,dim2));
        }
        sp = ders;
      }
      else
      {
        s->setState();
        SymbolPtr ders = s->getDerivative();
        addSymbol_Internal(ders, NULL, "");
        m_states[sp] = SymbolElement();
        m_derstates[ders] = SymbolElement();
        SymbolPtrElemMap::iterator iv = m_variables.find(sp);
        m_variables.erase(sp);
        sp = ders;
      }
    }
    SymbolPtrElemMap::iterator ik = res.find(sp);
    if (ik == res.end())
    {
      if (inElement)
      {
        res[sp] = SymbolElementT(dim1,dim2);
      }
      else
      {
        res[sp] = SymbolElementT();
      }
      return;
    }
    else
    {
      if (inElement)
      {
        if (ik->second.kind == ALL)
        {
          throw InternalError("Symbol " + s->getName() + "[" + str(dim1) + "," + str(dim2) + "] is more than once solved in expression");
        }
        else if (ik->second.kind == SEVERAL)
        {
          SizeTPair dims(dim1,dim2);
          SizeTPairSet::iterator ie = ik->second.elements.find(dims);
          if (ie != ik->second.elements.end())
          {
            throw InternalError("Symbol " + s->getName() + "[" + str(dim1) + "," + str(dim2) + "] is more than once solved in expression");
          }
          else
          {
            ik->second.elements.insert(dims);
            if (ik->second.elements.size() == ik->first->getShape().getNumEl())
            {
              ik->second.kind = ALL;
            }
            return;
          }
        }
      }
      else
        throw InternalError("Symbol " + s->getName() + " is more than once solved in expression");
    }
    return;
  }
  throw InternalError("lhs expression \"" + exp->toString() + "\" is neither Symbol nor Derivative!");
  return;
}
/*****************************************************************************/

/*****************************************************************************/
void EquationSystem::setVariable(SymbolPtr s, size_t dim1, size_t dim2)
/*****************************************************************************/
{
  s->setVariable(dim1,dim2);
  SymbolPtrElemMap::iterator ii = m_states.find(s);
  if (ii != m_states.end())
  {
    SymbolPtr ds = s->getDerivative();
    if (ii->first->is_Scalar())
    {
      if ((dim1 != 0) && (dim2 != 0))
        throw InternalError("Symbol " + s->toString() + " has no Dimension [" + str(dim1) + "," + str(dim2));
      m_states.erase(ii);
      m_variables[s] = SymbolElementT();
      m_derstates.erase(ds);
      m_variables[ds] = SymbolElementT();
    }
    else
    {
      if (ii->second.kind == ALL)
      {
        size_t dim1 = ii->first->getShape().getDimension(1);
        size_t dim2 = ii->first->getShape().getDimension(2);
        SymbolPtrElemMap::iterator id = m_derstates.find(ds);
        for(size_t i=0;i<dim1;++i)
        {
          for(size_t j=0;j<dim2;++j)
          {
            ii->second.elements.insert(SizeTPair(i,j));
            id->second.elements.insert(SizeTPair(i,j));
          }
        }
        ii->second.kind = SEVERAL;
        id->second.kind = SEVERAL;
      }
      SizeTPair p(dim1,dim2);
      SizeTPairSet::iterator ie = ii->second.elements.find(p);
      if (ie != ii->second.elements.end())
      {
        if (ii->second.elements.size() == 1)
        {
          m_states.erase(ii);
          m_variables[s] = SymbolElementT();
          m_derstates.erase(ds);
          m_variables[ds] = SymbolElementT();
        }
        else
        {
          ii->second.elements.erase(ie);
          m_variables[s].elements.insert(p);
          m_derstates[ds].elements.erase(p);
          m_variables[ds].elements.insert(p);
        }
      }
    }
  }
}
/*****************************************************************************/

/*****************************************************************************/
void EquationSystem::dumpSystem(std::string file)
/*****************************************************************************/
{
  ModelicaPrinter printer;
  std::ofstream f;
  f.open(file.c_str());
  f << "model Test" << std::endl;
  for (StringSymbolMap::iterator ii=m_SymbolMap.begin();ii!=m_SymbolMap.end();ii++)
  {
    if (ii->second->getName() == "time")
      continue;
    f << "Real " << (*ii).second->toString();
    Shape s = (*ii).second->getShape();

    if ((*ii).second->is_Scalar())
      f << "";
    else if ((*ii).second->is_Vector())
      f << "[" + str(s.getNumEl()) + "]";
    else
      f << "[" + str(s.getDimension(1)) + "," + str(s.getDimension(2)) + "]";
    if (ii->second->stateKind() == ALL)
    {
      if (ii->second->is_State(0,0) & STATE)
        f << "/* STATE */";
      else if (ii->second->is_State(0,0) & DER_STATE)
        f << "/* DER_STATE */";
    }
    else
    {
      size_t dim1=ii->second->getShape().getDimension(1);
      size_t dim2=ii->second->getShape().getDimension(2);
      f << "/* {";
      for (size_t i=0;i<dim1;i++)
      {
        for(size_t j=0;j<dim2;j++)
        {
          if (ii->second->is_State(i,j) & STATE)
            f << " STATE";
          else if (ii->second->is_State(i,j) & DER_STATE)
            f << " DER_STATE";
          if (j<dim2-1)
            f << ",";
        }
        if (i<dim1-1)
          f << ";";
       }
      f << " }*/";
    }
    f << ";" << std::endl;
  }

  f << "equation" << std::endl;
  size_t nr=0;
  for (EquationPtrSet::iterator ii=m_equations.begin();ii!=m_equations.end();ii++)
  {
    for (size_t l = 0;l<(*ii)->getRhsSize();l++)
    {
      BasicPtr lhs = (*ii)->getLhs(l);
      BasicPtr rhs = (*ii)->getRhs(l);
      if (rhs->getType() == Type_Solve)
      {
        Solve *s = Util::getAsPtr<Solve>(rhs);
        f << "  " << printer.print(s->getArg1()) << "*" << printer.print(lhs) << " = " << printer.print(s->getArg2()) << ";" << std::endl;
      }
      else
        f << "  " << printer.print(lhs) << " = " << printer.print(rhs) << ";" << std::endl;  
    }
  }
  f << "end Test;" << std::endl;
  f.close();
}
/*****************************************************************************/

/*****************************************************************************/
bool EquationSystem::InitValueScanner::process_Arg(BasicPtr const &p, bool &stop)
/*****************************************************************************/
{
  if (p->getType() == Type_Symbol)
  {
    const Symbol *sconst = Util::getAsConstPtr<Symbol>(p);
    static Symbol_Kind cat = CONSTANT|PARAMETER;
    if (!(sconst->getKind() & cat))
    {
      variable=true;
      return false;
    }
  }
  return true;
}
/*****************************************************************************/

/*****************************************************************************/
bool EquationSystem::DerivativeScanner::process_Arg(BasicPtr const &p, bool &stop)
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
  else if (p->getType() == Type_Element)
  {
    const Element *econst = Util::getAsConstPtr<Element>(p);
    inelement = true;
    dim1 = econst->getRow();
    dim2 = econst->getCol();
    p->getArg(0)->scanExp(*this);
    inelement = false;
    return false;
  }
  else if (p->getType() == Type_Symbol)
  {
    const Symbol *sconst = Util::getAsConstPtr<Symbol>(p);
    SymbolPtr sp(const_cast<Symbol*>(sconst));
    addAtom(sp);
    if (inder)
    {
      process_derArg(p);
    }
    return false;
  }
  return true;
}
/*****************************************************************************/

/*****************************************************************************/
void EquationSystem::DerivativeScanner::process_derArg(BasicPtr const &p)
/*****************************************************************************/
{
  const Symbol *sconst = Util::getAsConstPtr<Symbol>(p);
  Symbol *s = const_cast<Symbol*>(sconst);
  if (inelement)
  {
    s->setState(dim1,dim2);
    SymbolPtr sd(s->getDerivative());
    addAtom(sd);
    SymbolPtrElemMap::iterator ii = states.find(s);
    if (ii != states.end())
    {
      if (ii->second.kind != ALL)
        ii->second.elements.insert(SizeTPair(dim1,dim2));
    }
    else
      states[s] = SymbolElement(dim1,dim2);
  }
  else
  {
    s->setState();
    SymbolPtr sd(s->getDerivative());
    addAtom(sd);
    SymbolPtrElemMap::iterator ii = states.find(s);
    if (ii != states.end())
    {
      if (ii->second.kind != ALL)
        ii->second.kind = ALL;
    }
    else
      states[s] = SymbolElement();
  }
}
/*****************************************************************************/

/*****************************************************************************/
void EquationSystem::DerivativeScanner::addAtom(SymbolPtr sp)
/*****************************************************************************/
{
  SymbolPtrElemMap::iterator ii = atoms.find(sp);
  if (ii == atoms.end())
  {
    if (inelement)
    {
      atoms[sp] = SymbolElement(dim1,dim2);
    }
    else
    {
      atoms[sp] = SymbolElement();
    }
  }
  else
  {
    if (inelement)
    {
      ii->second.elements.insert(SizeTPair(dim1,dim2));
      if (ii->second.elements.size() == ii->first->getShape().getNumEl())
      {
        ii->second.kind = ALL;
      }
      return;
    }
    else
    {
      ii->second.kind = ALL;
    }
  }
}
/*****************************************************************************/
