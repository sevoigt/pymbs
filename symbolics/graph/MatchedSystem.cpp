#include "MatchedSystem.h"
#include "str.h"
#include <iostream>
#include <fstream>

using namespace Symbolics;
using namespace Graph;

/*****************************************************************************/
MatchedSystem::MatchedSystem(EquationSystemPtr eqsys, NodeVec& nodes): 
SystemHandler(eqsys,nodes)
/*****************************************************************************/
{
  m_time = m_eqsys->getSymbol("time");

  const EquationPtrSet &eqns = m_eqsys->getEquations();
  for (EquationPtrSet::const_iterator i = eqns.begin();i != eqns.end(); i++)
  {
    NodePtr node = new Node(*i,m_time);
    m_nodes.push_back(node);
  }
}
/*****************************************************************************/

/*****************************************************************************/
MatchedSystem::~MatchedSystem()
/*****************************************************************************/
{ 
}
/*****************************************************************************/

/*****************************************************************************/
double MatchedSystem::buildGraph()
/*****************************************************************************/
{
  double t1 = Util::getTime();

  for (size_t i=0;i<m_nodes.size();++i)
  {
    m_nodes[i]->findParentsNodes();
  }
  //return t2-t1;
  return Util::getTime()-t1;
}
/*****************************************************************************/

/*****************************************************************************/
void MatchedSystem::toGraphML( std::string file )
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
  // write nodes
  size_t id=0;
  std::map<std::string,size_t> symbolidmap;
  EquationSystem::StringSymbolMap symbolmap = m_eqsys->getSymbols();
  for (EquationSystem::StringSymbolMap::iterator ii =symbolmap.begin(); ii != symbolmap.end();++ii)
  {
    f << "    <node id=\"n" << id << "\">" << std::endl;
    f << "      <data key=\"d5\"/>" << std::endl;
    f << "      <data key=\"d6\">" << std::endl;
    f << "        <y:ShapeNode>" << std::endl;
    f << "          <y:Geometry height=\"30.0\" width=\"30.0\" x=\"1.0\" y=\"1.0\"/>" << std::endl;
    std::string color = "#000000";
    Category_Type cat =  (*ii).second->getKind();
    if (cat & STATE)
      color = "#FF0000";
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
    if ((*ii).second.get() != NULL)
    {
      SymbolNodeElement *elem = static_cast<SymbolNodeElement*>((*ii).second->getUserData(ID_UD_NODE));
      if (elem != NULL)
      {
        if (elem->kind == ALL)
          if (elem->node->is_Implicit())
            bscolor = "FF0000";
      }
      f << "          <y:BorderStyle color=\"" + bscolor + "\" type=\"line\" width=\"1.0\"/>" << std::endl;
      f << "          <y:NodeLabel alignment=\"center\" autoSizePolicy=\"content\" fontFamily=\"Dialog\" fontSize=\"12\" fontStyle=\"plain\" hasBackgroundColor=\"false\" hasLineColor=\"false\" height=\"18.701171875\" modelName=\"internal\" modelPosition=\"c\" textColor=\"#000000\" visible=\"true\" width=\"30.015625\" x=\"-0.0078125\" y=\"5.6494140625\">";
      std::string name = (*ii).second.get()->getName();
      f << name << "</y:NodeLabel>" << std::endl;
      symbolidmap[name] = id;
      f << "          <y:Shape type=\"rectangle\"/>" << std::endl;
      f << "        </y:ShapeNode>" << std::endl;
      f << "      </data>" << std::endl;
      f << "    </node>" << std::endl;
      id++;
    }
  }
  // write edges
  id = 0;
  for (NodeVec::iterator ii =m_nodes.begin(); ii != m_nodes.end();++ii)
  {
    SymbolPtrElemMap &symbols = (*ii)->getSolveFor();
    for (SymbolPtrElemMap::iterator i=symbols.begin();i!=symbols.end();i++)
    {
      size_t target = symbolidmap[i->first->getName()];
      SymbolPtrElemSizeTMap parents = (*ii)->getParentsSymbols();
      for (SymbolPtrElemSizeTMap::iterator jj=parents.begin();jj!=parents.end();++jj)
      {
        size_t source = symbolidmap[i->first->getName()];
        f << "    <edge id=\"e" << id <<"\" source=\"n" << source << "\" target=\"n" << target << "\">" << std::endl;
        f << "      <data key=\"d8\"/>" << std::endl;
        f << "      <data key=\"d9\"><![CDATA[UMLuses]]></data>" << std::endl;
        f << "      <data key=\"d10\">" << std::endl;
        f << "        <y:PolyLineEdge>" << std::endl;
        f << "          <y:Path sx=\"0.0\" sy=\"0.0\" tx=\"0.0\" ty=\"0.0\"/>" << std::endl;
        f << "          <y:LineStyle color=\"#000000\" type=\"dashed\" width=\"1.0\"/>" << std::endl;
        f << "          <y:Arrows source=\"none\" target=\"short\"/>" << std::endl;
        f << "          <y:EdgeLabel alignment=\"center\" distance=\"2.0\" fontFamily=\"Dialog\" fontSize=\"12\" fontStyle=\"plain\" hasBackgroundColor=\"false\" hasLineColor=\"false\" hasText=\"false\" height=\"4.0\" modelName=\"six_pos\" modelPosition=\"tail\" preferredPlacement=\"anywhere\" ratio=\"0.5\" textColor=\"#000000\" visible=\"true\" width=\"4.0\"/>" << std::endl;
        f << "          <y:BendStyle smoothed=\"false\"/>" << std::endl;
        f << "        </y:PolyLineEdge>" << std::endl;
        f << "      </data>" << std::endl;
        f << "    </edge>" << std::endl;
        id++;
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
void MatchedSystem::sortEquations()
/*****************************************************************************/
{
  NodeSorter v;

  for (NodeVec::iterator ii =m_nodes.begin(); ii != m_nodes.end();++ii)
  {
    Node::node_iterate_depth_first((*ii),v);
  }
  // reset visited
  for (NodeVec::iterator ii =m_nodes.begin(); ii != m_nodes.end();++ii)
  {
    (*ii)->visited = false;
  }
  m_nodes = v.getNodes();
}
/*****************************************************************************/
