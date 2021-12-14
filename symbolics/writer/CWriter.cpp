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

#include "CWriter.h"
#include "CPrinter.h"
#include "PythonPrinter.h"
#include "PythonWriter.h"
#include "str.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

using namespace Symbolics;


/*****************************************************************************/
CWriter::CWriter( std::map<std::string, std::string> &kwds ): 
	Writer(true), m_pymbs_wrapper(false), m_simulink_sfunction(false), m_include_visual(true)
/*****************************************************************************/
{
	m_p = new CPrinter();

	m_pymbs_wrapper=false;
	m_simulink_sfunction = false;
	if (kwds.find("pymbs_wrapper") != kwds.end())
		m_pymbs_wrapper = (kwds["pymbs_wrapper"] == "True");
	if (kwds.find("sfunction") != kwds.end())
		m_simulink_sfunction = (kwds["sfunction"] == "True");
	if (kwds.find("include_visual") != kwds.end())
		m_include_visual = (kwds["include_visual"] == "True");

}
/*****************************************************************************/


/*****************************************************************************/
CWriter::CWriter(): 
	Writer(true), m_pymbs_wrapper(false), m_simulink_sfunction(false), m_include_visual(true)
/*****************************************************************************/
{
	m_p = new CPrinter();
}
/*****************************************************************************/


/*****************************************************************************/
CWriter::~CWriter()
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
double CWriter::generateTarget_Impl(Graph::Graph &g)
/*****************************************************************************/
{
	int dim = 0;

	if (m_simulink_sfunction)
	{
		double t1 = generateAll(g, dim);
		double t2 = generateFunctionmodule(dim);
		return t1+t2;
	}
	else
	{
		double t1 = generateDerState(g, dim);
		double t2 = 0;
		if (m_include_visual)
			t2 = generateVisual(g);
		double t3 = generateSensors(g);
		double t4 = generateFunctionmodule(dim);
		double t5 = 0.0;
		if (m_pymbs_wrapper)
			t5 = generatePymbsWrapper(g);

		return t1+t2+t3+t4+t5;
	}
}
/*****************************************************************************/

/*****************************************************************************/
double CWriter::generateDerState(Graph::Graph& g, int &dim)
/*****************************************************************************/
{
	//get timestamp and catch variables from graph
	double t1 = Util::getTime();

	Graph::AssignmentsPtr a = g.getAssignments(DER_STATE);

	Graph::VariableVec states = a->getVariables(STATE);
	if (states.empty())
		throw InternalError("Error: Models without states are not yet implemented in C!");
	Graph::VariableVec variables = a->getVariables(VARIABLE|SENSOR|SENSOR_VISUAL);
	Graph::VariableVec inputs = a->getVariables(INPUT);
	Graph::VariableVec parameter = a->getVariables(PARAMETER);
	Graph::VariableVec constants = a->getVariables(CONSTANT);
	Graph::VariableVec controller = a->getVariables(CONTROLLER);
	Graph::VariableVec userexp = a->getVariables(USER_EXP);

	// StateVariables Vector sortieren:
	std::sort(states.begin(),states.end(), sortVariableVec);
    // Inputs Vector sortieren: 
	std::sort(inputs.begin(),inputs.end(), sortVariableVec);

	std::ofstream f;
    std::string filename= m_path + "/" + m_name + "_der_state.c";
    f.open(filename.c_str());

	f << "/* " << getHeaderLine() << " */" << std::endl;
	f << "#include <math.h>" << std::endl;
	f << "#include \"functionmodule.c\"" << std::endl;
	f << std::endl;

	f << "int "<< m_name <<"_der_state(double time, double * y, double * yd"; 
	for (Graph::VariableVec::iterator it=inputs.begin();it!=inputs.end();++it)
		f << ", double " << m_p->print(*it) << m_p->dimension(*it); 
	for (Graph::VariableVec::iterator it=controller.begin();it!=controller.end();++it)
		f << ", double " << m_p->print(*it) << m_p->dimension(*it); 
	f << ")" << std::endl;  //evtl noch die statesize mitübergeben, aber die kenn wir eigentlich auch
	f << "{" << std::endl;

	f << "/* declare state variables */" << std::endl;
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
	
	f << "/* calculate state derivative */" << std::endl;
	f << writeEquations(a->getEquations(PARAMETER | CONSTANT | INPUT )) << std::endl;
    f << std::endl;

	f << "/* set return values */" << std::endl;
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

	////Jetzt wieder Datei schreiben, zunächst noch zwei Zeilen an den Anfang setzen
	//f << "    int tobefreedIndex = 0;" << std::endl;
	//f << "    double *tobefreed[100];" << std::endl;

	//f << ss.rdbuf();

	f << "	return 0;" << std::endl;
	f << "}" << std::endl;

	f.close();

    if (m_p->getErrorcount())
        std::cerr << "There have been " << m_p->getErrorcount(true) << " error(s) during generation of " 
                  << m_name << "_der_state.c. Please have a look at that file to see where the error(s) occured." << std::endl;

	return Util::getTime() - t1;
}
/*****************************************************************************/

/*****************************************************************************/
double CWriter::generateVisual(Graph::Graph& g)
/*****************************************************************************/
{
	//get timestamp and catch variables from graph
	double t1 = Util::getTime();

	Graph::AssignmentsPtr a = g.getAssignments(SENSOR_VISUAL);

	Graph::VariableVec states = a->getVariables(STATE);
	Graph::VariableVec variables = a->getVariables(VARIABLE|SENSOR);
	//Graph::VariableVec inputs = a->getVariables(INPUT);
	Graph::VariableVec parameter = a->getVariables(PARAMETER);
	Graph::VariableVec constants = a->getVariables(CONSTANT);
	Graph::VariableVec userexp = a->getVariables(USER_EXP);
	Graph::VariableVec sens_vis = a->getVariables(SENSOR_VISUAL);
    if (sens_vis.empty())
	{
		std::cerr << "Warning: CWriter: Not generating visual file, since there are no visual sensors. (Maybe there are no visualisations defined?)" << std::endl;
		return Util::getTime() - t1;
	}

	// StateVariables Vector sortieren:
	std::sort(states.begin(),states.end(), sortVariableVec);
	// Visual_sensors Vector sortieren: 
	std::sort(sens_vis.begin(),sens_vis.end(), sortVariableVec);

	std::ofstream f;
    std::string filename= m_path + "/" + m_name + "_visual.c";
    f.open(filename.c_str());

	f << "/* " << getHeaderLine() << " */" << std::endl;
	
	f << "#include <math.h>" << std::endl;
	f << "#include \"functionmodule.c\"" << std::endl;
	f << std::endl;

	f << "int "<< m_name <<"_visual(double * y"; 
	for (Graph::VariableVec::iterator it=sens_vis.begin();it!=sens_vis.end();++it)
		f << ", double " << m_p->print(*it) << m_p->dimension(*it); 
	f << ")" << std::endl;  //evtl noch die statesize mitübergeben, aber die kenn wir eigentlich auch
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

	f << "/* calculate visual sensors */" << std::endl;
	f << writeEquations(a->getEquations(PARAMETER | CONSTANT | INPUT )) << std::endl;
    f << std::endl;

	f << "	return 0;" << std::endl;
	f << "}" << std::endl;
	
	f.close();

    if (m_p->getErrorcount())
        std::cerr << "There have been " << m_p->getErrorcount(true) << " error(s) during generation of " 
                  << m_name << "_visual.c. Please have a look at that file to see where the error(s) occured." << std::endl;

	return Util::getTime() - t1;
}
/*****************************************************************************/


/*****************************************************************************/
double CWriter::generateSensors(Graph::Graph& g)
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
		std::cerr << "Warning: CWriter: Not generating sensors file, since there are no sensors." << std::endl;
		return Util::getTime() - t1;
	}

	// StateVariables Vector sortieren:
	std::sort(states.begin(),states.end(), sortVariableVec);
	// Sensors Vector sortieren: 
	std::sort(sensors.begin(),sensors.end(), sortVariableVec);
    // Inputs Vector sortieren: 
	std::sort(inputs.begin(),inputs.end(), sortVariableVec);

	std::ofstream f;
    std::string filename= m_path + "/" + m_name + "_sensors.c";
    f.open(filename.c_str());

	f << "/* " << getHeaderLine() << " */" << std::endl;
	
	f << "#include <math.h>" << std::endl;
	f << "#include \"functionmodule.c\"" << std::endl;
	f << std::endl;

	f << "int "<< m_name <<"_sensors(double time, double * y"; 
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
                  << m_name << "_sensors.c. Please have a look at that file to see where the error(s) occured." << std::endl;

	return Util::getTime() - t1;
}
/*****************************************************************************/

/*****************************************************************************/
std::string CWriter::writeEquations(std::vector<Graph::Assignment> const& equations) const
/*****************************************************************************/
{
	std::stringstream s;

	for (std::vector<Graph::Assignment>::const_iterator it=equations.begin(); it!=equations.end(); ++it)
    {
        if (it->implizit)
            throw InternalError("Implicit equations are not yet implemented in C!");
		//Folgendes falls mehrere Gleichungen in einer verpackt sind (wird aber scheinbar kaum genutzt)
		for (size_t i=0; i < it->lhs.size(); ++i)
        {
            BasicPtr simple_exp = it->rhs[i]->simplify();
            if (simple_exp.get() == NULL) throw InternalError("CWriter: Value of Rhs is not Valid!");

			//Workaround: Solve muss speziell behandelt werden TODO: Irgendwie ist das nicht schön und deckt bestimmt nicht alle Fälle ab
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

/*****************************************************************************/
double CWriter::generatePymbsWrapper(Graph::Graph& g)
/*****************************************************************************/
{
	//get timestamp and catch variables from graph
	double t1 = Util::getTime();

	PythonPrinter p; //zur Abkürzung

	Graph::VariableVec state_inputs = g.getAssignments(DER_STATE)->getVariables(INPUT);
    Graph::VariableVec controller = g.getAssignments(DER_STATE)->getVariables(CONTROLLER);
    Graph::VariableVec sensors = g.getVariables(SENSOR);

	// Sensors Vector sortieren: 
	std::sort(sensors.begin(),sensors.end(), sortVariableVec);
    // Inputs Vector sortieren: 
	std::sort(state_inputs.begin(),state_inputs.end(), sortVariableVec);

	std::ofstream f;
    std::string filename= m_path + "/" + m_name + "_der_state_CWrapper.py";
    f.open(filename.c_str());

	f << "# " << getHeaderLine() << std::endl;
	f << std::endl;
	f << "from ctypes import c_double" << std::endl;
	f << "from numpy import matrix, zeros_like, zeros, ctypeslib" << std::endl;
    if (!sensors.empty())
	{
        f << "from " << m_name << "_sensors_CWrapper import graphSensors" << std::endl;
	}
	for (Graph::VariableVec::iterator it=controller.begin();it!=controller.end();++it)
	{
        std::string comment_tmp = m_p->comment2(g,*it);
        int comment_length = comment_tmp.length();
        std::string comment = comment_tmp.substr(3, comment_length-5);
        std::vector<std::string> comment_vector = split(comment, ':');
        f << "from " << comment_vector.front() << " import " << comment_vector.back() << std::endl;
	}
	f << std::endl;
	f << "cm = ctypeslib.load_library(\"" << m_name << "_der_state.so\", \"" << m_path << "\")" << std::endl;
	f << "cm." << m_name << "_der_state.argtypes = [c_double," << std::endl;
    f << "                    ctypeslib.ndpointer()," << std::endl;
	f << "                    ctypeslib.ndpointer()";
	for (Graph::VariableVec::iterator it=state_inputs.begin();it!=state_inputs.end();++it)
	{
		if ((*it)->is_Scalar())
			f << "," << std::endl << "                    c_double";
		else
			f << "," << std::endl << "                    ctypeslib.ndpointer(ndim=" << (*it)->getShape().getNrDimensions() << ")";
	}
	for (Graph::VariableVec::iterator it=controller.begin();it!=controller.end();++it)
	{
		if ((*it)->is_Scalar())
			f << "," << std::endl << "                    c_double";
		else
			f << "," << std::endl << "                    ctypeslib.ndpointer(ndim=" << (*it)->getShape().getNrDimensions() << ")";
	}
	f << "]" << std::endl;
	f << std::endl;
    f << "# inputs" << std::endl;
	f << "_inputs = {" << std::endl;
	for (Graph::VariableVec::iterator it=state_inputs.begin();it!=state_inputs.end();++it)
    { // Da wir in C momentan mit echten Vektoren arbeiten müssen wir hier unser eigenes zero bauen... (eigentlich geht das nicht wegen initVal - TODO!)
		//f << "           '" << p.print(*it) << "' : " << p.print(g.getinitVal(*it)) 
        //  << ",   " << p.comment2(g,*it) << std::endl;
        f << "           '" << p.print(*it) << "' : zeros(";
        if ((*it)->is_Scalar()) 
			f << "1";
		else if ((*it)->is_Vector()) 
			f << (*it)->getShape().getNumEl();
		else
			f << "(" << (*it)->getShape().getDimension(1) << "," << (*it)->getShape().getDimension(2) << ")";
		f << "),   " << p.comment2(g,*it) << std::endl;
    }
	f << "          }" << std::endl;        
	f << std::endl;
	f << "def setInputs(ext_inputs):" << std::endl;
	f << "    global _inputs" << std::endl;
	f << "    _inputs.update(ext_inputs)" << std::endl;
	f << std::endl;
	f << "def ode_int(t, y):" << std::endl;
	f << "    global _inputs" << std::endl;
	f << std::endl;
    if (!sensors.empty())
    {
        f << "    sensors = graphSensors(t, y)" << std::endl;
    }
    else
    {
        f << "    sensors = dict()" << std::endl;
    }
    for (Graph::VariableVec::iterator it=controller.begin();it!=controller.end();++it)
	{
        std::string comment_tmp = m_p->comment2(g,*it);
        int comment_length = comment_tmp.length();
        std::string comment = comment_tmp.substr(3, comment_length-5);
        std::vector<std::string> comment_vector = split(comment, ':');
        f << "    " << m_p->print(*it) << " = " << comment_vector.back() << "(t, y, sensors)" << std::endl;
	}
	f << "    yd = zeros_like(y)" << std::endl;
	f << "    cm." << m_name << "_der_state(t, y, yd";
    for (Graph::VariableVec::iterator it=state_inputs.begin();it!=state_inputs.end();++it)
        f << ", _inputs['" << p.print(*it) << "']";
    for (Graph::VariableVec::iterator it=controller.begin();it!=controller.end();++it)
        f << ", " << m_p->print(*it);
    f << ")" << std::endl;
	f << "    return yd" << std::endl;

	f.close();

    if (p.getErrorcount())
        std::cerr << "There have been " << p.getErrorcount(true) << " error(s) during generation of " 
                  << m_name << "_der_state_CWrapper.py. Please have a look at that file to see where the error(s) occured." << std::endl;


	Graph::VariableVec sens_vis = g.getVariables(SENSOR_VISUAL);
	// Visual_sensors Vector sortieren: 
	std::sort(sens_vis.begin(),sens_vis.end(), sortVariableVec);

    filename= m_path + "/" + m_name + "_visual_CWrapper.py";
    f.open(filename.c_str());

	f << "# " << getHeaderLine() << std::endl;
	f << std::endl;
//	f << "from ctypes import c_double" << std::endl;
	f << "from numpy import matrix, array, zeros, ctypeslib" << std::endl;
	f << std::endl;
	f << "cm = ctypeslib.load_library(\"" << m_name << "_visual.so\", \"" << m_path << "\")" << std::endl;
	f << "cm." << m_name << "_visual.argtypes = [ctypeslib.ndpointer()";
	for (Graph::VariableVec::iterator it=sens_vis.begin(); it!=sens_vis.end(); ++it)
	{
		if ((*it)->is_Scalar())
			f << "," << std::endl << "                    c_double";
		else
			f << "," << std::endl << "                    ctypeslib.ndpointer(ndim=" << (*it)->getShape().getNrDimensions() << ")";
	}
	f << "]" << std::endl;
	f << std::endl;

	f << "def graphVisualSensors(q):" << std::endl;
	f << std::endl;
	f << "    q = array(q)   # q ist manchmal Liste, muss aber unbedingt ndarray sein." << std::endl;
	for (Graph::VariableVec::iterator it=sens_vis.begin(); it!=sens_vis.end(); ++it)
	{
		f  << "    " << p.print(*it) << " = zeros(";
		if ((*it)->is_Scalar()) 
			f << "1";
		else if ((*it)->is_Vector()) 
			f << (*it)->getShape().getNumEl();
		else
			f << "(" << (*it)->getShape().getDimension(1) << "," << (*it)->getShape().getDimension(2) << ")";
		f << ")" << std::endl;
	}
	f << std::endl;

	f << "    cm." << m_name << "_visual(q";
	for (Graph::VariableVec::iterator it=sens_vis.begin(); it!=sens_vis.end(); ++it)
		f  << "," << std::endl << "        " << p.print(*it) << "";
	f << ")" << std::endl;
	f << std::endl;

	f << "    res_dict = {}" << std::endl;
	for (Graph::VariableVec::iterator it=sens_vis.begin();it!=sens_vis.end();++it)
		f  << "    res_dict.update({'" << p.print(*it) << "' : " << p.print(*it) << "})" << std::endl; 
	f << std::endl;

    f << "    return res_dict" << std::endl;

	f.close();

    if (p.getErrorcount())
        std::cerr << "There have been " << p.getErrorcount(true) << " error(s) during generation of " 
                  << m_name << "_visual_CWrapper.py. Please have a look at that file to see where the error(s) occured." << std::endl;


    Graph::VariableVec sens_inputs = g.getAssignments(SENSOR)->getVariables(INPUT);
    // Inputs Vector sortieren: 
	std::sort(sens_inputs.begin(),sens_inputs.end(), sortVariableVec);

    filename= m_path + "/" + m_name + "_sensors_CWrapper.py";
    f.open(filename.c_str());

	f << "# " << getHeaderLine() << std::endl;
	f << std::endl;
    f << "from ctypes import c_double" << std::endl;
	f << "from numpy import matrix, array, zeros, ctypeslib" << std::endl;
	f << std::endl;
	f << "cm = ctypeslib.load_library(\"" << m_name << "_sensors.so\", \"" << m_path << "\")" << std::endl;
	f << "cm." << m_name << "_sensors.argtypes = [c_double, ctypeslib.ndpointer()";
    for (Graph::VariableVec::iterator it=sens_inputs.begin();it!=sens_inputs.end();++it)
	{
		if ((*it)->is_Scalar())
			f << "," << std::endl << "                    c_double";
		else
			f << "," << std::endl << "                    ctypeslib.ndpointer(ndim=" << (*it)->getShape().getNrDimensions() << ")";
	}
	for (Graph::VariableVec::iterator it=sensors.begin(); it!=sensors.end(); ++it)
	{
		if ((*it)->is_Scalar())
			f << "," << std::endl << "                    c_double";
		else
		//	f << "," << std::endl << "                    ctypeslib.ndpointer(ndim=" << (*it)->getShape().getNrDimensions() << ")";
            f << "," << std::endl << "                    ctypeslib.ndpointer(ndim=2)";
	}
	f << "]" << std::endl;
	f << std::endl;

    f << "# inputs" << std::endl;
	f << "_inputs = {" << std::endl;
	for (Graph::VariableVec::iterator it=sens_inputs.begin();it!=sens_inputs.end();++it)
    { // Da wir in C momentan mit echten Vektoren arbeiten müssen wir hier unser eigenes zero bauen... (eigentlich geht das nicht wegen initVal - TODO!)
		//f << "           '" << p.print(*it) << "' : " << p.print(g.getinitVal(*it)) 
        //  << ",   " << p.comment2(g,*it) << std::endl;
        f << "           '" << p.print(*it) << "' : zeros(";
        if ((*it)->is_Scalar()) 
			f << "1";
		else if ((*it)->is_Vector()) 
			f << (*it)->getShape().getNumEl();
		else
			f << "(" << (*it)->getShape().getDimension(1) << "," << (*it)->getShape().getDimension(2) << ")";
		f << "),   " << p.comment2(g,*it) << std::endl;
    }
	f << "          }" << std::endl;        
	f << std::endl;
	f << "def setInputs(ext_inputs):" << std::endl;
	f << "    global _inputs" << std::endl;
	f << "    _inputs.update(ext_inputs)" << std::endl;
	f << std::endl;

	f << "def graphSensors(t, q):" << std::endl;
	f << std::endl;
	f << "    q = array(q)   # q ist manchmal Liste, muss aber unbedingt ndarray sein." << std::endl;
	for (Graph::VariableVec::iterator it=sensors.begin(); it!=sensors.end(); ++it)
	{
		f  << "    " << p.print(*it) << " = zeros(";
		if ((*it)->is_Scalar()) 
			f << "1";
//		else if ((*it)->is_Vector()) 
	//		f << (*it)->getShape().getNumEl();
		else
			f << "(" << (*it)->getShape().getDimension(1) << "," << (*it)->getShape().getDimension(2) << ")";
		f << ")" << std::endl;
	}
	f << std::endl;

	f << "    cm." << m_name << "_sensors(t, q";
    for (Graph::VariableVec::iterator it=sens_inputs.begin();it!=sens_inputs.end();++it)
        f << "," << std::endl << "        _inputs['" << p.print(*it) << "']";
	for (Graph::VariableVec::iterator it=sensors.begin(); it!=sensors.end(); ++it)
		f  << "," << std::endl << "        " << p.print(*it) << "";
	f << ")" << std::endl;
	f << std::endl;

	f << "    res_dict = {}" << std::endl;
	for (Graph::VariableVec::iterator it=sensors.begin();it!=sensors.end();++it)
		f  << "    res_dict.update({'" << p.print(*it) << "' : " << p.print(*it) << "})" << std::endl; 
	f << std::endl;

    f << "    return res_dict" << std::endl;

	f.close();

    if (p.getErrorcount())
        std::cerr << "There have been " << p.getErrorcount(true) << " error(s) during generation of " 
                  << m_name << "_sensors_CWrapper.py. Please have a look at that file to see where the error(s) occured." << std::endl;

	return Util::getTime() - t1;
}
/*****************************************************************************/


/*****************************************************************************/
double CWriter::generateFunctionmodule(int n)
/*****************************************************************************/
{
	//get timestamp and catch variables from graph
	double t1 = Util::getTime();

	std::ofstream f;
    std::string filename= m_path + "/functionmodule.c";
    f.open(filename.c_str());

	f << "/* " << getHeaderLine() << " */" << std::endl;
	f << std::endl;

	f << "/*************************    Program 4.3    ****************************/" << std::endl;
	f << "/*                                                                      */" << std::endl;
	f << "/************************************************************************/" << std::endl;
	f << "/* Please Note:                                                         */" << std::endl;
	f << "/*                                                                      */" << std::endl;
	f << "/* (1) This computer program is written by Tao Pang in conjunction with */" << std::endl;
	f << "/*     his book, \"An Introduction to Computational Physics,\" published  */" << std::endl;
	f << "/*     by Cambridge University Press in 1997.                           */" << std::endl;
	f << "/*                                                                      */" << std::endl;
	f << "/* (2) No warranties, express or implied, are made for this program.    */" << std::endl;
	f << "/*                                                                      */" << std::endl;
	f << "/************************************************************************/" << std::endl;
	f << std::endl;
	f << "#include <stdio.h>" << std::endl;
	f << "#include <math.h>" << std::endl;
	f << "#define n " << n << std::endl;
	f << std::endl;
	f << "void elgs(double A[n][n], int *indx);" << std::endl;
	f << std::endl;
	f << "void legs (double A[n][n], double *b, double *x)" << std::endl;
	f << "/* Function to solve the equation A[n][n]*x[n] = b[n] with the" << std::endl;
	f << "   partial-pivoting Gaussian elimination scheme." << std::endl;
	f << "   Copyright (c) Tao Pang 2001. */" << std::endl;
	f << "{" << std::endl;
	f << "  int indx[n];" << std::endl;
	f << "  int i, j;" << std::endl;
	f << std::endl;
	f << "  elgs (A,indx);" << std::endl;
	f << std::endl;
	f << "  for(i = 0; i < n-1; ++i)" << std::endl;
	f << "  {" << std::endl;
	f << "    for(j = i+1; j < n; ++j)" << std::endl;
	f << "    {" << std::endl;
	f << "      b[indx[j]] = b[indx[j]]-A[indx[j]][i]*b[indx[i]];" << std::endl;
	f << "    }" << std::endl;
	f << "  }" << std::endl;
	f << std::endl;
	f << "  x[n-1] = b[indx[n-1]]/A[indx[n-1]][n-1];" << std::endl;
	f << "  for (i = n-2; i>=0; i--)" << std::endl;
	f << "  {" << std::endl;
	f << "    x[i] = b[indx[i]];" << std::endl;
	f << "    for (j = i+1; j < n; ++j)" << std::endl;
	f << "    {" << std::endl;
	f << "      x[i] = x[i]-A[indx[i]][j]*x[j];" << std::endl;
	f << "    }" << std::endl;
	f << "    x[i] = x[i]/A[indx[i]][i];" << std::endl;
	f << "  }" << std::endl;
	f << std::endl;
	f << "}" << std::endl;
	f << std::endl;
	f << "void elgs (double A[n][n], int *indx)" << std::endl;
	f << std::endl;
	f << "/* Function to perform the partial pivoting Gaussian elimination." << std::endl;
	f << "   A[][] is the original matrix in the input and transformed" << std::endl;
	f << "   matrix plus the pivoting element ratios below the diagonal" << std::endl;
	f << "   in the output.  indx[] records the pivoting order." << std::endl;
	f << "   Copyright (c) Tao Pang 2001. */" << std::endl;
	f << std::endl;
	f << "{" << std::endl;
	f << "  double c[n];" << std::endl;
	f << "  int i, j, l;" << std::endl;
	f << std::endl;
	f << "/* Initialize the index */" << std::endl;
	f << std::endl;
	f << "  for (i = 0; i < n; ++i)" << std::endl;
	f << "  {" << std::endl;
	f << "    indx[i] = i;" << std::endl;
	f << "  }" << std::endl;
	f << std::endl;
	f << "/* Find the rescaling factors, one from each row */" << std::endl;
	f << " " << std::endl;
	f << "  for (i = 0; i < n; ++i)" << std::endl;
	f << "  {" << std::endl;
	f << "    double c1 = 0;" << std::endl;
	f << "    for (j = 0; j < n; ++j)" << std::endl;
	f << "    {" << std::endl;
	f << "      if (fabs(A[i][j]) > c1) c1 = fabs(A[i][j]);" << std::endl;
	f << "    }" << std::endl;
	f << "	c[i] = c1;" << std::endl;
	f << "  }" << std::endl;
	f << std::endl;
	f << "/* Search the pivoting (largest) element from each column */ " << std::endl;
	f << std::endl;
	f << "  for (j = 0; j < n-1; ++j)" << std::endl;
	f << "  {" << std::endl;
	f << "    int k = 0;" << std::endl;
	f << "	  int itmp = 0;" << std::endl;
	f << "    double pi1 = 0;" << std::endl;
	f << "    for (i = j; i < n; ++i)" << std::endl;
	f << "    {" << std::endl;
	f << "      double pi = fabs(A[indx[i]][j])/c[indx[i]];" << std::endl;
	f << "      if (pi > pi1)" << std::endl;
	f << "      {" << std::endl;
	f << "        pi1 = pi;" << std::endl;
	f << "        k = i;" << std::endl;
	f << "      }" << std::endl;
	f << "    }" << std::endl;
	f << std::endl;
	f << "/* Interchange the rows via indx[] to record pivoting order */" << std::endl;
	f << std::endl;
	f << "    itmp = indx[j];" << std::endl;
	f << "    indx[j] = indx[k];" << std::endl;
	f << "    indx[k] = itmp;" << std::endl;
	f << "    for (i = j+1; i < n; ++i)" << std::endl;
	f << "    {" << std::endl;
	f << "      double pj = A[indx[i]][j]/A[indx[j]][j];" << std::endl;
	f << std::endl;
	f << "/* Record pivoting ratios below the diagonal */" << std::endl;
	f << std::endl;
	f << "      A[indx[i]][j] = pj;" << std::endl;
	f << std::endl;
	f << "/* Modify other elements accordingly */" << std::endl;
	f << std::endl;
	f << "      for (l = j+1; l < n; ++l)" << std::endl;
	f << "      {" << std::endl;
	f << "        A[indx[i]][l] = A[indx[i]][l]-pj*A[indx[j]][l];" << std::endl;
	f << "      }" << std::endl;
	f << "    }" << std::endl;
	f << "  }" << std::endl;
	f << "}" << std::endl;
	f << std::endl;

	f.close();
	return Util::getTime() - t1;
}
/*****************************************************************************/

/*****************************************************************************/
double CWriter::generateAll(Graph::Graph& g, int &dim)
/*****************************************************************************/
{
	//get timestamp and catch variables from graph
	double t1 = Util::getTime();

	std::ofstream f;
    std::string filename= m_path + "/" + m_name + ".c"; 
    f.open(filename.c_str());

	Symbolics::Graph::Category_Type cats = DER_STATE|SENSOR;
	if (m_include_visual)
		cats |= SENSOR_VISUAL;
	Graph::AssignmentsPtr a = g.getAssignments(cats);

	Graph::VariableVec states = a->getVariables(STATE);
	cats = VARIABLE;
	if (!m_include_visual)
		cats |= SENSOR_VISUAL;
	Graph::VariableVec variables = a->getVariables(cats);
	Graph::VariableVec inputs = a->getVariables(INPUT);
	Graph::VariableVec parameters = a->getVariables(PARAMETER);
	Graph::VariableVec constants = a->getVariables(CONSTANT);
	Graph::VariableVec userexp = a->getVariables(USER_EXP);

	// StateVariables Vector sortieren:
	std::sort(states.begin(),states.end(), sortVariableVec);

	f << "/* " << getHeaderLine() << " */" << std::endl;
	f << "/*";
	f << "#include <math.h>" << std::endl;
	f << "#include \"functionmodule.c\"" << std::endl;
	f << "*/";
	f << std::endl;

	f << "/* declare state variables */" << std::endl;
	for (size_t i=0; i < states.size(); ++i)
	{
		size_t n = states.at(i)->getShape().getNumEl();
		dim = n;
		f << "    const double "  << m_p->print(states.at(i)) << m_p->dimension(states.at(i)) << " = " << ( n>1 ? "{" : "");
		for (size_t j=0; j < n ; ++j)
			f << "y[" << i*n+j << "]" << (j+1 < n ? ", " : "");
		f << ( n>1 ? "}" : "") << ";" << m_p->comment2(g, states.at(i)) << std::endl;
		f << "    double " << "der_" << m_p->print(states.at(i)) << m_p->dimension(states.at(i)) << " = " 
          << m_p->print(Zero::getZero(states.at(i)->getShape())) << ";" << std::endl;	
	}

	f << "/* Parameters */" << std::endl;
	for (Graph::VariableVec::iterator it=parameters.begin();it!=parameters.end();++it)
		f << "    const double " << m_p->print(*it) << m_p->dimension(*it) << "=" << m_p->print(g.getEquation(*it)) << "; " << m_p->comment2(g,*it) <<  std::endl;
	f << std::endl;

	f << "/* Constants */" << std::endl;
	for (Graph::VariableVec::iterator it=constants.begin();it!=constants.end();++it)
        f << "    const double " << m_p->print(*it) << m_p->dimension(*it) << "=" << m_p->print(g.getEquation(*it)) << "; " << m_p->comment2(g,*it) <<  std::endl;
	f << std::endl;

	f << "/* User Expression variables */" << std::endl;
	for (Graph::VariableVec::iterator it=userexp.begin();it!=userexp.end();++it)
		f << "    double " << m_p->print(*it) << m_p->dimension(*it) << "= " << m_p->print(g.getinitVal(*it)) << "; " << m_p->comment2(g,*it) <<  std::endl;
	f << std::endl;

	f << "/* ordinary variables */" << std::endl;
    for (Graph::VariableVec::iterator it=variables.begin();it!=variables.end();++it)
        f << "    double " << m_p->print(*it) << m_p->dimension(*it) << "= " << m_p->print(g.getinitVal(*it)) << "; " << m_p->comment2(g,*it) <<  std::endl;
	f << std::endl;

	f << "/* calculate sensors and state derivative */" << std::endl;
	f << writeEquations(a->getEquations(PARAMETER | CONSTANT | INPUT )) << std::endl;
    f << std::endl;

	f << "/* set return values */" << std::endl;
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
	
	f.close();

    if (m_p->getErrorcount())
        std::cerr << "There have been " << m_p->getErrorcount(true) << " error(s) during generation of " 
                  << m_name << ".c. Please have a look at that file to see where the error(s) occured." << std::endl;

	return Util::getTime() - t1;
}
/*****************************************************************************/
