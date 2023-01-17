#include "Graph.h"
#include "str.h"
#include "MatchedSystem.h"
#include "UnMatchedSystem.h"
#include "PreOptimisation.h"
#include "PastOptimisation.h"

#include <iostream>
#include <fstream>


/*****************************************************************************/
Symbolics::Graph::Graph::Graph() 
/*****************************************************************************/
{
  // open new Scope
  Util::newScope();
  // add symbol time
  SymbolPtr t = new Symbol("time");
  eqsys = new EquationSystem();
  eqsys->addSymbol(t);
  m_time = BasicPtr(t);
}
/*****************************************************************************/

/*****************************************************************************/
Symbolics::Graph::Graph::~Graph()
/*****************************************************************************/
{ 
  NodeVec::iterator ie = m_nodes.end();
  for (NodeVec::iterator ii =m_nodes.begin(); ii != ie;++ii)
  {
    (*ii)->clear();
  }
}
/*****************************************************************************/


/*****************************************************************************/
Symbolics::BasicPtr Symbolics::Graph::Graph::addSymbol( SymbolPtr const& s , Symbolics::Basic *initalValue, std::string comment)
/*****************************************************************************/
{
  return eqsys->addSymbol(s,initalValue,comment);
}
/*****************************************************************************/

/*****************************************************************************/
void Symbolics::Graph::Graph::addExpression(BasicPtr const& solveFor,
  BasicPtr const& exp, 
  bool implicit)
/*****************************************************************************/
{
  eqsys->addEquation(solveFor,exp,implicit);
}
/*****************************************************************************/


/*****************************************************************************/
void Symbolics::Graph::Graph::addExpression( BasicPtrVec const& solveFor, 
  BasicPtrVec const& exp, 
  bool implicit)
/*****************************************************************************/
{
  eqsys->addEquation(solveFor,exp,implicit);
}
/*****************************************************************************/


/*****************************************************************************/
Symbolics::SymbolPtr Symbolics::Graph::Graph::getSymbol( std::string const& name)
/*****************************************************************************/
{
  return eqsys->getSymbol(name);
}
/*****************************************************************************/


/*****************************************************************************/
void Symbolics::Graph::Graph::makeScalar()
/*****************************************************************************/
{
  // neuen Graphen Aufbauen
  eqsys = eqsys->makeScalar();
  m_nodes.clear();
}
/*****************************************************************************/

/*****************************************************************************/
Symbolics::Graph::AssignmentsPtr Symbolics::Graph::Graph::getAssignments(Category_Type inc,
  Category_Type exclude)
/*****************************************************************************/
{
  // get required nodes
  NodeVec reqnodes;
  NodeCollector v(exclude,reqnodes);
  for (size_t i=0;i<m_nodes.size();++i)
  {
    if (m_nodes[i]->visited == false)
    {
      Category_Type c = m_nodes[i]->get_Category();
      if( ( c & inc ) && ( !( c & exclude) ) )
      {
        // get other nodes
        Node::node_iterate_depth_first(m_nodes[i],v);
      }
    }
  }
  // reset visited
  for (size_t i=0;i<m_nodes.size();++i)
  {
    m_nodes[i]->visited = false;
  }

  // build equations
  return AssignmentsPtr(new Assignments(reqnodes));
}
/*****************************************************************************/

/*****************************************************************************/
Symbolics::SymbolPtrVec Symbolics::Graph::Graph::getVariables(Symbol_Kind kind)
/*****************************************************************************/
{
  return eqsys->getVariables(kind);
}
/*****************************************************************************/

/*****************************************************************************/
Symbolics::BasicPtr Symbolics::Graph::Graph::getinitVal(SymbolPtr const& s)
/*****************************************************************************/
{
  return eqsys->getinitVal(s);
}
/*****************************************************************************/

/*****************************************************************************/
Symbolics::BasicPtr Symbolics::Graph::Graph::getEquation(SymbolPtr const& s)
/*****************************************************************************/
{
  return eqsys->getEquation(s);
}
/*****************************************************************************/

/*****************************************************************************/
std::string Symbolics::Graph::Graph::getComment(SymbolPtr const& s)
/*****************************************************************************/
{
  return eqsys->getComment(s);
}
/*****************************************************************************/

/*****************************************************************************/
size_t Symbolics::Graph::Graph::getDerivativeOrder(SymbolPtr const& s)
/*****************************************************************************/
{
  size_t order=0;
  DerivativeOrderMap::iterator dk = m_derivativeOrder.find(s);
  if (dk != m_derivativeOrder.end())
    order = dk->second.first;
  return order;
}
/*****************************************************************************/

/*****************************************************************************/
void Symbolics::Graph::Graph::toGraphML( std::string file )
/*****************************************************************************/
{
  if (m_syshandler.get() != NULL)
    m_syshandler->toGraphML(file);
}
/*****************************************************************************/

/*****************************************************************************/
double Symbolics::Graph::Graph::buildGraph(bool optimize)
/*****************************************************************************/
{
  double t1 = Util::getTime();
  double t2 = 0;
  if (optimize)
  {
    PreOptimisation preopt(eqsys);
    preopt.optimize();
  }
  if (eqsys->is_Balanced())
  {
    m_syshandler = new MatchedSystem(eqsys,m_nodes);
  }
  else
  {
    m_syshandler = new UnMatchedSystem(eqsys,m_nodes);
  }
  m_syshandler->buildGraph();
  if (optimize)
  {
    PastOptimisation pastopt(eqsys,m_nodes);
    pastopt.optimize();
  }
  //SymbolMap& states = eqsys->getStates();
  //for (SymbolMap::iterator ii = states.begin();ii!=states.end();ii++)
  //{
  //  m_derivativeOrder[(ii)->first] = std::pair<size_t, SymbolPtr>(1,(ii)->second);
  //}
  //// get DerivativeOrder
  //SymbolMap& derStates = eqsys->getDerStates();
  //for (SymbolMap::iterator ii = derStates.begin();ii!=derStates.end();ii++)
  //{
  //  SymbolNodeElement *elem = static_cast<SymbolNodeElement*>(ii->first->getUserData(ID_UD_NODE));
  //  if (elem != NULL)
  //  {
  //    if (elem->kind != ALL)
  //      continue;
  //    SymbolPtrElemSizeTMap &psyms = elem->node->getSymbols();
  //    if (psyms.size() == 2)
  //    {
  //      SymbolPtr x;
  //      SymbolPtr dx;
  //      for (SymbolPtrElemSizeTMap::iterator ik=psyms.begin();ik!=psyms.end();ik++)
  //      {
  //        if ((ik->first->getKind() & STATE) && (ik->second.counter == 1) && (ik->second.kind==ALL))
  //        {
  //          dx = ik->first;
  //        }
  //        if ((ik->first->getKind() & DER_STATE) && (ik->second.counter == 1) && (ik->second.kind==ALL))
  //        {
  //          x = ik->first;
  //        }
  //      }
  //      if ((x.get() != NULL) && (dx.get() != NULL))
  //      {
  //        Symbol *s = static_cast<Symbol*>((ii)->first->getUserData(ID_UD_STATE));
  //        if (s != NULL)
  //        {
  //          std::cout << "State and der " << s->toString() << " " << x->toString() << std::endl;
  //          DerivativeOrderMap::iterator ik = m_derivativeOrder.find(s);
  //          if (ik != m_derivativeOrder.end())
  //          {
  //            if (ik->second.second->getKind() != DER_STATE)
  //              std::cout << "State with two der " << s->toString() << " " << x->toString() << " or " << ik->second.second->toString() << std::endl;
  //          }
  //          DerivativeOrderMap::iterator dk = m_derivativeOrder.find(dx);
  //          size_t o = 1;
  //          if (dk != m_derivativeOrder.end())
  //            o = dk->second.first;
  //          m_derivativeOrder[s] = std::pair<size_t, SymbolPtr>(o+1,dx);
  //        }
  //      }
  //    }
  //  }
  // }
  //return t2-t1;
  return Util::getTime()-t1;
}
/*****************************************************************************/

/*****************************************************************************/
Symbolics::Graph::Graph::NodeCollector::NodeCollector(Category_Type exclude,
  NodeVec& reqnodes):
m_exclude(exclude),m_nodes(reqnodes)
/*****************************************************************************/
{

}
/*****************************************************************************/

/*****************************************************************************/
Symbolics::Graph::Graph::NodeCollector::~NodeCollector()
/*****************************************************************************/
{}
/*****************************************************************************/

/*****************************************************************************/
void Symbolics::Graph::Graph::NodeCollector::process_Node(NodePtr p)
/*****************************************************************************/
{
  if( !(  p->get_Category() & m_exclude) ) 
  {
    m_nodes.push_back(p);
  }
}
/*****************************************************************************/
