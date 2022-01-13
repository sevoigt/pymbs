#include "CSharpWriter.h"
#include "CSharpPrinter.h"
#include "str.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

using namespace Symbolics;


/*****************************************************************************/
CSharpWriter::CSharpWriter( std::map<std::string, std::string> &kwds ): 
	Writer(true), m_include_visual(true)
/*****************************************************************************/
{
	m_p = new CSharpPrinter();

	if (kwds.find("include_visual") != kwds.end())
		m_include_visual = (kwds["include_visual"] == "True");

}
/*****************************************************************************/


/*****************************************************************************/
CSharpWriter::CSharpWriter(): 
	Writer(true), m_include_visual(true)
/*****************************************************************************/
{
	m_p = new CSharpPrinter();
}
/*****************************************************************************/


/*****************************************************************************/
CSharpWriter::~CSharpWriter()
/*****************************************************************************/
{
	if (m_p != NULL)
	{
		delete m_p;
		m_p = NULL;
	}
}
/*****************************************************************************/


/*****************************************************************************/
double CSharpWriter::generateTarget_Impl(Graph::Graph &g)
/*****************************************************************************/
{
	int dim = 0;

	double t1 = generateDerState(g, dim);
	double t2 = 0;
	if (m_include_visual)
		t2 = generateVisual(g);
	double t3 = generateSensors(g);

	return t1+t2+t3;
}
/*****************************************************************************/


/*****************************************************************************/
double CSharpWriter::generateDerState(Graph::Graph& g, int &dim)
/*****************************************************************************/
{
	//get timestamp and catch variables from graph
	double t1 = Util::getTime();

	Graph::AssignmentsPtr a = g.getAssignments(DER_STATE);

	Graph::VariableVec states = a->getVariables(STATE);
	if (states.empty())
		throw InternalError("Error: Models without states are not yet implemented in C#!");
	Graph::VariableVec variables = a->getVariables(VARIABLE|SENSOR|SENSOR_VISUAL);
	Graph::VariableVec inputs = a->getVariables(INPUT);
	Graph::VariableVec parameter = a->getVariables(PARAMETER);
	Graph::VariableVec constants = a->getVariables(CONSTANT);
	Graph::VariableVec controller = a->getVariables(CONTROLLER);
	Graph::VariableVec userexp = a->getVariables(USER_EXP);

	// Sort variable vectors
	std::sort(states.begin(),states.end(), sortVariableVec);
	std::sort(inputs.begin(),inputs.end(), sortVariableVec);

	std::ofstream f;
    std::string filename= m_path + "/" + m_name + "_der_state.cs";
    f.open(filename.c_str());

	f << "/* " << getHeaderLine() << " */" << std::endl;
	f << "using System;" << std::endl;
	f << "using MathNet.Numerics.LinearAlgebra;" << std::endl;
	f << std::endl;

	f << "int "<< m_name <<"_der_state(double time, double[] y), out double[] yd"; 
	for (Graph::VariableVec::iterator it=inputs.begin();it!=inputs.end();++it)
		f << ", double " << m_p->print(*it) << m_p->dimension(*it); 
	for (Graph::VariableVec::iterator it=controller.begin();it!=controller.end();++it)
		f << ", double " << m_p->print(*it) << m_p->dimension(*it); 
	f << ")" << std::endl;  //evtl noch die statesize mituebergeben, aber die kenn wir eigentlich auch
	f << "{" << std::endl;

	f << "    /* declare state variables */" << std::endl;
	for (size_t i=0; i < states.size(); ++i)
	{
		size_t n = states.at(i)->getShape().getNumEl();
		dim = n;
		f << "    double "  << m_p->print(states.at(i)) << m_p->dimension(states.at(i)) << " = " << ( n>1 ? "{" : "");
		for (size_t j=0; j < n ; ++j)
			f << "y[" << i*n+j << "]" << (j+1 < n ? ", " : "");
		f << ( n>1 ? "}" : "") << ";" << m_p->comment2(g, states.at(i)) << std::endl;
		f << "    double " << "der_" << m_p->print(states.at(i)) << m_p->dimension(states.at(i)) << " = " 
          << m_p->print(Zero::getZero(states.at(i)->getShape())) << ";" << std::endl;	
	}
	f << std::endl; 

	f << "    /* Parameters */" << std::endl;
	for (Graph::VariableVec::iterator it=parameter.begin();it!=parameter.end();++it)
		f << "    double " << m_p->print(*it) << m_p->dimension(*it) << " = " << m_p->print(g.getEquation(*it)) << "; " << m_p->comment2(g,*it) <<  std::endl;
	f << std::endl;

	f << "    /* Constants */" << std::endl;
	for (Graph::VariableVec::iterator it=constants.begin();it!=constants.end();++it)
        f << "    double " << m_p->print(*it) << m_p->dimension(*it) << " = " << m_p->print(g.getEquation(*it)) << "; " << m_p->comment2(g,*it) <<  std::endl;
	f << std::endl;

	f << "    /* User Expression variables */" << std::endl;
	for (Graph::VariableVec::iterator it=userexp.begin();it!=userexp.end();++it)
		f << "    double " << m_p->print(*it) << m_p->dimension(*it) << " = " << m_p->print(g.getinitVal(*it)) << "; " << m_p->comment2(g,*it) <<  std::endl;
	f << std::endl;

	f << "    /* ordinary variables */" << std::endl;
    for (Graph::VariableVec::iterator it=variables.begin();it!=variables.end();++it)
        f << "    double " << m_p->print(*it) << m_p->dimension(*it) << " = " << m_p->print(g.getinitVal(*it)) << "; " << m_p->comment2(g,*it) <<  std::endl;
	f << std::endl;
	
	f << "    /* calculate state derivative */" << std::endl;
	f << writeEquations(a->getEquations(PARAMETER | CONSTANT | INPUT )) << std::endl;
    f << std::endl;

	f << "    /* set return values */" << std::endl;
	for (size_t i=0;i < states.size(); ++i)
	{
		size_t n = states.at(i)->getShape().getNumEl();
		for (size_t j=0; j < n ; ++j) 
		{
			f << "    yd[" << i*n+j << "] = der_" << m_p->print(states.at(i));
			if (n>1) // hm.. was ist schneller "n-1" oder "n>1"?
				f << "[" << j << "];" << std::endl;
			else 
				f << ";" << std::endl; 
		}
	}
	f << std::endl; 

	f << "    return 0;" << std::endl;
	f << "}" << std::endl;

	f.close();

    if (m_p->getErrorcount())
        std::cerr << "There have been " << m_p->getErrorcount(true) << " error(s) during generation of " 
                  << m_name << "_der_state.c. Please have a look at that file to see where the error(s) occured." << std::endl;

	return Util::getTime() - t1;
}
/*****************************************************************************/


/*****************************************************************************/
double CSharpWriter::generateVisual(Graph::Graph& g)
/*****************************************************************************/
{
	//get timestamp and catch variables from graph
	double t1 = Util::getTime();

	Graph::AssignmentsPtr a = g.getAssignments(SENSOR_VISUAL);

	Graph::VariableVec states = a->getVariables(STATE);
	Graph::VariableVec variables = a->getVariables(VARIABLE|SENSOR);
	Graph::VariableVec parameter = a->getVariables(PARAMETER);
	Graph::VariableVec constants = a->getVariables(CONSTANT);
	Graph::VariableVec userexp = a->getVariables(USER_EXP);
	Graph::VariableVec sens_vis = a->getVariables(SENSOR_VISUAL);
    if (sens_vis.empty())
	{
		std::cerr << "Warning: CSharpWriter: Not generating visual file, since there are no visual sensors. (Maybe there are no visualisations defined?)" << std::endl;
		return Util::getTime() - t1;
	}

	// Sort variable vectors
	std::sort(states.begin(),states.end(), sortVariableVec);
	std::sort(sens_vis.begin(),sens_vis.end(), sortVariableVec);

	std::ofstream f;
    std::string filename= m_path + "/" + m_name + "_visual.cs";
    f.open(filename.c_str());

	f << "/* " << getHeaderLine() << " */" << std::endl;
		
	f << "using System;" << std::endl;
	f << "using MathNet.Numerics.LinearAlgebra;" << std::endl;
	f << std::endl;

	f << "int "<< m_name <<"_visual(Vector<double> y"; 
	for (Graph::VariableVec::iterator it=sens_vis.begin(); it!=sens_vis.end(); ++it)
		
		if (((BasicPtr)*it)->is_Scalar())
		{			
			f << ", double " << m_p->print(*it);	// should not happen for visual sensor
		}
		else if (((BasicPtr)*it)->is_Vector()) 	//if (m_p->dimension(*it) == "[3]") -- will break when dimension() changes
		{ 
			f << ", out Vector<double> " << m_p->print(*it);
		}
		else
		{
			f << ", out Matrix<double> " << m_p->print(*it);
		}

	f << ")" << std::endl;
	f << "{" << std::endl;

	f << "    /* declare state variables */" << std::endl;
	for (size_t i=0; i < states.size(); ++i)
	{
		size_t n = states.at(i)->getShape().getNumEl();
		f << "    double "  << m_p->print(states.at(i)) << m_p->dimension(states.at(i)) << " = " << ( n>1 ? "{" : "");
		for (size_t j=0; j < n ; ++j)
			f << "y[" << i*n+j << "]" << (j+1 < n ? ", " : "");
		f << ( n>1 ? "}" : "") << ";" << m_p->comment2(g, states.at(i)) << std::endl;	
	}
	f << std::endl; 

	f << "    /* Parameters */" << std::endl;
	for (Graph::VariableVec::iterator it=parameter.begin();it!=parameter.end();++it)
		f << "    double " << m_p->print(*it) << m_p->dimension(*it) << " = " << m_p->print(g.getEquation(*it)) << "; " << m_p->comment2(g,*it) <<  std::endl;
	f << std::endl;

	f << "    /* Constants */" << std::endl;
	for (Graph::VariableVec::iterator it=constants.begin();it!=constants.end();++it)
        f << "    double " << m_p->print(*it) << m_p->dimension(*it) << " = " << m_p->print(g.getEquation(*it)) << "; " << m_p->comment2(g,*it) <<  std::endl;
	f << std::endl;

	f << "    /* User Expression variables */" << std::endl;
	for (Graph::VariableVec::iterator it=userexp.begin();it!=userexp.end();++it)
		f << "    double " << m_p->print(*it) << m_p->dimension(*it) << " = " << m_p->print(g.getinitVal(*it)) << "; " << m_p->comment2(g,*it) <<  std::endl;
	f << std::endl;

	f << "    /* ordinary variables */" << std::endl;
    for (Graph::VariableVec::iterator it=variables.begin();it!=variables.end();++it)
        f << "    double " << m_p->print(*it) << m_p->dimension(*it) << " = " << m_p->print(g.getinitVal(*it)) << "; " << m_p->comment2(g,*it) <<  std::endl;
	f << std::endl;

	f << "    /* calculate visual sensors */" << std::endl;
	f << writeEquations(a->getEquations(PARAMETER | CONSTANT | INPUT )) << std::endl;
    f << std::endl;

	f << "	return 0;" << std::endl;
	f << "}" << std::endl;
	
	f.close();

    if (m_p->getErrorcount())
        std::cerr << "There have been " << m_p->getErrorcount(true) << " error(s) during generation of " 
                  << m_name << "_visual.cs. Please have a look at that file to see where the error(s) occured." << std::endl;

	return Util::getTime() - t1;
}
/*****************************************************************************/


/*****************************************************************************/
double CSharpWriter::generateSensors(Graph::Graph& g)
/*****************************************************************************/
{
	//get timestamp and catch variables from graph
	double t1 = Util::getTime();

	Graph::AssignmentsPtr a = g.getAssignments(SENSOR);

	Graph::VariableVec states = a->getVariables(STATE);
	Graph::VariableVec variables = a->getVariables(VARIABLE|SENSOR_VISUAL);
	Graph::VariableVec inputs = a->getVariables(INPUT);
	Graph::VariableVec parameter = a->getVariables(PARAMETER);
	Graph::VariableVec constants = a->getVariables(CONSTANT);
	Graph::VariableVec userexp = a->getVariables(USER_EXP);
	Graph::VariableVec sensors = a->getVariables(SENSOR);
	if (sensors.empty())
	{
		std::cerr << "Warning: CSharpWriter: Not generating sensors file, since there are no sensors." << std::endl;
		return Util::getTime() - t1;
	}

	// StateVariables Vector sortieren:
	std::sort(states.begin(),states.end(), sortVariableVec);
	// Sensors Vector sortieren: 
	std::sort(sensors.begin(),sensors.end(), sortVariableVec);
    // Inputs Vector sortieren: 
	std::sort(inputs.begin(),inputs.end(), sortVariableVec);

	std::ofstream f;
    std::string filename= m_path + "/" + m_name + "_sensors.cs";
    f.open(filename.c_str());

	f << "/* " << getHeaderLine() << " */" << std::endl;
	
	f << "using System" << std::endl;
	f << "using MathNet.Numerics.LinearAlgebra" << std::endl;
	f << std::endl;

	f << "int "<< m_name <<"_sensors(double time, double y"; 
	for (Graph::VariableVec::iterator it=inputs.begin();it!=inputs.end();++it)
		f << ", double " << m_p->print(*it) << m_p->dimension(*it); 
	for (Graph::VariableVec::iterator it=sensors.begin();it!=sensors.end();++it)
		if (it==sensors.begin())
			f << ", " << std::endl <<"                double " << m_p->print(*it) << m_p->dimension(*it);
		else
			f << ", double " << m_p->print(*it) << m_p->dimension(*it); 
	f << ")" << std::endl;  
	f << "{" << std::endl;

	f << "/* declare state variables */" << std::endl;
	for (size_t i=0; i < states.size(); ++i)
	{
		size_t n = states.at(i)->getShape().getNumEl();
		f << "    double "  << m_p->print(states.at(i)) << m_p->dimension(states.at(i)) << " = " << ( n>1 ? "{" : "");
		for (size_t j=0; j < n ; ++j)
			f << "y[" << i*n+j << "]" << (j+1 < n ? ", " : "");
		f << ( n>1 ? "}" : "") << ";" << m_p->comment2(g, states.at(i)) << std::endl;	
	}
	f << std::endl; 

	f << "/* Parameters */" << std::endl;
	for (Graph::VariableVec::iterator it=parameter.begin();it!=parameter.end();++it)
		f << "    double " << m_p->print(*it) << m_p->dimension(*it) << "=" << m_p->print(g.getEquation(*it)) << "; " << m_p->comment2(g,*it) <<  std::endl;
	f << std::endl;

	f << "/* Constants */" << std::endl;
	for (Graph::VariableVec::iterator it=constants.begin();it!=constants.end();++it)
        f << "    double " << m_p->print(*it) << m_p->dimension(*it) << "=" << m_p->print(g.getEquation(*it)) << "; " << m_p->comment2(g,*it) <<  std::endl;
	f << std::endl;

	f << "/* User Expression variables */" << std::endl;
	for (Graph::VariableVec::iterator it=userexp.begin();it!=userexp.end();++it)
		f << "    double " << m_p->print(*it) << m_p->dimension(*it) << "= " << m_p->print(g.getinitVal(*it)) << "; " << m_p->comment2(g,*it) <<  std::endl;
	f << std::endl;

	f << "/* ordinary variables */" << std::endl;
    for (Graph::VariableVec::iterator it=variables.begin();it!=variables.end();++it)
        f << "    double " << m_p->print(*it) << m_p->dimension(*it) << "= " << m_p->print(g.getinitVal(*it)) << "; " << m_p->comment2(g,*it) <<  std::endl;
	f << std::endl;

	f << "/* calculate sensors */" << std::endl;
	f << writeEquations(a->getEquations(PARAMETER | CONSTANT | INPUT )) << std::endl;
    f << std::endl;

	f << "	return 0;" << std::endl;
	f << "}" << std::endl;
	
	f.close();

    if (m_p->getErrorcount())
        std::cerr << "There have been " << m_p->getErrorcount(true) << " error(s) during generation of " 
                  << m_name << "_sensors.cs. Please have a look at that file to see where the error(s) occured." << std::endl;

	return Util::getTime() - t1;
}
/*****************************************************************************/


/*****************************************************************************/
std::string CSharpWriter::writeEquations(std::vector<Graph::Assignment> const& equations) const
/*****************************************************************************/
{
	std::stringstream s;

	for (std::vector<Graph::Assignment>::const_iterator it=equations.begin(); it!=equations.end(); ++it)
    {
        if (it->implizit)
            throw InternalError("Implicit equations are not yet implemented in C#!");
		//Folgendes falls mehrere Gleichungen in einer verpackt sind (wird aber scheinbar kaum genutzt)
		for (size_t i=0; i < it->lhs.size(); ++i)
        {
            BasicPtr simple_exp = it->rhs[i]->simplify();
            if (simple_exp.get() == NULL) throw InternalError("CSharpWriter: Value of Rhs is not Valid!");

			//Workaround: Solve muss speziell behandelt werden TODO: Irgendwie ist das nicht sch�n und deckt bestimmt nicht alle F�lle ab
			if (it->rhs[i]->getType() == Type_Solve) 
			{
				const Solve *solve = Util::getAsConstPtr<Solve>(it->rhs[i]);
				//Util::getAsConstPtr<Element>(it->lhs[i]->getArg(0)->getArg(0));
				int dim = solve->getArg2()->getShape().getDimension(1);
				s << "    {" << std::endl;
				s << "        double solve_A[" << dim << "][" << dim << "] = " << m_p->print(solve->getArg1()) << ";" << std::endl;
				s << "        double solve_b[" << dim << "] = " << m_p->print(solve->getArg2()) << ";" << std::endl;
				s << "        legs(solve_A, solve_b, " << m_p->print(it->lhs[i]->getArg(0)->getArg(0)) << ");" << std::endl;
				s << "    }" << std::endl;
			} else {
				// ordinary equation
				s << "    " << m_p->print(it->lhs[i]) << " = " << m_p->print(simple_exp) << ";" << std::endl;
			}
        }
    }

	return s.str();
}
/*****************************************************************************/
