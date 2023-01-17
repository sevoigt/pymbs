#include "Assignments.h"



/*****************************************************************************/
Symbolics::Graph::Assignment::Assignment( NodePtr node)
/*****************************************************************************/
{
  if (node.get() == NULL) throw InternalError("Node is invalid!");
  category = node->get_Category();
  implizit = node->is_Implicit();
  for(size_t i=0;i<node->getLhsSize();++i)
    lhs.push_back(node->getLhs(i));
  for(size_t i=0;i<node->getRhsSize();++i)
    rhs.push_back(node->getRhs(i));
}
/*****************************************************************************/

/*****************************************************************************/
Symbolics::Graph::Assignment::~Assignment()
/*****************************************************************************/
{
  rhs.clear();
  lhs.clear();
}
/*****************************************************************************/


/*****************************************************************************/
Symbolics::Graph::Assignments::Assignments(std::vector<NodePtr> const& nodes):
m_refCount(0)
/*****************************************************************************/
{
  std::set<SymbolPtr> vars;
  Category_Type check = Symbolics::PARAMETER | Symbolics::CONSTANT;
  for (std::vector<NodePtr>::const_iterator ii =nodes.begin(); ii != nodes.end();++ii)
  {
    SymbolPtrElemSizeTMap &symbols = (*ii)->getSymbols();
    for (SymbolPtrElemSizeTMap::const_iterator jj =symbols.begin(); jj != symbols.end();++jj)
      if (vars.find(jj->first) == vars.end())
      {
        m_symbols.push_back(jj->first);
        vars.insert(jj->first);
        if (jj->first->stateKind() == ALL)
        {
          if (jj->first->is_State(0,0) & DER_STATE)
          {
            SymbolPtr s = static_cast<Symbol*>(jj->first->getUserData(ID_UD_STATE));
            if (s.get() != NULL)
            {
              if (vars.find(s) == vars.end())
              {
                m_symbols.push_back(s);
                vars.insert(s);
              }
            }
            else
              throw InternalError("Statederivative \"" + jj->first->getName() + "\" has no State!");
          }
        }
        else
        {
          throw InternalError("Feature not supported!");
        }
      }


      // exclude params and constants
      Category_Type c = (*ii)->get_Category();
      if (!(c & check ))
      {
        Assignment e((*ii));
        m_assignments.push_back(e);
      }
  }    
}
/*****************************************************************************/

/*****************************************************************************/
Symbolics::Graph::Assignments::~Assignments()
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
Symbolics::Graph::VariableVec Symbolics::Graph::Assignments::getVariables(Symbolics::Graph::Category_Type Category) const
/*****************************************************************************/
{
  VariableVec res;

  if (Category & (STATE | DER_STATE))
  {
    for (SymbolPtrVec::const_iterator ii=m_symbols.begin();ii!=m_symbols.end();++ii)
    {
      if ((*ii)->stateKind() == ALL)
      {
        if ((*ii)->is_State(0,0) & Category)
          res.push_back((*ii));
      }
      else
        throw InternalError("Feature not Implemented");
    }
  }
  for (SymbolPtrVec::const_iterator ii=m_symbols.begin();ii!=m_symbols.end();++ii)
  {
    if ((*ii)->stateKind() == ALL)
    {
      if ((*ii)->is_State(0,0) & (STATE | DER_STATE))
        continue;
    }
    else
        throw InternalError("Feature not Implemented");
    if (((*ii)->getKind() & Category))
        if ((*ii)->getName() != "time")
            res.push_back((*ii));
  }
  return res;
}
/*****************************************************************************/

/*****************************************************************************/
std::vector<Symbolics::Graph::Assignment> Symbolics::Graph::Assignments::getEquations(Symbolics::Graph::Category_Type exclude) const
/*****************************************************************************/
{
  std::vector<Symbolics::Graph::Assignment> res;

  for (std::vector<Symbolics::Graph::Assignment>::const_iterator ii=m_assignments.begin();ii!=m_assignments.end();++ii)
  {
    if (!((*ii).category & exclude))
      res.push_back((*ii));
  }
  return res;
}
/*****************************************************************************/
