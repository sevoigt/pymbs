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

#include "FortranWriter.h"
#include "PythonPrinter.h"
#include "str.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>

using namespace Symbolics;


/*****************************************************************************/
FortranWriter::FortranWriter( std::map<std::string, std::string> &kwds ):Writer(false)
/*****************************************************************************/
{
	m_pymbs_wrapper=false;
	if (kwds.find("pymbs_wrapper") != kwds.end())
		m_pymbs_wrapper = (kwds["pymbs_wrapper"] == "True");
    m_p = new FortranPrinter();
}
/*****************************************************************************/


/*****************************************************************************/
FortranWriter::FortranWriter():Writer(false)
/*****************************************************************************/
{
	m_pymbs_wrapper=false;
    m_p = new FortranPrinter();
}
/*****************************************************************************/


/*****************************************************************************/
FortranWriter::~FortranWriter()
/*****************************************************************************/
{
    delete m_p;
}
/*****************************************************************************/

/*****************************************************************************/
double FortranWriter::generateTarget_Impl(Graph::Graph &g)
/*****************************************************************************/
{
	double t1 = generateDerState(g);
	double t2 = generateSensors(g);
	double t3 = generateVisual(g);
	double t4 = generateFunctionmodule();
	double t5 = 0.0;
	if (m_pymbs_wrapper)
		t5 = generatePymbsWrapper(g);

	return t1+t2+t3+t4+t5;
}
/*****************************************************************************/

/*****************************************************************************/
double FortranWriter::generateDerState(Graph::Graph& g)
/*****************************************************************************/
{
	//get timestamp and catch variables from graph
	double t1 = Util::getTime();

	Graph::AssignmentsPtr a = g.getAssignments(DER_STATE);

	Graph::VariableVec states = a->getVariables(STATE);
	if (states.empty())
		throw InternalError("Error: Models without states are not implemented yet in Fortran!");

    Graph::VariableVec variables = a->getVariables(VARIABLE|SENSOR|SENSOR_VISUAL);
	Graph::VariableVec inputs = a->getVariables(INPUT);
	Graph::VariableVec parameter = a->getVariables(PARAMETER);
	Graph::VariableVec constants = a->getVariables(CONSTANT);
	Graph::VariableVec userexp = a->getVariables(USER_EXP);

	// StateVariables Vector sortieren:
	std::sort(states.begin(),states.end(), sortVariableVec);
    // Inputs Vector sortieren: 
	std::sort(inputs.begin(),inputs.end(), sortVariableVec);

	std::ofstream f;
    std::string filename= m_path + "/" + m_name + "_der_state.f90";
    f.open(filename.c_str());

	f << "! " << getHeaderLine() << std::endl;
	f << std::endl;
	
	f << "subroutine "<< m_name <<"_der_state(time, y, yd"; 
	for (Graph::VariableVec::iterator it=inputs.begin();it!=inputs.end();++it)
		f << ", " << m_p->print(*it); 
	f << ")" << std::endl; 

	f << "use functionmodule" << std::endl;
	f << "implicit none" << std::endl;
	f << std::endl;

	f << "!declare fixed variables" << std::endl;
	int input_dim=0; 
	for (Graph::VariableVec::iterator it=states.begin();it != states.end();++it)
	{
		f << "    double precision"<< m_p->dimension(*it) << " :: " << m_p->print(*it) << m_p->comment2(g,*it) << std::endl;
		f << "    double precision"<< m_p->dimension(*it) << " :: der_" << m_p->print(*it) << std::endl;
		input_dim += (*it)->getShape().getDimension(1);
	}
    f << "    double precision, intent(in) :: time" << std::endl;
	f << "    double precision, dimension(" << input_dim << ",1), intent(in) :: y" << std::endl;
	f << "    double precision, dimension(" << input_dim << ",1), intent(out) :: yd" << std::endl;
	f << std::endl;

	f << "!declare inputs" << std::endl;
	for (Graph::VariableVec::iterator it=inputs.begin();it!=inputs.end();++it)
		f << "    double precision, intent(in)" << m_p->dimension(*it) << " :: " << m_p->print(*it) << m_p->comment2(g,*it) <<  std::endl;
	f << std::endl;
    
	f << "!Parameters" << std::endl;
	for (Graph::VariableVec::iterator it=parameter.begin();it!=parameter.end();++it)
		f << "    double precision, parameter" << m_p->dimension(*it) << " :: " << m_p->print(*it) << "=" << m_p->print(g.getEquation(*it)) << m_p->comment2(g,*it) <<  std::endl;
	f << std::endl;

	f << "!Constants" << std::endl;
	for (Graph::VariableVec::iterator it=constants.begin();it!=constants.end();++it)
        f << "    double precision, constant" << m_p->dimension(*it) << " :: " << m_p->print(*it) << " = " << m_p->print(g.getEquation(*it)) << m_p->comment2(g,*it) << std::endl;
	f << std::endl;

	f << "!User Expression variables" << std::endl;
	for (Graph::VariableVec::iterator it=userexp.begin();it!=userexp.end();++it)
        f << "    double precision" << m_p->dimension(*it) << " :: " << m_p->print(*it) << m_p->comment2(g,*it) << std::endl;
	f << std::endl;

	f << "!declare variables" << std::endl;
    for (Graph::VariableVec::iterator it=variables.begin();it!=variables.end();++it)
        f << "    double precision" << m_p->dimension(*it) << " :: " << m_p->print(*it) << m_p->comment2(g,*it) <<  std::endl;
	f << std::endl;

	f << "!Additional temporary variables (code export specific)" << std::endl;

	// Das Folgende muss erstmal in einem Stringstream zwischengespeichert werden, da die tempor�ren Variablen erst nach
	// schreiben der Equations bekannt sind. Es ist in C++ nicht m�glich nachtr�glich etwas in einen Stream einzuf�gen
	std::stringstream fss;

	fss << "!define states" << std::endl;
	Shape n = states.at(0)->getShape(); 
	for (Graph::VariableVec::size_type i = 0; i < states.size() ; ++i) 
	{
		fss << "    " << m_p->print(states[i]) << " = y(";
		if (states[i]->is_Scalar())
			fss << str(n.getDimension(1) * i + 1) << ",1)" << std::endl;
		else
			fss << str(n.getDimension(1) * i + 1) << ":" 
			  << str(n.getDimension(1) * (i+1)) << ",:)" 
			  << std::endl;
	}
	fss << std::endl;
	// fss << "write (*,*) \"y:=\", y" << std::endl; //for debug

	fss << "!calculate state derivative" << std::endl;
	std::vector<std::string> additionalVarDefs;
	fss << writeEquations(a->getEquations(PARAMETER | CONSTANT | INPUT ), additionalVarDefs) << std::endl;
    fss << std::endl;

	// Jetzt sind die tempor�ren Variablen bekannt
	for (std::vector<std::string>::iterator it = additionalVarDefs.begin(); it != additionalVarDefs.end(); ++it)
		f << "    " << *it << std::endl;
	f << std::endl;

	f << fss.rdbuf();

	f << "    yd = reshape((/";
	for (Graph::VariableVec::iterator it=states.begin();it!=states.end();++it)
		f << "der_"<< m_p->print(*it) << (it+1 != states.end() ? ", " : "" );
	f << "/), (/" << input_dim << ",1/))" << std::endl;
	f << std::endl;

	f << "end subroutine" << std::endl;

	f.close();

    if (m_p->getErrorcount())
        std::cerr << "There have been " << m_p->getErrorcount(true) << " error(s) during generation of " 
                  << m_name << "_der_state.f90. Please have a look at that file to see where the error(s) occured." << std::endl;

	return Util::getTime() - t1;
}
/*****************************************************************************/

/*****************************************************************************/
double FortranWriter::generateSensors(Graph::Graph& g)
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
		std::cerr << "Warning: FortranWriter: Not generating sensors file, since there are no sensors." << std::endl;
		return Util::getTime() - t1;
	}

	// StateVariables Vector sortieren:
	std::sort(states.begin(),states.end(), sortVariableVec);
    // Inputs Vector sortieren: 
	std::sort(inputs.begin(),inputs.end(), sortVariableVec);
	// Sensors Vector sortieren: 
	std::sort(sensors.begin(),sensors.end(), sortVariableVec);

	std::ofstream f;
    std::string filename= m_path + "/" + m_name + "_sensors.f90";
    f.open(filename.c_str());

	f << "! " << getHeaderLine() << std::endl;
	f << std::endl;
	
	f << "subroutine "<< m_name <<"_sensors(time, y"; 
	for (Graph::VariableVec::iterator it=inputs.begin();it!=inputs.end();++it)
		f << ", " << m_p->print(*it);
	for (Graph::VariableVec::iterator it=sensors.begin();it!=sensors.end();++it)
		if (it==sensors.begin())
			f << ", &" << std::endl <<"                " << m_p->print(*it);
		else
			f << ", " << m_p->print(*it); 
	f << ")" << std::endl; 

	f << "use functionmodule" << std::endl;
	f << "implicit none" << std::endl;
	f << std::endl;

	f << "!declare arguments and return variables" << std::endl;
    f << "    double precision, intent(in) :: time" << std::endl;
	int input_dim=0; 
	for (Graph::VariableVec::iterator it=states.begin();it != states.end();++it)
	{
		f << "    double precision"<< m_p->dimension(*it) << " :: " << m_p->print(*it) << m_p->comment2(g,*it) << std::endl;
		input_dim += (*it)->getShape().getDimension(1);
	}
	f << "    double precision, dimension(" << input_dim << ",1), intent(in) :: y" << std::endl;
	for (Graph::VariableVec::iterator it=sensors.begin();it!=sensors.end();++it)
		f << "    double precision"<< m_p->dimension(*it) << ", intent(out) :: " << m_p->print(*it) << m_p->comment2(g,*it) << std::endl;
	f << std::endl;

	f << "!declare inputs" << std::endl;
	for (Graph::VariableVec::iterator it=inputs.begin();it!=inputs.end();++it)
		f << "    double precision, intent(in)" << m_p->dimension(*it) << " :: " << m_p->print(*it) << m_p->comment2(g,*it) <<  std::endl;
	f << std::endl;
    
	f << "!Parameters" << std::endl;
	for (Graph::VariableVec::iterator it=parameter.begin();it!=parameter.end();++it)
		f << "    double precision, parameter"          << " :: " << m_p->print(*it) << "=" << m_p->print(g.getEquation(*it)) << m_p->comment2(g,*it) <<  std::endl;
	f << std::endl;

	f << "!Constants" << std::endl;
	for (Graph::VariableVec::iterator it=constants.begin();it!=constants.end();++it)
        f << "    double precision, constant" << m_p->dimension(*it) << " :: " << m_p->print(*it) << " = " << m_p->print(g.getEquation(*it)) << m_p->comment2(g,*it) << std::endl;
	f << std::endl;

	f << "!User Expression variables" << std::endl;
	for (Graph::VariableVec::iterator it=userexp.begin();it!=userexp.end();++it)
        f << "    double precision" << m_p->dimension(*it) << " :: " << m_p->print(*it) << m_p->comment2(g,*it) << std::endl;
	f << std::endl;

	f << "!declare variables" << std::endl;
    for (Graph::VariableVec::iterator it=variables.begin();it!=variables.end();++it)
        f << "    double precision" << m_p->dimension(*it) << " :: " << m_p->print(*it) << m_p->comment2(g,*it) <<  std::endl;
	f << std::endl;
	
	f << "!Additional temporary variables (code export specific)" << std::endl;

	// Das Folgende muss erstmal in einem Stringstream zwischengespeichert werden, da die tempor�ren Variablen erst nach
	// schreiben der Equations bekannt sind. Es ist in C++ nicht m�glich nachtr�glich etwas in einen Stream einzuf�gen
	std::stringstream fss;

	fss << "!define states" << std::endl; 
	for (Graph::VariableVec::size_type i = 0; i < states.size() ; ++i) 
	{
        Shape n = states.at(i)->getShape();
		fss << "    " << m_p->print(states[i]) << " = y(";
		if (states[i]->is_Scalar())
			fss << str(n.getDimension(1) * i + 1) << ",1)" << std::endl;
		else
			fss << str(n.getDimension(1) * i + 1) << ":" 
			  << str(n.getDimension(1) * (i+1)) << ",:)" 
			  << std::endl;
	}
	fss << std::endl;
	// fss << "write (*,*) \"y:=\", y" << std::endl; //for debug

	fss << "!calculate sensors" << std::endl;
	std::vector<std::string> additionalVarDefs;
	fss << writeEquations(a->getEquations(PARAMETER | CONSTANT | INPUT ), additionalVarDefs) << std::endl;
    fss << std::endl;

	// Jetzt sind die tempor�ren Variablen bekannt
	for (std::vector<std::string>::iterator it = additionalVarDefs.begin(); it != additionalVarDefs.end(); ++it)
		f << "    " << *it << std::endl;
	f << std::endl;

	f << fss.rdbuf();

	f << "end subroutine" << std::endl;
	f.close();

    if (m_p->getErrorcount())
        std::cerr << "There have been " << m_p->getErrorcount(true) << " error(s) during generation of " 
                  << m_name << "_sensors.f90. Please have a look at that file to see where the error(s) occured." << std::endl;

	return Util::getTime() - t1;
}
/*****************************************************************************/


/*****************************************************************************/
double FortranWriter::generateVisual(Graph::Graph& g)
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
		std::cerr << "Warning: FortranWriter: Not generating visual file, since there are no visual sensors. (Maybe there are no visualisations defined?)" << std::endl;
		return Util::getTime() - t1;
	}
	
	// StateVariables Vector sortieren:
	std::sort(states.begin(),states.end(), sortVariableVec);
	// Visual_sensors Vector sortieren: 
	std::sort(sens_vis.begin(),sens_vis.end(), sortVariableVec);

	std::ofstream f;
    std::string filename= m_path + "/" + m_name + "_visual.f90";
    f.open(filename.c_str());

	f << "! " << getHeaderLine() << std::endl;
	f << std::endl;
	
	f << "subroutine "<< m_name <<"_visual(y"; 
	for (Graph::VariableVec::iterator it=sens_vis.begin();it!=sens_vis.end();++it)
		f << ", " << m_p->print(*it); 
	f << ")" << std::endl; 

	f << "use functionmodule" << std::endl;
	f << "implicit none" << std::endl;
	f << std::endl;

	f << "!declare arguments and return variables" << std::endl;
	int input_dim=0; 
	for (Graph::VariableVec::iterator it=states.begin();it != states.end();++it)
	{
		f << "    double precision"<< m_p->dimension(*it) << " :: " << m_p->print(*it) << m_p->comment2(g,*it) << std::endl;
		input_dim += (*it)->getShape().getDimension(1);
	}
	f << "    double precision, dimension(" << input_dim << ",1), intent(in) :: y" << std::endl;
	for (Graph::VariableVec::iterator it=sens_vis.begin();it!=sens_vis.end();++it)
		f << "    double precision"<< m_p->dimension(*it) << ", intent(out) :: " << m_p->print(*it) << m_p->comment2(g,*it) << std::endl;
	f << std::endl;

    
	f << "!Parameters" << std::endl;
	for (Graph::VariableVec::iterator it=parameter.begin();it!=parameter.end();++it)
		f << "    double precision, parameter"          << " :: " << m_p->print(*it) << "=" << m_p->print(g.getEquation(*it)) << m_p->comment2(g,*it) <<  std::endl;
	f << std::endl;

	f << "!Constants" << std::endl;
	for (Graph::VariableVec::iterator it=constants.begin();it!=constants.end();++it)
        f << "    double precision, constant" << m_p->dimension(*it) << " :: " << m_p->print(*it) << " = " << m_p->print(g.getEquation(*it)) << m_p->comment2(g,*it) << std::endl;
	f << std::endl;

	f << "!User Expression variables" << std::endl;
	for (Graph::VariableVec::iterator it=userexp.begin();it!=userexp.end();++it)
        f << "    double precision" << m_p->dimension(*it) << " :: " << m_p->print(*it) << m_p->comment2(g,*it) << std::endl;
	f << std::endl;

	f << "!declare variables" << std::endl;
    for (Graph::VariableVec::iterator it=variables.begin();it!=variables.end();++it)
        f << "    double precision" << m_p->dimension(*it) << " :: " << m_p->print(*it) << m_p->comment2(g,*it) <<  std::endl;
	f << std::endl;
	
	f << "!Additional temporary variables (code export specific)" << std::endl;

	// Das Folgende muss erstmal in einem Stringstream zwischengespeichert werden, da die tempor�ren Variablen erst nach
	// schreiben der Equations bekannt sind. Es ist in C++ nicht m�glich nachtr�glich etwas in einen Stream einzuf�gen
	std::stringstream fss;

	fss << "!define states" << std::endl;
	for (Graph::VariableVec::size_type i = 0; i < states.size() ; ++i) 
	{
        Shape n = states.at(i)->getShape(); 
		fss << "    " << m_p->print(states[i]) << " = y(";
		if (states[i]->is_Scalar())
			fss << str(n.getDimension(1) * i + 1) << ",1)" << std::endl;
		else
			fss << str(n.getDimension(1) * i + 1) << ":" 
			  << str(n.getDimension(1) * (i+1)) << ",:)" 
			  << std::endl;
	}
	fss << std::endl;
	// fss << "write (*,*) \"y:=\", y" << std::endl; //for debug

	fss << "!calculate sensors" << std::endl;
	std::vector<std::string> additionalVarDefs;
	fss << writeEquations(a->getEquations(PARAMETER | CONSTANT | INPUT ), additionalVarDefs) << std::endl;
    fss << std::endl;

	// Jetzt sind die tempor�ren Variablen bekannt
	for (std::vector<std::string>::iterator it = additionalVarDefs.begin(); it != additionalVarDefs.end(); ++it)
		f << "    " << *it << std::endl;
	f << std::endl;

	f << fss.rdbuf();

	f << "end subroutine" << std::endl;
	f.close();

    if (m_p->getErrorcount())
        std::cerr << "There have been " << m_p->getErrorcount(true) << " error(s) during generation of " 
                  << m_name << "_visual.f90. Please have a look at that file to see where the error(s) occured." << std::endl;

	return Util::getTime() - t1;
}
/*****************************************************************************/

/*****************************************************************************/
std::string FortranWriter::writeEquations(std::vector<Graph::Assignment> const& equations, std::vector<std::string> &additionalVarDefs) const
/*****************************************************************************/
{
	std::stringstream s;

	for (std::vector<Graph::Assignment>::const_iterator it=equations.begin(); it!=equations.end(); ++it)
    {
        if (it->implizit)
            throw InternalError("Implicit equations are not yet implemented in Fortran!");
		//Folgendes falls mehrere Gleichungen in einer verpackt sind (wird aber scheinbar kaum genutzt)
		for (size_t i=0; i < it->lhs.size(); ++i)
        {
            BasicPtr simple_exp = it->rhs[i]->simplify();
            if (simple_exp.get() == NULL) throw InternalError("FortranWriter: Value of Rhs is not Valid!");
            
			//Workaround: links Matrix (also mehrere Ergebnisse)
			if (it->lhs[i]->getType() == Type_Matrix)
			{
				const Matrix *mat = Util::getAsConstPtr<Matrix>(it->lhs[i]);
				if (simple_exp->getType() == Type_Symbol)
				{  // Wir k�nnen es ohne tempor�re Variable aufl�sen
					for (size_t j=0; j < mat->getShape().getDimension(1); j++)
						for (size_t k=0; k < mat->getShape().getDimension(2); k++)
							s << "    " << m_p->print(mat->get(j,k)) << " = " << m_p->print(simple_exp) << "(" << j+1<< "," << k+1 << ")" << std::endl;
				} else 	{  // Es muss eine tempor�re Variable eingef�hrt werden
					additionalVarDefs.push_back("double precision" + m_p->dimension(it->lhs[i]) + " :: temp_var_" + str(additionalVarDefs.size() + 1));
					s << "    temp_var_" << additionalVarDefs.size() << " = " << m_p->print(simple_exp) << std::endl; 
					for (size_t j=0; j < mat->getShape().getDimension(1); j++)
						for (size_t k=0; k < mat->getShape().getDimension(2); k++)
							s << "    " << m_p->print(mat->get(j,k)) << " = temp_var_1(" << j+1<< "," << k+1 << ")" << std::endl;
				}
			} else {
				// ordinary equation
				s << "    " << m_p->print(it->lhs[i]) << " = " << m_p->print(simple_exp) << std::endl;
				//s << "write (*,*) \"" << m_p->print(it->lhs[i]) << ": \", " << m_p->print(it->lhs[i]) << std::endl; //for Debug
			}
        }
    }

	return s.str();
}
/*****************************************************************************/

/*****************************************************************************/
double FortranWriter::generatePymbsWrapper(Graph::Graph& g)
/*****************************************************************************/
{
	// This used to simulate the compiled model in the PyMbs gui

	//get timestamp and catch variables from graph
	double t1 = Util::getTime();

	PythonPrinter p; //hier ist ein anderer Printer nötig

	//Wir brauchen einen lowercase Namen (f2py erzeugt das so)
	std::string m_name_lower = m_name;
	std::transform(m_name_lower.begin(), m_name_lower.end(), m_name_lower.begin(), ::tolower);
	
	Graph::VariableVec der_state_inputs = g.getAssignments(DER_STATE)->getVariables(INPUT);
    // Inputs Vector sortieren: 
	std::sort(der_state_inputs.begin(),der_state_inputs.end(), sortVariableVec);

	std::ofstream f;
    std::string filename= m_path + "/" + m_name + "_der_state_F90Wrapper.py";
    f.open(filename.c_str());

	f << "# " << getHeaderLine() << std::endl;
	f << std::endl;
	f << "from scipy import matrix, zeros" << std::endl;
	f << "from " << m_name << "_der_state_compiledF90 import " << m_name_lower << "_der_state" << std::endl;
	f << std::endl;
    f << "# inputs" << std::endl;
	f << "_inputs = {" << std::endl;
	for (Graph::VariableVec::iterator it=der_state_inputs.begin();it!=der_state_inputs.end();++it)
		f << "           '" << p.print(*it) << "' : " << p.print(g.getinitVal(*it)) 
        << ",   " << p.comment2(g,*it) << std::endl;
	f << "          }" << std::endl;        
	f << std::endl;
	f << "def setInputs(ext_inputs):" << std::endl;
	f << "    global _inputs" << std::endl;
	f << "    _inputs.update(ext_inputs)" << std::endl;
	f << std::endl;
	f << "def ode_int(t, y):" << std::endl;
	f << "    global _inputs" << std::endl;
	f << std::endl;
	f << "    return " << m_name_lower << "_der_state(t, y";
    for (Graph::VariableVec::iterator it=der_state_inputs.begin();it!=der_state_inputs.end();++it)
        f << ", _inputs['" << p.print(*it) << "']";
    f << ")" << std::endl;

	f.close();

    if (p.getErrorcount())
        std::cerr << "There have been " << p.getErrorcount(true) << " error(s) during generation of " 
                  << m_name << "_der_state_F90Wrapper.py. Please have a look at that file to see where the error(s) occured." << std::endl;


	Graph::VariableVec sens_vis = g.getAssignments(SENSOR_VISUAL)->getVariables(SENSOR_VISUAL);
	// Visual_sensors Vector sortieren:
	std::sort(sens_vis.begin(),sens_vis.end(), sortVariableVec);

    filename= m_path + "/" + m_name + "_visual_F90Wrapper.py";
    f.open(filename.c_str());

	f << "# " << getHeaderLine() << std::endl;
	f << std::endl;
	f << "from scipy import matrix, zeros" << std::endl;
	f << "from " << m_name << "_visual_compiledF90 import " << m_name_lower << "_visual" << std::endl;

	f << "def graphVisualSensors(q):" << std::endl;
	f << "    ";
	for (Graph::VariableVec::iterator it=sens_vis.begin();it!=sens_vis.end();++it)
		f  << p.print(*it) << (it+1 != sens_vis.end() ? ", " : "" ); 
	f << " = " << m_name_lower << "_visual(q)" << std::endl;
	f << std::endl;

    f << "    res_dict = {}" << std::endl;
	for (Graph::VariableVec::iterator it=sens_vis.begin();it!=sens_vis.end();++it)
		f  << "    res_dict.update({'" << p.print(*it) << "' : " << p.print(*it) << "})" << std::endl; 
	f << std::endl;

    f << "    return res_dict" << std::endl;

	f.close();

    if (p.getErrorcount())
        std::cerr << "There have been " << p.getErrorcount(true) << " error(s) during generation of " 
                  << m_name << "_visual_F90Wrapper.py. Please have a look at that file to see where the error(s) occured." << std::endl;


    Graph::VariableVec sensor = g.getAssignments(SENSOR)->getVariables(SENSOR);
    if (sensor.empty()) {
        std::cerr << "Warning: FortranWriter: Not generating sensors wrapper file, since there are no sensors." << std::endl;
        return Util::getTime() - t1;
    }

    Graph::VariableVec sensor_inputs = g.getAssignments(SENSOR)->getVariables(INPUT);
    // Inputs Vector sortieren: 
	std::sort(sensor_inputs.begin(),sensor_inputs.end(), sortVariableVec);

	// Sensors Vector sortieren:
	std::sort(sensor.begin(),sensor.end(), sortVariableVec);

    filename= m_path + "/" + m_name + "_sensors_F90Wrapper.py";
    f.open(filename.c_str());

	f << "# " << getHeaderLine() << std::endl;
	f << std::endl;
	f << "from scipy import matrix, zeros" << std::endl;
	f << "from " << m_name << "_sensors_compiledF90 import " << m_name_lower << "_sensors" << std::endl;
    f << std::endl;
    f << "# inputs" << std::endl;
	f << "_inputs = {" << std::endl;
	for (Graph::VariableVec::iterator it=sensor_inputs.begin();it!=sensor_inputs.end();++it)
		f << "           '" << p.print(*it) << "' : " << p.print(g.getinitVal(*it)) 
        << ",   " << p.comment2(g,*it) << std::endl;
	f << "          }" << std::endl;        
	f << std::endl;
	f << "def setInputs(ext_inputs):" << std::endl;
	f << "    global _inputs" << std::endl;
	f << "    _inputs.update(ext_inputs)" << std::endl;
	f << std::endl;
	f << "def graphSensors(t, q):" << std::endl;
	f << "    global _inputs" << std::endl;
	f << std::endl;
    f << "    ";
	for (Graph::VariableVec::iterator it=sensor.begin();it!=sensor.end();++it)
		f  << p.print(*it) << (it+1 != sensor.end() ? ", " : "" ); 
	f << " = " << m_name_lower << "_sensors(t, q";
    for (Graph::VariableVec::iterator it=sensor_inputs.begin();it!=sensor_inputs.end();++it)
        f << ", _inputs['" << p.print(*it) << "']";
    f << ")" << std::endl;
    f << "    res_dict = {}" << std::endl;
	for (Graph::VariableVec::iterator it=sensor.begin();it!=sensor.end();++it)
		f  << "    res_dict.update({'" << p.print(*it) << "' : " << p.print(*it) << "})" << std::endl; 
	f << std::endl;

    f << "    return res_dict" << std::endl;

	f.close();

    if (p.getErrorcount())
        std::cerr << "There have been " << p.getErrorcount(true) << " error(s) during generation of " 
                  << m_name << "_sensor_F90Wrapper.py. Please have a look at that file to see where the error(s) occured." << std::endl;
	return Util::getTime() - t1;
}
/*****************************************************************************/


/*****************************************************************************/
double FortranWriter::generateFunctionmodule()
/*****************************************************************************/
{
	//get timestamp and catch variables from graph
	double t1 = Util::getTime();

	std::ofstream f;
    std::string filename= m_path + "/functionmodule.f90";
    f.open(filename.c_str());

	f << "! " << getHeaderLine() << std::endl;
	f << std::endl;

	f << "! -*- f90 -*-" << std::endl;
	f << "module functionmodule" << std::endl;
	f << std::endl;
	f << "  implicit none" << std::endl;
	f << "  contains" << std::endl;
	f << std::endl;
	f << "  function scalar(x)" << std::endl;
	f << "    double precision  :: scalar" << std::endl;
	f << "    double precision, intent(in), dimension(1,1) :: x" << std::endl;
	f << "    scalar = x(1,1)" << std::endl;
	f << "  end function" << std::endl;
	f << std::endl;
    f << "  function skew(v)" << std::endl;
	f << "    double precision, dimension(3,3)  :: skew" << std::endl;
	f << "    double precision, intent(in), dimension(3,1) :: v" << std::endl;
	f << "    skew = reshape((/   0d0 ,  v(3,1), -v(2,1),  &" << std::endl;
	f << "                     -v(3,1),    0d0 ,  v(1,1),  &" << std::endl;
	f << "                      v(2,1), -v(1,1),    0d0 /) ,(/3,3/))" << std::endl;
	f << "  end function" << std::endl;
	f << std::endl;
	f << "  function matpow(BB, e)" << std::endl;
	f << "  !" << std::endl;
	f << "  ! Function to get a matrix BB to the power of e" << std::endl;
	f << "  !" << std::endl;
	f << "    double precision, intent(in), dimension(:,:) :: BB" << std::endl;
	f << "    integer, intent(in) :: e" << std::endl;
	f << "    double precision, dimension(size(BB,1),size(BB,2)) :: matpow" << std::endl;
	f << "    double precision, dimension(size(BB,1),size(BB,2)) :: B" << std::endl;
	f << "    integer :: i" << std::endl;
	f << "    " << std::endl;
	f << "    if (e == 0) then" << std::endl;
	f << "        matpow = 0" << std::endl;
	f << "        do i=1, size(B,1)" << std::endl;
	f << "            matpow(i,i)=1" << std::endl;
	f << "        end do" << std::endl;
	f << "        return" << std::endl;
	f << "    end if" << std::endl;
	f << "    " << std::endl;
	f << "    if (e > 0) then" << std::endl;
	f << "        B=BB" << std::endl;
	f << "    else" << std::endl;
	f << "        B=MIGS(BB)" << std::endl;
	f << "    end if" << std::endl;
	f << "    matpow = B" << std::endl;
	f << std::endl;
	f << "    do i=2, e" << std::endl;
	f << "        matpow = matmul(matpow, B)" << std::endl;
	f << "    end do" << std::endl;
	f << std::endl;
	f << "    return    " << std::endl;
	f << "  end function" << std::endl;
	f << std::endl;
	f << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
	f << "!                                                             !" << std::endl;
	f << "! The following subroutines are written by Tao Pang in        !" << std::endl;
	f << "! conjunction with his book, \"An Introduction to Computational!" << std::endl;
	f << "! Physics,\" published by Cambridge University Press in 1997.  !" << std::endl;
	f << "!                                                             !" << std::endl;
	f << "! The LEGS and MIGS Subroutines have been modified to a       !" << std::endl;
	f << "! function, to double precision and to use \"Matrix-Vectors\"   !" << std::endl;
	f << "! by Sebastian Wienforth                                      !" << std::endl;
	f << "!                                                             !" << std::endl;
	f << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
	f << std::endl;
	f << "FUNCTION LEGS (AA,BB)" << std::endl;
	f << "!" << std::endl;
	f << "! Subroutine to solve the equation AA*LEGS = BB with the" << std::endl;
	f << "! partial-pivoting Gaussian elimination scheme." << std::endl;
	f << "! Copyright (c) Tao Pang 2001." << std::endl;
	f << "!" << std::endl;
	f << "  IMPLICIT NONE" << std::endl;
	f << "  double precision, INTENT (IN), DIMENSION (:,:) :: AA" << std::endl;
	f << "  double precision, DIMENSION (size(AA,1),size(AA,2)) :: A" << std::endl;
	f << "  double precision, INTENT (IN), DIMENSION (:,:) :: BB" << std::endl;
	f << "  double precision, DIMENSION (size(BB,1),size(BB,2)) :: B" << std::endl;
	f << "  double precision, DIMENSION (size(BB,1),size(BB,2)) :: LEGS" << std::endl;
	f << "  INTEGER :: I,J,N" << std::endl;
	f << "  INTEGER, DIMENSION (size(AA,1)) :: INDX" << std::endl;
	f << "!" << std::endl;
	f << "  INDX=0" << std::endl;
	f << "  N=size(AA,1)" << std::endl;
	f << "  A=AA" << std::endl;
	f << "  B=BB" << std::endl;
	f << "!" << std::endl;
	f << "  CALL ELGS (A,N,INDX)" << std::endl;
	f << "!" << std::endl;
	f << "  DO I = 1, N-1" << std::endl;
	f << "    DO J = I+1, N" << std::endl;
	f << "      B(INDX(J),1) = B(INDX(J),1)-A(INDX(J),I)*B(INDX(I),1)" << std::endl;
	f << "    END DO" << std::endl;
	f << "  END DO" << std::endl;
	f << "!" << std::endl;
	f << "  LEGS(N,1) = B(INDX(N),1)/A(INDX(N),N)" << std::endl;
	f << "  DO I = N-1, 1, -1" << std::endl;
	f << "    LEGS(I,1) = B(INDX(I),1)" << std::endl;
	f << "    DO J = I+1, N" << std::endl;
	f << "      LEGS(I,1) = LEGS(I,1)-A(INDX(I),J)*LEGS(J,1)" << std::endl;
	f << "    END DO" << std::endl;
	f << "    LEGS(I,1) =  LEGS(I,1)/A(INDX(I),I)" << std::endl;
	f << "  END DO" << std::endl;
	f << "!" << std::endl;
	f << "END FUNCTION LEGS" << std::endl;
	f << "!" << std::endl;
	f << "FUNCTION MIGS (AA)" << std::endl;
	f << "!" << std::endl;
	f << "! Subroutine to invert matrix AA with the inverse stored" << std::endl;
	f << "! in MIGS in the output.  Copyright (c) Tao Pang 2001." << std::endl;
	f << "!" << std::endl;
	f << "  IMPLICIT NONE" << std::endl;
	f << "  double precision, INTENT (IN), DIMENSION (:,:):: AA" << std::endl;
	f << "  double precision, DIMENSION (size(AA,1),size(AA,2)):: A" << std::endl;
	f << "  double precision, DIMENSION (size(AA,1),size(AA,2)):: MIGS" << std::endl;
	f << "  double precision, DIMENSION (size(AA,1),size(AA,2)) :: B" << std::endl;
	f << "  INTEGER :: I,J,K,N" << std::endl;
	f << "  INTEGER, DIMENSION (size(AA,1)) :: INDX" << std::endl;
	f << "!" << std::endl;
	f << "  INDX=0" << std::endl;
	f << "  N=size(AA,1)" << std::endl;
	f << "  A=AA" << std::endl;
	f << "!" << std::endl;
	f << "  DO I = 1, N" << std::endl;
	f << "    DO J = 1, N" << std::endl;
	f << "      B(I,J) = 0.0" << std::endl;
	f << "    END DO" << std::endl;
	f << "  END DO" << std::endl;
	f << "  DO I = 1, N" << std::endl;
	f << "    B(I,I) = 1.0" << std::endl;
	f << "  END DO" << std::endl;
	f << "!" << std::endl;
	f << "  CALL ELGS (A,N,INDX)" << std::endl;
	f << "!" << std::endl;
	f << "  DO I = 1, N-1" << std::endl;
	f << "    DO J = I+1, N" << std::endl;
	f << "      DO K = 1, N" << std::endl;
	f << "        B(INDX(J),K) = B(INDX(J),K)-A(INDX(J),I)*B(INDX(I),K)" << std::endl;
	f << "      END DO" << std::endl;
	f << "    END DO" << std::endl;
	f << "  END DO" << std::endl;
	f << "!" << std::endl;
	f << "  DO I = 1, N" << std::endl;
	f << "    MIGS(N,I) = B(INDX(N),I)/A(INDX(N),N)" << std::endl;
	f << "    DO J = N-1, 1, -1" << std::endl;
	f << "      MIGS(J,I) = B(INDX(J),I)" << std::endl;
	f << "      DO K = J+1, N" << std::endl;
	f << "        MIGS(J,I) = MIGS(J,I)-A(INDX(J),K)*MIGS(K,I)" << std::endl;
	f << "      END DO" << std::endl;
	f << "      MIGS(J,I) =  MIGS(J,I)/A(INDX(J),J)" << std::endl;
	f << "    END DO" << std::endl;
	f << "  END DO" << std::endl;
	f << "END FUNCTION MIGS" << std::endl;
	f << "!" << std::endl;
	f << "SUBROUTINE ELGS (A,N,INDX)" << std::endl;
	f << "!" << std::endl;
	f << "! Subroutine to perform the partial-pivoting Gaussian elimination." << std::endl;
	f << "! A(N,N) is the original matrix in the input and transformed matrix" << std::endl;
	f << "! plus the pivoting element ratios below the diagonal in the output." << std::endl;
	f << "! INDX(N) records the pivoting order.  Copyright (c) Tao Pang 2001." << std::endl;
	f << "!" << std::endl;
	f << "  IMPLICIT NONE" << std::endl;
	f << "  INTEGER, INTENT (IN) :: N" << std::endl;
	f << "  INTEGER :: I,J,K,ITMP" << std::endl;
	f << "  INTEGER, INTENT (OUT), DIMENSION (N) :: INDX" << std::endl;
	f << "  double precision :: C1,PI,PI1,PJ" << std::endl;
	f << "  double precision, INTENT (INOUT), DIMENSION (N,N) :: A" << std::endl;
	f << "  double precision, DIMENSION (N) :: C" << std::endl;
	f << "!" << std::endl;
	f << "! Initialize the index" << std::endl;
	f << "!" << std::endl;
	f << "  DO I = 1, N" << std::endl;
	f << "    INDX(I) = I" << std::endl;
	f << "  END DO" << std::endl;
	f << "!" << std::endl;
	f << "! Find the rescaling factors, one from each row" << std::endl;
	f << "!" << std::endl;
	f << "  DO I = 1, N" << std::endl;
	f << "    C1= 0.0" << std::endl;
	f << "    DO J = 1, N" << std::endl;
	f << "      C1 = AMAX1(C1,ABS(A(I,J)))" << std::endl;
	f << "    END DO" << std::endl;
	f << "    C(I) = C1" << std::endl;
	f << "  END DO" << std::endl;
	f << "!" << std::endl;
	f << "! Search the pivoting (largest) element from each column" << std::endl;
	f << "!" << std::endl;
	f << "  DO J = 1, N-1" << std::endl;
	f << "    PI1 = 0.0" << std::endl;
	f << "    DO I = J, N" << std::endl;
	f << "      PI = ABS(A(INDX(I),J))/C(INDX(I))" << std::endl;
	f << "      IF (PI.GT.PI1) THEN" << std::endl;
	f << "        PI1 = PI" << std::endl;
	f << "        K   = I" << std::endl;
	f << "      ENDIF" << std::endl;
	f << "    END DO" << std::endl;
	f << "!" << std::endl;
	f << "! Interchange the rows via INDX(N) to record pivoting order" << std::endl;
	f << "!" << std::endl;
	f << "    ITMP    = INDX(J)" << std::endl;
	f << "    INDX(J) = INDX(K)" << std::endl;
	f << "    INDX(K) = ITMP" << std::endl;
	f << "    DO I = J+1, N" << std::endl;
	f << "      PJ  = A(INDX(I),J)/A(INDX(J),J)" << std::endl;
	f << "!" << std::endl;
	f << "! Record pivoting ratios below the diagonal" << std::endl;
	f << "!" << std::endl;
	f << "      A(INDX(I),J) = PJ" << std::endl;
	f << "!" << std::endl;
	f << "! Modify other elements accordingly" << std::endl;
	f << "!" << std::endl;
	f << "      DO K = J+1, N" << std::endl;
	f << "        A(INDX(I),K) = A(INDX(I),K)-PJ*A(INDX(J),K)" << std::endl;
	f << "      END DO" << std::endl;
	f << "    END DO" << std::endl;
	f << "  END DO" << std::endl;
	f << "!" << std::endl;
	f << "END SUBROUTINE ELGS" << std::endl;
	f << "end module" << std::endl;
	f << std::endl;

	f.close();
	return Util::getTime() - t1;
}
/*****************************************************************************/