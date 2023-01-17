#include "ModelicaWriter.h"
#include "ModelicaPrinter.h"
#include "str.h"
#include <iostream>
#include <fstream>

using namespace Symbolics;

/*****************************************************************************/
ModelicaWriter::ModelicaWriter( std::map<std::string, std::string> &kwds ):
Writer(false), m_InputsAsInputs(true)
/*****************************************************************************/
{
  // Nach Einstellungen suchen
  if (kwds.find("inputsAsInputs") != kwds.end())
    m_InputsAsInputs = (kwds["inputsAsInputs"] == "True");

  if (kwds.find("package") != kwds.end())
    package = kwds["package"];
}
/*****************************************************************************/


/*****************************************************************************/
ModelicaWriter::ModelicaWriter():
Writer(false), m_InputsAsInputs(true)
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
ModelicaWriter::~ModelicaWriter()
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
double ModelicaWriter::generateTarget_Impl(Graph::Graph &g)
/*****************************************************************************/
{
  ModelicaPrinter p;
  // open file
  std::ofstream f;
  std::string filename= m_path + "/" + m_name + ".mo";
  f.open(filename.c_str());

  //f.close();
  double t1 = Util::getTime();
  Graph::AssignmentsPtr a = g.getAssignments(DER_STATE | SENSOR | SENSOR_VISUAL);

  Graph::VariableVec states = a->getVariables(STATE);
  Graph::VariableVec variables = a->getVariables(VARIABLE);
  Graph::VariableVec inputs = a->getVariables(INPUT);
  Graph::VariableVec parameter = a->getVariables(PARAMETER);
  Graph::VariableVec constants = a->getVariables(CONSTANT);
  Graph::VariableVec userexp = a->getVariables(USER_EXP);
  Graph::VariableVec sens = a->getVariables(Writer::SENSOR);
  Graph::VariableVec sensviss = a->getVariables(Writer::SENSOR_VISUAL);


  f << "//" + getHeaderLine() << std::endl;

  if (package.length() > 0)
    f << "within " << package << ";" << std::endl;

  if ((!m_InputsAsInputs) && (inputs.size() > 0))
    f << "partial model " << m_name << std::endl;
  else
    f << "model " << m_name << std::endl;

  f << "  //Imports from Modelica Standard Library" << std::endl;
  f << "  import Modelica.Math.asin;" << std::endl;
  f << "  import Modelica.Math.acos;" << std::endl;
  f << "  import Modelica.Math.atan;" << std::endl;
  f << "  import Modelica.Math.atan2;" << std::endl;
  f << "  import Modelica.Constants.pi;" << std::endl;

  f << "//State" << std::endl;
  for (Graph::VariableVec::iterator ii=states.begin();ii!=states.end();++ii)
  {
    f << "   Real" << p.dimension(*ii) << " " << (*ii)->getName() << "(start=" << p.print(g.getinitVal(*ii)) << ", each stateSelect=StateSelect.prefer)" << p.comment2(g,*ii) << ";" << std::endl;
  }

  f << "//Inputs" << std::endl;
  for (Graph::VariableVec::iterator ii=inputs.begin();ii!=inputs.end();++ii)
  {
    if (m_InputsAsInputs)
      f << "   input Real" << p.dimension(*ii) << " " << (*ii)->getName() << p.comment2(g,*ii) <<  ";" << std::endl;
    else
      f << "   Real" << p.dimension(*ii) << " " << (*ii)->getName() << p.comment2(g,*ii) <<  ";" << std::endl;
  }
  f << "//Parameters" << std::endl;
  for (Graph::VariableVec::iterator ii=parameter.begin();ii!=parameter.end();++ii)
  {
    f << "   parameter Real" << p.dimension(*ii) << " " << (*ii)->getName() << " = " << p.print(g.getEquation(*ii)) << p.comment2(g,*ii) <<  ";" << std::endl;
  }

  f << "//Sensors" << std::endl;
  for (Graph::VariableVec::iterator ii=sens.begin();ii!=sens.end();++ii)
  {
    f << "   output Real" << p.dimension(*ii) << " " << (*ii)->getName() << p.comment2(g,*ii) <<  ";" << std::endl;
  }
  f << "//Sensors - Visualisation" << std::endl;
  for (Graph::VariableVec::iterator ii=sensviss.begin();ii!=sensviss.end();++ii)
  {
    f << "   output Real" << p.dimension(*ii) << " " << (*ii)->getName() << p.comment2(g,*ii) <<  ";" << std::endl;
  }
  f << "//User Exp" << std::endl;
  for (Graph::VariableVec::iterator ii=userexp.begin();ii!=userexp.end();++ii)
  {
    f << "   Real" << p.dimension(*ii) << " " << (*ii)->getName();
    if (!Util::is_Zero(g.getinitVal(*ii)))
      f << "(start=" << p.print(g.getinitVal(*ii)) << ")";
    f << p.comment2(g,*ii) <<  ";" << std::endl;
  }

  f << std::endl;
  f << "protected" << std::endl;

  f << "//Constants" << std::endl;
  for (Graph::VariableVec::iterator ii=constants.begin();ii!=constants.end();++ii)
  {
    f << "   constant Real" <<  p.dimension(*ii) << " " <<  p.dimension(*ii) << (*ii)->getName() << " = " << p.print(g.getEquation(*ii)) << p.comment2(g,*ii) <<  ";" << std::endl;
  }
  f << "//Variables" << std::endl;
  for (Graph::VariableVec::iterator ii=variables.begin();ii!=variables.end();++ii)
  {
    f << "   Real" <<  p.dimension(*ii) << " " << (*ii)->getName();
    if (!Util::is_Zero(g.getinitVal(*ii)))
      f << "(start=" << p.print(g.getinitVal(*ii)) << ")";
    f  << p.comment2(g,*ii) <<  ";" << std::endl;
  }
  f << "equation" << std::endl;

  std::vector<Graph::Assignment> equations =  a->getEquations(PARAMETER | CONSTANT | INPUT );
  for (std::vector<Graph::Assignment>::const_iterator ii=equations.begin(); ii!=equations.end(); ++ii)
  {
    for (size_t i=0; i < ii->lhs.size(); ++i)
    {
      BasicPtr simple_exp = ii->rhs[i]->simplify();
      if (simple_exp.get() == NULL)
        throw InternalError("ModelicaWriter: Value of Rhs is not Valid!");
      f << "  " << p.print(ii->lhs[i]) << " = " << p.print(simple_exp) << ";" << std::endl;
    }
  }
  f << "end " << m_name << ";" << std::endl;

  // close file
  f.close();

  if (p.getErrorcount())
    std::cerr << "There have been " << p.getErrorcount(true) << " error(s) during generation of " 
              << m_name << ".mo. Please have a look at that file to see where the error(s) occured." << std::endl;

  return Util::getTime() - t1;
}
/*****************************************************************************/

