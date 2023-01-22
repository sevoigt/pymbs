#include "MatlabWriter.h"
#include "MatlabSymPrinter.h"
#include "str.h"
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace Symbolics;

/*****************************************************************************/
MatlabWriter::MatlabWriter( std::map<std::string, std::string> &kwds ) : Writer(false)
/*****************************************************************************/
{
    m_symbolicmode=false;
    if (kwds.find("symbolic_mode") != kwds.end())
        m_symbolicmode = (kwds["symbolic_mode"] == "True");
    if (m_symbolicmode)
        m_p = new MatlabSymPrinter();
    else
        m_p = new MatlabPrinter();
}
/*****************************************************************************/

/*****************************************************************************/
MatlabWriter::MatlabWriter():Writer(false)
/*****************************************************************************/
{
    m_symbolicmode=false;
    m_p = new MatlabPrinter();
}
/*****************************************************************************/

/*****************************************************************************/
MatlabWriter::~MatlabWriter()
/*****************************************************************************/
{
    delete m_p;
}
/*****************************************************************************/


/*****************************************************************************/
double MatlabWriter::generateTarget_Impl(Graph::Graph &g)
/*****************************************************************************/
{
	double t1=0, t2=0, t3=0, t4=0;
	if (!m_symbolicmode) {
		t1 = generateBasicSim(g);
		t2 = generateDerState(g);
		t3 = generateInputs(g);
		t4 = generateSensors(g);
	} else {
		t2 = generateSymDerState(g);
		t4 = generateSymSensors(g);
	}

	return t1+t2+t3+t4;
	
}
/*****************************************************************************/

/*****************************************************************************/
double MatlabWriter::generateBasicSim(Graph::Graph& g)
/*****************************************************************************/
{
	//get timestamp and catch variables from graph
	double t1 = Util::getTime();

	std::ofstream f;
    std::string filename= m_path + "/" + m_name + "_sim.m";
    f.open(filename.c_str());

	Graph::AssignmentsPtr a = g.getAssignments(DER_STATE);

	Graph::VariableVec sensors = a->getVariables(SENSOR);
	Graph::VariableVec states = a->getVariables(STATE);
	if (states.empty())
		throw InternalError("System without States, Feature is not yet Implemented in Matlab");

	// StateVariables Vector sortieren:
	std::sort(states.begin(),states.end(), sortVariableVec);

	//weil es h�ufiger verwendet wird:
	size_t state0_dim1 = states.at(0)->getShape().getDimension(1);
    
	f << "% " << getHeaderLine() << std::endl;
	f << std::endl;
	f << "% Reset" << std::endl;
	f << "close all" << std::endl;
	f << "clear" << std::endl;
	f << "clc" << std::endl;
	f << std::endl;
	f << "% Set initial Values" << std::endl;
	for (Graph::VariableVec::iterator it=states.begin();it!=states.end();++it)
        f << "   " << m_p->print(*it) << "0 = " << m_p->print(g.getinitVal(*it)) << "; " << m_p->comment2(g,*it) << std::endl;
	f << std::endl;
	f << "y0 = [";
	for (Graph::VariableVec::iterator it=states.begin();it!=states.end();++it)
		f << m_p->print(*it) << "0" << (it+1 != states.end() ? "; " : "" ); //Matlab k�me zwar auch mit dem zus�tzlichen Komma am Ende klar, aber wir wollen ja sch�nen Code...
	f << "];" << std::endl; 
	f << std::endl;
	f << "% Configure simulation" << std::endl;
	f << "tSpan = 0:0.01:10;" << std::endl;
	f << "options = odeset('AbsTol', 1e-6, 'RelTol', 1e-3);" << std::endl; //...?
	f << std::endl;
	f << "% Start simulation" << std::endl;
	f << "[T, Y] = ode15s(@"<< m_name <<"_der_state, tSpan, y0, options);" << std::endl;
	f << std::endl;
	f << "% Output" << std::endl;
	f << "plot(T,Y(:,1:";
	f << state0_dim1 << "));" << std::endl;
	f << "legend(";
	for (size_t i = 0; i < state0_dim1; ++i)
		f << "'" << g.getComment(states.at(0)) << i+1 << "'" << (i+1 < state0_dim1 ? ", " : "");
	f << ");" << std::endl;
	// TODO: Keine wirklich sinnvolle Legende, geht das auch besser?
	f << std::endl;
	
    if (sensors.empty())
    {
        std::cerr << "Warning: MatlabWriter: Not generating sensors file, since there are no sensors." << std::endl;
    }
    else
    {
        f << "% Sensors" << std::endl;
        f << "for i=numel(T):-1:1" << std::endl;
        f << "   S(i) = " << m_name << "_sensors(T(i), Y(i,:)');" << std::endl;
        f << "end" << std::endl;
        f << std::endl;
    }

	f << "% Export results to MAT-file, which is playable in PyMBS GUI. This is disabled by default, just uncomment the following lines" << std::endl;
	f << "% PyMbs needs column vectors which are named T and Y" << std::endl;
	f << "%T=T';" << std::endl << "%Y=Y';" << std::endl;
	f << "%save ('" << m_name << "_results.mat', 'T','Y');" << std::endl;
	f << "% Revert first transpose" << std::endl;
	f << "%T=T';" << std::endl << "%Y=Y';" << std::endl;
	f << std::endl;

	f.close();

    if (m_p->getErrorcount())
        std::cerr << "There have been " << m_p->getErrorcount(true) << " error(s) during generation of " 
                  << m_name << "_sim.m. Please have a look at that file to see where the error(s) occured." << std::endl;
	return Util::getTime() - t1;
}
/*****************************************************************************/

/*****************************************************************************/
double MatlabWriter::generateDerState(Graph::Graph& g)
/*****************************************************************************/
{
	//get timestamp and catch variables from graph
	double t1 = Util::getTime();

	Graph::AssignmentsPtr a = g.getAssignments(DER_STATE);

	Graph::VariableVec states = a->getVariables(STATE);
	if (states.empty())
		throw InternalError("Error: Models without states are not yet implemented in Matlab!");
	Graph::VariableVec inputs = a->getVariables(INPUT);
	Graph::VariableVec parameter = a->getVariables(PARAMETER);
	Graph::VariableVec constants = a->getVariables(CONSTANT);

	// StateVariables Vector sortieren:
	std::sort(states.begin(),states.end(), sortVariableVec);
	
	std::ofstream f;
	std::string filename= m_path + "/" + m_name + "_der_state.m";
	f.open(filename.c_str());

	f << "% " << getHeaderLine() << std::endl;
	f << std::endl;
	
	f << "function yd = "<< m_name <<"_der_state(time, y)" << std::endl;

	f << "% State" << std::endl;
	for (Graph::VariableVec::size_type i = 0; i < states.size() ; ++i)
	{
		f << "	" << m_p->print(states.at(i)) << " = y("
		  << str(states.at(i)->getShape().getDimension(1) * i + 1) << ":" 
		  << str(states.at(i)->getShape().getDimension(1) * (i+1)) << ");" 
		  << std::endl;
	}
	f << std::endl;

	f << "% Inputs" << std::endl;
	f << "	I = "<< m_name <<"_inputs();" << std::endl;
	for (Graph::VariableVec::iterator it=inputs.begin();it!=inputs.end();++it)
        f << "	" << m_p->print(*it) << " = I." << m_p->print(*it) << "; " << m_p->comment2(g,*it) << std::endl;
	f << std::endl;

	f << "% Parameters" << std::endl;
	for (Graph::VariableVec::iterator it=parameter.begin();it!=parameter.end();++it)
		f << "    " << m_p->print(*it) << " = " << m_p->print(g.getEquation(*it)) << "; " << m_p->comment2(g,*it) << std::endl;
	f << std::endl;

	f << "% Constants" << std::endl;
	for (Graph::VariableVec::iterator it=constants.begin();it!=constants.end();++it)
        f << "	" << m_p->print(*it) << " = " << m_p->print(g.getEquation(*it)) << "; " << m_p->comment2(g,*it) << std::endl;
	f << std::endl;

	f << "% Equations" << std::endl;
	f << writeEquations(a->getEquations(PARAMETER | CONSTANT | INPUT )) << std::endl;

	f << "	yd = [";
	for (Graph::VariableVec::iterator it=states.begin();it!=states.end();++it)
		f << (it == states.begin() ? "" : "; " ) << "der_"<< m_p->print(*it);
	f << "];" << std::endl;
	f << "end" << std::endl;

	f.close();

    if (m_p->getErrorcount())
        std::cerr << "There have been " << m_p->getErrorcount(true) << " error(s) during generation of " 
                  << m_name << "_der_state.m. Please have a look at that file to see where the error(s) occured." << std::endl;
	return Util::getTime() - t1;
}
/*****************************************************************************/

/*****************************************************************************/
double MatlabWriter::generateSymDerState(Graph::Graph& g)
/*****************************************************************************/
{
	//get timestamp and catch variables from graph
	double t1 = Util::getTime();

	std::ofstream f;
    std::string filename= m_path + "/" + m_name + "_der_state_symbolic.m";
    f.open(filename.c_str());

	Graph::AssignmentsPtr a = g.getAssignments(DER_STATE);

	Graph::VariableVec states = a->getVariables(STATE);
    Graph::VariableVec variables = a->getVariables(VARIABLE|SENSOR|SENSOR_VISUAL);
	Graph::VariableVec inputs = a->getVariables(INPUT);
	Graph::VariableVec parameter = a->getVariables(PARAMETER);
	Graph::VariableVec constants = a->getVariables(CONSTANT);
	Graph::VariableVec userexp = a->getVariables(USER_EXP);

	// StateVariables Vector sortieren:
	std::sort(states.begin(),states.end(), sortVariableVec);

	f << "% " << getHeaderLine() << std::endl;
	f << std::endl;
	
	f << "function yd = "<< m_name <<"_der_state_symbolic" << std::endl;

	f << "% State" << std::endl;
	for (Graph::VariableVec::iterator it=states.begin();it!=states.end();++it)
		f << "	" << m_p->print(*it) << " = sym ('" << m_p->print(*it) << "'" << m_p->dimension(*it) << ")" << m_p->comment2(g,*it) << std::endl;
	f << std::endl;

    f << "% Time" << std::endl;
    f << "  syms time" << std::endl;
    f << std::endl;

	f << "% Inputs" << std::endl;
	for (Graph::VariableVec::iterator it=inputs.begin();it!=inputs.end();++it)
        f << "	" << m_p->print(*it) << " = sym ('" << m_p->print(*it) << "'" << m_p->dimension(*it) << ")" << m_p->comment2(g,*it) << std::endl;
	f << std::endl;

	f << "% Parameters" << std::endl;
	for (Graph::VariableVec::iterator it=parameter.begin();it!=parameter.end();++it) {
		f << "	" << m_p->print(*it) << " = sym ('" << m_p->print(*it) << "'" << m_p->dimension(*it) << ")" << m_p->comment2(g,*it) << std::endl;
		f << "	%" << m_p->print(*it) << " = " << m_p->print(g.getEquation(*it)) << ";" << std::endl;
	}
	f << std::endl;

	f << "% Constants" << std::endl;
	for (Graph::VariableVec::iterator it=constants.begin();it!=constants.end();++it) {
		f << "	" << m_p->print(*it) << " = sym ('" << m_p->print(*it) << "'" << m_p->dimension(*it) << ")" << m_p->comment2(g,*it) << std::endl;
        f << "	% " << m_p->print(*it) << " = " << m_p->print(g.getEquation(*it)) << ";" << std::endl;
	}
	f << std::endl;

	f << "% Equations" << std::endl;
	f << writeEquations(a->getEquations(PARAMETER | CONSTANT | INPUT )) << std::endl;

	f << "	yd = [";
	for (Graph::VariableVec::iterator it=states.begin();it!=states.end();++it)
		f << (it == states.begin() ? "" : "; " ) << "der_"<< m_p->print(*it);
	f << "];" << std::endl;
	f << "end" << std::endl;

	f.close();

    if (m_p->getErrorcount())
        std::cerr << "There have been " << m_p->getErrorcount(true) << " error(s) during generation of " 
                  << m_name << "_der_state_symbolic.m. Please have a look at that file to see where the error(s) occured." << std::endl;
	return Util::getTime() - t1;
}
/*****************************************************************************/

/*****************************************************************************/
double MatlabWriter::generateInputs(Graph::Graph& g)
/*****************************************************************************/
{
	//get timestamp and catch variables from graph
	double t1 = Util::getTime();

	std::ofstream f;
    std::string filename= m_path + "/" + m_name + "_inputs.m";
    f.open(filename.c_str());
	Graph::VariableVec inputs = g.getVariables(INPUT);

	f << "% " << getHeaderLine() << std::endl;
	f << std::endl;
	f << "function I = "<< m_name <<"_inputs()" << std::endl;
	f << std::endl;
	if (inputs.begin()==inputs.end())
		f << "	I = [];" << std::endl; 
	 // folgende zwei Schleifen werden ja nicht ausgef�hrt wenn obiges zutrifft
	for (Graph::VariableVec::iterator it=inputs.begin();it!=inputs.end();++it)
		f << "	" << m_p->print(*it) << " = " << m_p->print(g.getinitVal(*it)) << "; " << m_p->comment2(g,*it) << std::endl;
	f << std::endl;
	for (Graph::VariableVec::iterator it=inputs.begin();it!=inputs.end();++it)
        f << "	I." << m_p->print(*it) << " = " << m_p->print(*it) << ";" << std::endl;

	f << "end" << std::endl;

	f.close();

    if (m_p->getErrorcount())
        std::cerr << "There have been " << m_p->getErrorcount(true) << " error(s) during generation of " 
                  << m_name << "_inputs.m. Please have a look at that file to see where the error(s) occured." << std::endl;
	return Util::getTime() - t1;
}
/*****************************************************************************/


/*****************************************************************************/
double MatlabWriter::generateSensors(Graph::Graph& g)
/*****************************************************************************/
{
	//get timestamp and catch variables from graph
	double t1 = Util::getTime();

	Graph::AssignmentsPtr a = g.getAssignments(SENSOR);

	Graph::VariableVec states = a->getVariables(STATE);
	Graph::VariableVec inputs = a->getVariables(INPUT);
	Graph::VariableVec parameter = a->getVariables(PARAMETER);
	Graph::VariableVec constants = a->getVariables(CONSTANT);
	Graph::VariableVec sensors = a->getVariables(SENSOR);
    if (sensors.empty())
    {
        std::cerr << "Warning: MatlabWriter: Not generating sensors file, since there are no sensors." << std::endl;
        return Util::getTime() - t1;
    }

	// StateVariables Vector sortieren:
	std::sort(states.begin(),states.end(), sortVariableVec);

	std::ofstream f;
    std::string filename= m_path + "/" + m_name + "_sensors.m";
    f.open(filename.c_str());

	f << "% " << getHeaderLine() << std::endl;
	f << std::endl;
	f << "function S = "<< m_name <<"_sensors(time, y)" << std::endl;
	f << std::endl;
	f << "% make sure y is a column vector (uncomment if not needed to save time)" << std::endl;
	f << "	y = y(:);" << std::endl;
	f << std::endl;
	f << "% State" << std::endl;

	for (Graph::VariableVec::size_type i = 0; i < states.size() ; ++i)
	{
		const Symbolics::BasicPtr s = states.at(i);
		f << "	" << m_p->print(s) << " = y("
		  << str(s->getShape().getDimension(1) * i + 1) << ":" 
		  << str(s->getShape().getDimension(1) * (i+1)) << ");" 
		  << std::endl;
	}

	f << "% Inputs" << std::endl;
	f << "	I = "<< m_name <<"_inputs();" << std::endl;
	for (Graph::VariableVec::iterator it=inputs.begin();it!=inputs.end();++it)
        f << "	" << m_p->print(*it) << " = I." << m_p->print(*it) << "; " << m_p->comment2(g,*it) << std::endl;
	f << std::endl;

	f << "% Parameters" << std::endl;
	for (Graph::VariableVec::iterator it=parameter.begin();it!=parameter.end();++it)
		f << "    " << m_p->print(*it) << " = " << m_p->print(g.getEquation(*it)) << "; " << m_p->comment2(g,*it) << std::endl;
	f << std::endl;

	f << "% Constants" << std::endl;
	for (Graph::VariableVec::iterator it=constants.begin();it!=constants.end();++it)
        f << "	" << m_p->print(*it) << " = " << m_p->print(g.getEquation(*it)) << "; " << m_p->comment2(g,*it) << std::endl;
	f << std::endl;

	f << "% Equations and Sensors" << std::endl;
	f << writeEquations(a->getEquations(PARAMETER | CONSTANT | INPUT )) << std::endl;

	f << "% Struct including sensor-values" << std::endl;
	for (Graph::VariableVec::iterator it=sensors.begin();it!=sensors.end();++it)
        f << "S." << m_p->print(*it) << " = " << m_p->print(*it) << "; " << std::endl;
	f << std::endl;

	f << "end" << std::endl;

	f.close();

    if (m_p->getErrorcount())
        std::cerr << "There have been " << m_p->getErrorcount(true) << " error(s) during generation of " 
                  << m_name << "_sensors.m. Please have a look at that file to see where the error(s) occured." << std::endl;
	return Util::getTime() - t1;
}
/*****************************************************************************/

/*****************************************************************************/
double MatlabWriter::generateSymSensors(Graph::Graph& g)
/*****************************************************************************/
{
	//get timestamp and catch variables from graph
	double t1 = Util::getTime();

	Graph::AssignmentsPtr a = g.getAssignments(SENSOR);

	Graph::VariableVec states = a->getVariables(STATE);
	Graph::VariableVec inputs = a->getVariables(INPUT);
	Graph::VariableVec parameter = a->getVariables(PARAMETER);
	Graph::VariableVec constants = a->getVariables(CONSTANT);
	Graph::VariableVec sensors = a->getVariables(SENSOR);
    if (sensors.empty())
    {
        std::cerr << "Warning: MatlabWriter: Not generating symbolic sensors file, since there are no sensors." << std::endl;
        return Util::getTime() - t1;
    }

	// StateVariables Vector sortieren:
	std::sort(states.begin(),states.end(), sortVariableVec);

	std::ofstream f;
    std::string filename= m_path + "/" + m_name + "_sensors_symbolic.m";
    f.open(filename.c_str());

	f << "% " << getHeaderLine() << std::endl;
	f << std::endl;
	f << "function S = "<< m_name <<"_sensors_symbolic" << std::endl;
	f << std::endl;
	f << "% State" << std::endl;
	for (Graph::VariableVec::iterator it=states.begin();it!=states.end();++it)
		f << "	" << m_p->print(*it) << " = sym ('" << m_p->print(*it) << "'" << m_p->dimension(*it) << ")" << m_p->comment2(g,*it) << std::endl;
	f << std::endl;

    f << "% Time" << std::endl;
    f << "  syms time" << std::endl;
    f << std::endl;

	f << "% Inputs" << std::endl;
	for (Graph::VariableVec::iterator it=inputs.begin();it!=inputs.end();++it)
        f << "	" << m_p->print(*it) << " = sym ('" << m_p->print(*it) << "'" << m_p->dimension(*it) << ")" << m_p->comment2(g,*it) << std::endl;
	f << std::endl;

	f << "% Parameters" << std::endl;
	for (Graph::VariableVec::iterator it=parameter.begin();it!=parameter.end();++it) {
		f << "	" << m_p->print(*it) << " = sym ('" << m_p->print(*it) << "'" << m_p->dimension(*it) << ")" << m_p->comment2(g,*it) << std::endl;
		f << "	%" << m_p->print(*it) << " = " << m_p->print(g.getEquation(*it)) << ";" << std::endl;
	}
	f << std::endl;

	f << "% Constants" << std::endl;
	for (Graph::VariableVec::iterator it=constants.begin();it!=constants.end();++it) {
		f << "	" << m_p->print(*it) << " = sym ('" << m_p->print(*it) << "'" << m_p->dimension(*it) << ")" << m_p->comment2(g,*it) << std::endl;
        f << "	% " << m_p->print(*it) << " = " << m_p->print(g.getEquation(*it)) << ";" << std::endl;
	}
	f << std::endl;

	f << "% Equations" << std::endl;
	f << writeEquations(a->getEquations(PARAMETER | CONSTANT | INPUT | SENSOR )) << std::endl;

	f << "% Sensors" << std::endl;
    for (Graph::VariableVec::iterator it=sensors.begin();it!=sensors.end();++it)
        f << "	" << m_p->print(*it) << " = " << m_p->print(g.getEquation(*it)) << " " << m_p->comment2(g,*it) << std::endl;
	f << std::endl;

	f << "% Struct including sensor-values" << std::endl;
	for (Graph::VariableVec::iterator it=sensors.begin();it!=sensors.end();++it)
        f << "S." << m_p->print(*it) << " = " << m_p->print(*it) << "; " << std::endl;
	f << std::endl;

	f << "end" << std::endl;

	f.close();

    if (m_p->getErrorcount())
        std::cerr << "There have been " << m_p->getErrorcount(true) << " error(s) during generation of " 
                  << m_name << "_sensors_symbolic.m. Please have a look at that file to see where the error(s) occured." << std::endl;
	return Util::getTime() - t1;
}
/*****************************************************************************/


/*****************************************************************************/
std::string MatlabWriter::writeEquations(std::vector<Graph::Assignment> const& equations) const
/*****************************************************************************/
{
	std::stringstream s;

	for (std::vector<Graph::Assignment>::const_iterator it=equations.begin(); it!=equations.end(); ++it)
    {
        if (it->implizit)
            throw InternalError("Implicit equations are not yet implemented in Matlab!");
		//Folgendes falls mehrere Gleichungen in einer verpackt sind (wird aber scheinbar kaum genutzt)
		for (size_t i=0; i < it->lhs.size(); ++i)
        {
            BasicPtr simple_exp = it->rhs[i]->simplify();
            if (simple_exp.get() == NULL) throw InternalError("MatlabWriter: Value of Rhs is not Valid!");

			//Workaround: links mehrere Ergebnisse
			if (it->lhs[i]->getType() == Type_Matrix)
			{
				const Matrix *mat = Util::getAsConstPtr<Matrix>(it->lhs[i]);
				if (simple_exp->getType() == Type_Symbol)
				{  // Wir k�nnen es ohne tempor�re Variable aufl�sen
					for (size_t j=0; j < mat->getShape().getDimension(1); j++)
						for (size_t k=0; k < mat->getShape().getDimension(2); k++)
							s << "    " << m_p->print(mat->get(j,k)) << " = " << m_p->print(simple_exp) << "(" << j+1<< "," << k+1 << ")" << (m_symbolicmode ? "" : ";") << std::endl;
				} else 	{  // Es muss eine tempor�re Variable eingef�hrt werden
					s << "    temp_var_1 = " << m_p->print(simple_exp) << (m_symbolicmode ? "" : ";") << std::endl; // TODO: So ein fester Name ist nicht sch�n, wie lassen sich Kollisionen vermeiden?
					for (size_t j=0; j < mat->getShape().getDimension(1); j++)
						for (size_t k=0; k < mat->getShape().getDimension(2); k++)
							s << "    " << m_p->print(mat->get(j,k)) << " = temp_var_1(" << j+1<< "," << k+1 << ")" << (m_symbolicmode ? "" : ";") << std::endl;
				}
			} else 	{
				// ordinary equation
				s << "	" << m_p->print(it->lhs[i]) << " = " << m_p->print(simple_exp) << (m_symbolicmode ? "" : ";") << std::endl;
			}
        }
    }

	return s.str();
}
/*****************************************************************************/
