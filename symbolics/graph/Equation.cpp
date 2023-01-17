#include <map>
#include "Equation.h"
#include "str.h"

using namespace Symbolics;
using namespace Graph;

/*****************************************************************************/
Equation::Equation(SymbolPtrElemMap const& SolveFor,
  BasicPtr const& Lhs, 
  BasicPtr const& Rhs, bool implizit):
m_implizit(implizit),m_category(0),m_numElem(0),m_refCount(0)
/*****************************************************************************/
{
  setSolveFor(SolveFor);
  m_lhs.push_back(Argument(Lhs->simplify()));
  if (Rhs.get() == NULL) throw InternalError("Equation: Value of Rhs is not Valid!");
  m_rhs.push_back(Argument(Rhs->simplify()));
  m_numElem = Rhs->getShape().getNumEl();
  findSymbols();
}
/*****************************************************************************/

/*****************************************************************************/
Equation::Equation(SymbolPtrElemMap const& SolveFor, 
  BasicPtrVec const&  Lhs, BasicPtrVec const& Rhs, bool implizit):
m_implizit(implizit),m_category(0),m_numElem(0),m_refCount(0)
/*****************************************************************************/
{
  setSolveFor(SolveFor);
  m_lhs.reserve(Lhs.size());
  for (size_t i=0;i<Lhs.size();++i)
    m_lhs.push_back(Argument(Lhs[i]->simplify()));
  m_rhs.reserve(Rhs.size());
  for (size_t i=0;i<Rhs.size();++i)
  {
    m_rhs.push_back(Argument(Rhs[i]->simplify()));
    m_numElem += Rhs[i]->getShape().getNumEl();
  }
  if (Lhs.size() != Rhs.size())
    throw InternalError("Number of Lhs not equal to number of Rhs in Equation " + toString());
  findSymbols();
}
/*****************************************************************************/

/*****************************************************************************/
Equation::~Equation()
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
void Equation::setSolveFor(SymbolPtrElemMap const& SolveFor)
/*****************************************************************************/
{
  for (SymbolPtrElemMap::const_iterator ii=SolveFor.begin();ii!=SolveFor.end();ii++)
  {
    m_solveFor[ii->first] = ii->second;
    m_category = m_category | ii->first->getKind();
    if (ii->first->stateKind() == ALL)
    {
      if (ii->first->is_State(0,0) & STATE)
        m_category = m_category | STATE;
      if (ii->first->is_State(0,0) & DER_STATE)
        m_category = m_category | DER_STATE;
    }
    else
    {
      for (SizeTPairSet::const_iterator ie=ii->second.elements.begin();ie!=ii->second.elements.end();ie++)
      {
        // set state once is enough
        if (ii->first->is_State(ie->first,ie->second) & STATE)
        {
          m_category = m_category | STATE;
        }
        if (ii->first->is_State(ie->first,ie->second) & DER_STATE)
        {
          m_category = m_category | DER_STATE;
        }
      }
    }
  }
}
/*****************************************************************************/


/*****************************************************************************/
void Equation::findSymbols()
/*****************************************************************************/
{
  m_symbols.clear();
  DerivativeScanner scanner(m_symbols);
  for (size_t i=0;i<m_rhs.size();++i)
  {
    m_rhs[i].getArg()->scanExp(scanner);
  }
  for (size_t i=0;i<m_lhs.size();++i)
  {
    m_lhs[i].getArg()->scanExp(scanner);
  }
}
/*****************************************************************************/

/*****************************************************************************/
void Equation::subs(ConstBasicPtr const& old_exp, BasicPtr const& new_exp)
/*****************************************************************************/
{
  for (size_t i=0;i<m_rhs.size();++i)
  {
    m_rhs[i].setArg(m_rhs[i].getArg()->subs(old_exp,new_exp));
  }  
  // Was ist mit SolveFor?
  for (size_t i=0;i<m_lhs.size();++i)
  {
    m_lhs[i].setArg(m_lhs[i].getArg()->subs(old_exp,new_exp));
  }  
}
/*****************************************************************************/

/*****************************************************************************/
void Equation::simplify( )
/*****************************************************************************/
{
    for (size_t i=0;i<m_lhs.size();++i)
        m_lhs[i].setArg(m_lhs[i].getArg()->simplify());
    for (size_t i=0;i<m_rhs.size();++i)
        m_rhs[i].setArg(m_rhs[i].getArg()->simplify());
}
/*****************************************************************************/

/*****************************************************************************/
void Equation::solve(BasicPtr const& exp)
/*****************************************************************************/
{
  SymbolPtrElemMap solveFor;
  SymbolScanner scanner(solveFor,true);
  exp->scanExp(scanner);
  for (SymbolPtrElemMap::iterator jj=solveFor.begin();jj!=solveFor.end();jj++)
  {
    SymbolPtrElemSizeTMap::iterator ii = m_symbols.find(jj->first);
    if (ii == m_symbols.end())
      throw InternalError("Cannot solve Equation " + toString() + " for " + exp->toString());
  }
  // check shape
  if (exp->getShape().getNumEl() != m_numElem)
    throw InternalError("Equation " + toString() + " is not solvable for Expression " + exp->toString());
  // reset SolveFor and Category
  m_category = 0;
  m_solveFor.clear();
  setSolveFor(solveFor);

  if (getRhsSize() == 1)
  {
    if (m_lhs[0].getArg() == exp)
    {
      Basic::LookFor sc(exp);
      m_rhs[0].getArg()->scanExp(sc);
      if (!sc.found)
      {
        m_implizit = false;
        return;
      }
    }
    if (m_rhs[0].getArg() == exp)
    {
      Basic::LookFor sc(exp);
      m_lhs[0].getArg()->scanExp(sc);
      if (!sc.found)
      {
        m_implizit = false;
        m_rhs[0].setArg(m_lhs[0].getArg());
        m_lhs[0].setArg(exp);
        return;
      }
    }
    BasicPtr eqn = m_lhs[0].getArg() - m_rhs[0].getArg();
    BasicPtr solve;
    bool isDerivative = false;
    BasicPtr derexp;
    if (exp->getType() == Type_Symbol)
    {
      const Symbol *sconst = Util::getAsConstPtr<Symbol>(exp);
      /* TODO: L�sung f�r Solve(DER...) als Symbol mit nur einigen DER_STATES) */
      if (sconst->stateKind() == ALL)
      {
        if (sconst->is_State(0,0) & DER_STATE)
        {
          Symbol *s = static_cast<Symbol*>(sconst->getUserData(ID_UD_STATE));
          if (s == NULL)
            throw InternalError("Derivative without state " + sconst->toString());
          isDerivative = true;
          derexp = Der::New(BasicPtr(s));
          if (m_lhs[0].getArg() == derexp)
          {
            Basic::LookFor sc(derexp);
            m_rhs[0].getArg()->scanExp(sc);
            if (!sc.found)
            {
              m_implizit = false;
              return;
            }
          }
          if (m_rhs[0].getArg() == derexp)
          {
            Basic::LookFor sc(derexp);
            m_lhs[0].getArg()->scanExp(sc);
            if (!sc.found)
            {
              m_implizit = false;
              m_rhs[0].setArg(m_lhs[0].getArg());
              m_lhs[0].setArg(derexp);
              return;
            }
          }
          solve = eqn->solve(derexp);
        }
        else
        {
          solve = eqn->solve(exp);
        }
      }
      else
      {
        Symbol *s = static_cast<Symbol*>(sconst->getUserData(ID_UD_STATE));
        if (s == NULL)
          throw InternalError("Derivative without state " + sconst->toString());
        Matrix *mat = new Matrix(sconst->getShape());
        size_t dim1 = sconst->getShape().getDimension(1);
        size_t dim2 = sconst->getShape().getDimension(2);
        for(size_t i=0;i<dim1;++i)
        {
          for(size_t j=0;j<dim2;++j)
          {
            if (sconst->is_State(i,j) & DER_STATE)
              mat->set(i,j,Der::New(Element::New(BasicPtr(s),i,j)));
            else
            {
              mat->set(i,j, Element::New(exp,i,j));
            }
          }
        }
        isDerivative = true;
        derexp = BasicPtr(mat);
        if (m_lhs[0].getArg() == derexp)
        {
          Basic::LookFor sc(derexp);
          m_rhs[0].getArg()->scanExp(sc);
          if (!sc.found)
          {
            m_implizit = false;
            return;
          }
        }
        if (m_rhs[0].getArg() == derexp)
        {
          Basic::LookFor sc(derexp);
          m_lhs[0].getArg()->scanExp(sc);
          if (!sc.found)
          {
            m_implizit = false;
            m_rhs[0].setArg(m_lhs[0].getArg());
            m_lhs[0].setArg(derexp);
            return;
          }
        }
        solve = eqn->solve(derexp);
      }    
    }
    else
      solve = eqn->solve(exp);
    if (solve.get() == NULL)
    {
      m_implizit = true;
    }
    else
    {
      if (isDerivative)
        m_lhs[0].setArg(derexp);
      else
        m_lhs[0].setArg(exp);
      m_rhs[0].setArg(solve);
      m_implizit = false;
    }
  }
  else
  {
    m_implizit = true;
  }
}
/*****************************************************************************/

/*****************************************************************************/
EquationPtr Equation::diff()
/*****************************************************************************/
{
  BasicPtrVec lhs;
  BasicPtrVec rhs;
  DerRepl dr;
  for (size_t i=0;i<m_lhs.size();i++)
  {
    BasicPtr dlhs = m_lhs[i].getArg()->iterateExp(dr);
    lhs.push_back(dlhs->der());
  }
  for (size_t i=0;i<m_rhs.size();i++)
  {
    BasicPtr drhs = m_rhs[i].getArg()->iterateExp(dr);
    rhs.push_back(drhs->der());
  }
  return EquationPtr(new Equation(SymbolPtrElemMap(),lhs,rhs,false));
}
/*****************************************************************************/

/*****************************************************************************/
void Equation::iterateEquation(Basic::Iterator &v)
/*****************************************************************************/
{
  for (size_t i=0;i<m_lhs.size();i++)
  {
    m_lhs[i].setArg(m_lhs[i].getArg()->iterateExp(v));
  }
  for (size_t i=0;i<m_rhs.size();i++)
  {
    m_rhs[i].setArg(m_rhs[i].getArg()->iterateExp(v));
  }
}
/*****************************************************************************/

/*****************************************************************************/
std::string Equation::toString()
/*****************************************************************************/
{
  if (getRhsSize() == 1)
    return m_lhs[0].getArg()->toString() + " = " + m_rhs[0].getArg()->toString() + ";\n";
  else
  {
    std::string res;
    for(size_t i = 0;i < getRhsSize();++i)
    {
      if (i>0)
        res += ";\n";
      res += m_lhs[i].getArg()->toString() + " = " + m_rhs[i].getArg()->toString();
    }
    return res;
  }
}
/*****************************************************************************/

/*****************************************************************************/
bool Equation::DerivativeScanner::process_Arg(BasicPtr const &p, bool &stop)
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
  /*
   DoTo: bei Element(MatrixSymbol*VectorSymbol) geht das hier schief
   Eine L�sung w�re erst die Element exp zu elementarisieren (hoffnung auf Element(Matrix)) 
  */
  else if (p->getType() == Type_Element)
  {
    if (inelement)
      throw InternalError("Error: element(element(exp)) is not supported!");
    const Element *e= Util::getAsConstPtr<Element>(p);
    dim1 = e->getRow();
    dim2 = e->getCol();
    inelement = true;
    p->getArg(0)->scanExp(*this);
    inelement = false;
    return false;
  }
  else if (p->getType() == Type_Symbol)
  {
    SymbolPtr sp;
    if (inder)
    {
      const Symbol *sconst = Util::getAsConstPtr<Symbol>(p);
      sp = const_cast<Symbol*>(sconst)->getDerivative();
    }
    else
    {
      const Symbol *sconst = Util::getAsConstPtr<Symbol>(p);
      sp = const_cast<Symbol*>(sconst);
    }
    if (sp.get() == NULL)
      throw InternalError("Error while scann expression \"" + p->toString() + "\" for symbols and cast Symbol");
    if (inelement)
    {
      SymbolPtrElemSizeTMap::iterator ii = atoms.find(sp);
      if (ii != atoms.end())
      {
        SizeTPair dim(dim1,dim2);
        SizeTPairSizeTMap::iterator ie = ii->second.elements.find(dim);
        if (ie != ii->second.elements.end())
        {
          ie->second++;
        }
        else
          ii->second.elements[dim] = 1;
      }
      else
      {
        atoms[sp] = SymbolElementSizeT(dim1,dim2);
      }
    }
    else
    {
      SymbolPtrElemSizeTMap::iterator ii = atoms.find(sp);
      if (ii != atoms.end())
      {
        ii->second.counter++;
      }
      else
      {
        atoms[sp] = SymbolElementSizeT();
      }
    }
    return false;
  }
  return true;
}
/*****************************************************************************/


/*****************************************************************************/
bool Equation::SymbolScanner::process_Arg(BasicPtr const &p, bool &stop)
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
    if (inelement)
      throw InternalError("Error: element(element(exp)) is not supported!");
    const Element *e= Util::getAsConstPtr<Element>(p);
    dim1 = e->getRow();
    dim2 = e->getCol();
    inelement = true;
    p->getArg(0)->scanExp(*this);
    inelement = false;
    return false;
  }
  else if (p->getType() == Type_Symbol)
  {
    Symbol *s=NULL;
    const Symbol *sconst = Util::getAsConstPtr<Symbol>(p);
    if (inder)
    {
      s = const_cast<Symbol*>(sconst)->getDerivative();
    }
    else
    {
      s = const_cast<Symbol*>(sconst);
    }
    if (s == NULL)
      throw InternalError("Error while scann expression and cast Symbol");
    if (inelement)
    {
      SymbolPtrElemMap::iterator ii = atoms.find(SymbolPtr(s));
      if (ii != atoms.end())
      {
        if (ii->second.kind == ALL)
        {
          if (errorifmorethanonce)
            throw InternalError("Symbol " + s->getName() + " is more than once solved in expression");
        }
        else if (ii->second.kind == SEVERAL)
        {
          SizeTPair dims(dim1,dim2);
          SizeTPairSet::iterator ie = ii->second.elements.find(dims);
          if (ie != ii->second.elements.end())
          {
            if (errorifmorethanonce)
              throw InternalError("Symbol " + s->getName() + " is more than once solved in expression");
          }
          else
            ii->second.elements.insert(dims);
        }
      }
      else
        atoms[SymbolPtr(s)] = SymbolElement(dim1,dim2);
    }
    else
    {
      SymbolPtrElemMap::iterator ii = atoms.find(SymbolPtr(s));
      if (ii != atoms.end())
      {
        if (errorifmorethanonce)
          throw InternalError("Symbol " + s->getName() + " is more than once solved in expression");
      }
      else
        atoms[SymbolPtr(s)] = SymbolElement();
    }
    return false;
  }
  return true;
}
/*****************************************************************************/


/*****************************************************************************/
BasicPtr Equation::DerRepl::process_Arg(BasicPtr const &p)
/*****************************************************************************/
{
  if (p->getType() == Type_Der)
  {
    if (p->getArg(0)->getType() == Type_Symbol)
    {
      const Symbol *sconst = Util::getAsConstPtr<Symbol>(p->getArg(0));
      Symbol *s = static_cast<Symbol*>(sconst->getUserData(ID_UD_STATEDERIVATIVE));
      if (s != NULL)
      {
        return SymbolPtr(s);
      }
    }
    if (p->getArg(0)->getType() == Type_Element)
    {
      const Element *econst = Util::getAsConstPtr<Element>(p->getArg(0));
      if (econst->getArg(0)->getType() == Type_Symbol)
      {
        const Symbol *sconst = Util::getAsConstPtr<Symbol>(econst->getArg(0));
        Symbol *s = static_cast<Symbol*>(sconst->getUserData(ID_UD_STATEDERIVATIVE));
        if (s != NULL)
        {
          return Element::New(SymbolPtr(s),econst->getRow(),econst->getCol());
        }
      }
    }
  }
  else if (p->getType() == Type_Element)
  {
    const Element *econst = Util::getAsConstPtr<Element>(p);
    if (econst->getArg(0)->getType() == Type_Der)
    {
      if (econst->getArg(0)->getArg(0)->getType() == Type_Symbol)
      {
         const Symbol *sconst = Util::getAsConstPtr<Symbol>(econst->getArg(0)->getArg(0));
         Symbol *s = static_cast<Symbol*>(sconst->getUserData(ID_UD_STATEDERIVATIVE));
         if (s != NULL)
         {
           return Element::New(SymbolPtr(s),econst->getRow(),econst->getCol());
         }
      }
    }
  }
  return p;
}
/*****************************************************************************/