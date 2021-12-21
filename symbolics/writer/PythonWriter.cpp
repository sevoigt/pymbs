#include "PythonWriter.h"
#include "str.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>

using namespace Symbolics;

std::vector<std::string> split2(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> Symbolics::split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split2(s, delim, elems);
    return elems;
}

/*****************************************************************************/
PythonWriter::PythonWriter( std::map<std::string, std::string> &kwds ):Writer(false)
/*****************************************************************************/
{
    m_p = new PythonPrinter();
}
/*****************************************************************************/

/*****************************************************************************/
PythonWriter::PythonWriter():Writer(false)
/*****************************************************************************/
{
    m_p = new PythonPrinter();
}
/*****************************************************************************/

/*****************************************************************************/
PythonWriter::~PythonWriter()
/*****************************************************************************/
{
    delete m_p;
}
/*****************************************************************************/


/*****************************************************************************/
double PythonWriter::generateTarget_Impl(Graph::Graph &g)
/*****************************************************************************/
{
	double t1 = generateStateDerivative(g);
	double t2 = generateVisualSensors(g);
	double t3 = generateSensors(g);
	return t1+t2+t3;
}

/*****************************************************************************/
double PythonWriter::generateStateDerivative(Graph::Graph &g)
/*****************************************************************************/
{
	//get timestamp and catch variables from graph
	double t1 = Util::getTime();

	Graph::AssignmentsPtr a = g.getAssignments(DER_STATE);

	Graph::VariableVec states = a->getVariables(STATE);
	if (states.empty())
		throw InternalError("Error: Models without states are not implemented yet in Python!");
	Graph::VariableVec inputs = a->getVariables(INPUT);
	Graph::VariableVec parameter = a->getVariables(PARAMETER);
	Graph::VariableVec constants = a->getVariables(CONSTANT);
	Graph::VariableVec controller = a->getVariables(CONTROLLER);

	std::vector<Graph::Assignment> equations = a->getEquations(PARAMETER | CONSTANT | INPUT | CONTROLLER);

	Graph::AssignmentsPtr b = g.getAssignments(Writer::SENSOR);
	Graph::VariableVec sensors = b->getVariables(Writer::SENSOR);

	// StateVariables Vector sortieren:
	std::sort(states.begin(),states.end(), sortVariableVec);

	// open file
	std::ofstream f;
	std::string filename= m_path + "/" + m_name + "_der_state.py";
	f.open(filename.c_str());


	// create header line and import statements
	f << "# " << getHeaderLine() << std::endl;
	f << std::endl << std::endl;

	f << "from numpy import linalg, vstack, sin, cos, pi, sqrt, zeros, bmat, matrix" << std::endl;
	f << "from numpy import arctan2 as atan2" << std::endl;
	f << "from numpy import arctan as atan" << std::endl;
	f << "from numpy import arcsin as asin" << std::endl;
	f << "from numpy import arccos as acos" << std::endl;
	f << "from numpy import sign" << std::endl;

    // import sensors function if there are any sensors
    if (!sensors.empty())
	{
        f << "from " << m_name << "_sensors import " << m_name << "_sensors" << std::endl;
	}
	for (Graph::VariableVec::iterator it=controller.begin();it!=controller.end();++it)
	{
        std::string comment = m_p->comment2(g,*it).substr(3);
        std::vector<std::string> comment_vector = split(comment, ':');
        f << "from " << comment_vector.front() << " import " << comment_vector.back() << std::endl;
	}
	f << std::endl << std::endl;

	if (!inputs.empty()) 
	{
        // create global inputs dictionary
		f << "# inputs" << std::endl;
		f << "_inputs = {" << std::endl;
		for (Graph::VariableVec::iterator it=inputs.begin();it!=inputs.end();++it)
			f << "           '" << m_p->print(*it) << "' : " << m_p->print(g.getinitVal(*it)) 
			  << ",   " << m_p->comment2(g,*it) << std::endl;
		f << "          }" << std::endl;        
		f << std::endl << std::endl;

        // set method for inputs
		f << "def " << m_name << "_setInputs(ext_inputs):" << std::endl;
		f << "    global _inputs" << std::endl;
		f << "    _inputs.update(ext_inputs)" << std::endl;
		f << std::endl << std::endl;
	}

	// write state derivative function
	f << "def " << m_name << "_der_state(time, y):" << std::endl;
	f << std::endl;
	// f << "    print \"y:=%s\"%y"; //for debug

	// extract state from argument
	f << "    # state" << std::endl;
	for (Graph::VariableVec::size_type i = 0; i < states.size() ; ++i) 
	{
		Shape n = states.at(i)->getShape();
		f << "    " << m_p->print(states[i]) << " = ";
		if (states[i]->is_Scalar())
			f << "y[" << str(i) << "]" << std::endl;
		else
			f << "matrix(y[" << str(n.getDimension(1) * i) << ":" 
			  << str(n.getDimension(1) * (i+1)) << "], dtype=float).T" 
			  << std::endl;
	}
	f << std::endl;

	// write inputs
	f << "    # inputs" << std::endl;
	for (Graph::VariableVec::iterator it=inputs.begin();it!=inputs.end();++it)
	{
		f << "    " << m_p->print(*it) << " = _inputs['" << m_p->print(*it) << "']" << m_p->comment2(g,*it) << std::endl;
	}
	f << std::endl;

    // write controller
    f << "    # controller" << std::endl;
    if (!sensors.empty())
    {
        f << "    sensors = " << m_name << "_sensors(time, y)" << std::endl;
    }
    else
    {
        f << "    sensors = dict()" << std::endl;
    }
	for (Graph::VariableVec::iterator it=controller.begin();it!=controller.end();++it)
	{
        std::string comment = m_p->comment2(g,*it).substr(3);
        std::vector<std::string> comment_vector = split(comment, ':');
        f << "    " << m_p->print(*it) << " = " << comment_vector.back() << "(time, y, sensors)" << std::endl;
	}
	f << std::endl;

	// write constants
	f << "    # constants" << std::endl;
	for (Graph::VariableVec::iterator it=constants.begin();it!=constants.end();++it)
	{
		f << "   " << m_p->print(*it) << " = " << m_p->print(g.getEquation(*it)) << m_p->comment2(g,*it) << std::endl;
	}
	f << std::endl;

	// write parameters
	f << "    # parameter" << std::endl;
	for (Graph::VariableVec::iterator it=parameter.begin();it!=parameter.end();++it)
	{
		f << "    " << m_p->print(*it) << " = " << m_p->print(g.getEquation(*it)) << m_p->comment2(g,*it) << std::endl;
	}
	f << std::endl;

	// write equations
	f << "    # equations" << std::endl;
	f << writeEquations(equations);
	f << std::endl;

	// write return statement
	f << "    return " << "vstack((";
	for (Graph::VariableVec::iterator it=states.begin();it!=states.end();++it)
		f << (it==states.begin() ? "" : ", " ) << "der_" << m_p->print(*it);
	f << "))" << std::endl;

	// close file
	f.close();

    if (m_p->getErrorcount())
        std::cerr << "There have been " << m_p->getErrorcount(true) << " error(s) during generation of " 
                  << m_name << "_der_state.py. Please have a look at that file to see where the error(s) occured." << std::endl;
	return Util::getTime() - t1;
}
/*****************************************************************************/

/*****************************************************************************/
double PythonWriter::generateVisualSensors(Graph::Graph &g)
/*****************************************************************************/
{
	//get timestamp and catch variables from graph
	double t1 = Util::getTime();

	// get equations of visual sensors
	Graph::AssignmentsPtr a = g.getAssignments(Writer::SENSOR_VISUAL);

	Graph::VariableVec states = a->getVariables(STATE);
	Graph::VariableVec inputs = a->getVariables(INPUT);
	Graph::VariableVec parameter = a->getVariables(PARAMETER);
	Graph::VariableVec constants = a->getVariables(CONSTANT);
	Graph::VariableVec sensvis = a->getVariables(Writer::SENSOR_VISUAL);
    if (sensvis.empty())
	{
		std::cerr << "Warning: PythonWriter: Not generating visual file, since there are no visual sensors." << std::endl;
		return Util::getTime() - t1;
	}

	std::vector<Graph::Assignment> equations =  a->getEquations(PARAMETER | CONSTANT | INPUT);

	// StateVariables Vector sortieren:
	std::sort(states.begin(),states.end(), sortVariableVec);

	// open file
	std::ofstream f;
	std::string filename = m_path + "/" + m_name + "_visual.py";
	f.open(filename.c_str());


	// create header line and import statements
	f << "# " << getHeaderLine() << std::endl;
	f << std::endl << std::endl;

	f << "from numpy import linalg, vstack, sin, cos, pi, sqrt, zeros, bmat, matrix" << std::endl;
	f << "from numpy import arctan2 as atan2" << std::endl;
	f << "from numpy import arctan as atan" << std::endl;
	f << "from numpy import arcsin as asin" << std::endl;
	f << "from numpy import arccos as acos" << std::endl;
	f << "from numpy import sign" << std::endl;
	f << std::endl << std::endl;

	if (! inputs.empty())
	{
        // create global inputs dictionary
		f << "# inputs" << std::endl;
		f << "_inputs = {" << std::endl;
		for (Graph::VariableVec::iterator it=inputs.begin();it!=inputs.end();++it)
		{
			f << "           '" << m_p->print(*it) << "' : " << m_p->print(g.getinitVal(*it)) 
			  << ",   " << m_p->comment2(g,*it) << std::endl;
		}
		f << "          }" << std::endl;        
		f << std::endl << std::endl;

		// set method for inputs
		f << "def " << m_name << "_setInputs(ext_inputs):" << std::endl;
		f << "    global _inputs" << std::endl;
		f << "    _inputs.update(ext_inputs)" << std::endl;
		f << std::endl << std::endl;
	}

	// write visual sensors function
	f << "def " << m_name << "_visual(y):" << std::endl;
	f << std::endl;

	// extract state from argument
	f << "    # state" << std::endl;
	for (Graph::VariableVec::size_type i = 0; i < states.size() ; ++i) 
	{
		Shape n = states.at(i)->getShape();
		f << "    " << m_p->print(states[i]) << " = ";
		if (states[i]->is_Scalar())
			f << "y[" << str(i) << "]" << std::endl;
		else
			f << "matrix(y[" << str(n.getDimension(1) * i) << ":" 
			  << str(n.getDimension(1) * (i+1)) << "], dtype=float).T" 
			  << std::endl;
	}
	f << std::endl;

	// write inputs
	f << "    # inputs" << std::endl;
	for (Graph::VariableVec::iterator it=inputs.begin();it!=inputs.end();++it)
	{
		f << "    " << m_p->print(*it) << " = _inputs['" << m_p->print(*it) << "']" << m_p->comment2(g,*it) << std::endl;
	}
	f << std::endl;
	
	// write constants
	f << "    # constants" << std::endl;
	for (Graph::VariableVec::iterator it=constants.begin();it!=constants.end();++it)
	{
		f << "   " << m_p->print(*it) << " = " << m_p->print(g.getEquation(*it)) << m_p->comment2(g,*it) << std::endl;
	}
	f << std::endl;
	
	// write parameters
	f << "    # parameter" << std::endl;
	for (Graph::VariableVec::iterator it=parameter.begin();it!=parameter.end();++it)
	{
		f << "    " << m_p->print(*it) << " = " << m_p->print(g.getEquation(*it)) << m_p->comment2(g,*it) << std::endl;
	}
	f << std::endl;
	
	// write equations
	f << "    # visual sensor equations" << std::endl;
	f << writeEquations(equations);
	f << std::endl;

	// return dictionary with visual sensor values
	f << "    return {" << std::endl;
	for (Graph::VariableVec::iterator it=sensvis.begin();it!=sensvis.end();++it)
	{
		f << "            '" << m_p->print(*it) << "' : " << m_p->print(*it) << "," << std::endl;
	}
	f << "           }" << std::endl;
	
	// close file
	f.close();

    if (m_p->getErrorcount())
        std::cerr << "There have been " << m_p->getErrorcount(true) << " error(s) during generation of " 
                  << m_name << "_visual.py. Please have a look at that file to see where the error(s) occured." << std::endl;
	return Util::getTime() - t1;
}
/*****************************************************************************/

/*****************************************************************************/
double PythonWriter::generateSensors(Graph::Graph &g)
/*****************************************************************************/
{
	//get timestamp and catch variables from graph
	double t1 = Util::getTime();

	// get equations of visual sensors
	Graph::AssignmentsPtr a = g.getAssignments(Writer::SENSOR);

	Graph::VariableVec states = a->getVariables(STATE);
	Graph::VariableVec inputs = a->getVariables(INPUT);
	Graph::VariableVec parameter = a->getVariables(PARAMETER);
	Graph::VariableVec constants = a->getVariables(CONSTANT);
	Graph::VariableVec sensors = a->getVariables(Writer::SENSOR);
    if (sensors.empty())
	{
		std::cerr << "Warning: PythonWriter: Not generating sensors file, since there are no sensors." << std::endl;
		return Util::getTime() - t1;
	}

	std::vector<Graph::Assignment> equations =  a->getEquations(PARAMETER | CONSTANT | INPUT);

	// StateVariables Vector sortieren:
	std::sort(states.begin(),states.end(), sortVariableVec);

	// open file
	std::ofstream f;
	std::string filename = m_path + "/" + m_name + "_sensors.py";
	f.open(filename.c_str());


	// create header line and import statements
	f << "# " << getHeaderLine() << std::endl;
	f << std::endl << std::endl;

	f << "from numpy import linalg, vstack, sin, cos, pi, sqrt, zeros, bmat, matrix" << std::endl;
	f << "from numpy import arctan2 as atan2" << std::endl;
	f << "from numpy import arctan as atan" << std::endl;
	f << "from numpy import arcsin as asin" << std::endl;
	f << "from numpy import arccos as acos" << std::endl;
	f << "from numpy import sign" << std::endl;
	f << std::endl << std::endl;

	if (! inputs.empty())
	{
        // create global inputs dictionary
		f << "# inputs" << std::endl;
		f << "_inputs = {" << std::endl;
		for (Graph::VariableVec::iterator it=inputs.begin();it!=inputs.end();++it)
		{
			f << "           '" << m_p->print(*it) << "' : " << m_p->print(g.getinitVal(*it)) 
			  << ",   " << m_p->comment2(g,*it) << std::endl;
		}
		f << "          }" << std::endl;        
		f << std::endl << std::endl;

		// set method for inputs
		f << "def " << m_name << "_setInputs(ext_inputs):" << std::endl;
		f << "    global _inputs" << std::endl;
		f << "    _inputs.update(ext_inputs)" << std::endl;
		f << std::endl << std::endl;
	}

    // write sensors function
    f << "def " << m_name << "_sensors(time, y):" << std::endl;
    f << std::endl;

	// extract state from argument
	f << "    # state" << std::endl;
	for (Graph::VariableVec::size_type i = 0; i < states.size() ; ++i) 
	{
		Shape n = states.at(i)->getShape();
		f << "    " << m_p->print(states[i]) << " = ";
		if (states[i]->is_Scalar())
			f << "y[" << str(i) << "]" << std::endl;
		else
			f << "matrix(y[" << str(n.getDimension(1) * i) << ":" 
			  << str(n.getDimension(1) * (i+1)) << "], dtype=float).T" 
			  << std::endl;
	}
	f << std::endl;

	// write inputs
	f << "    # inputs" << std::endl;
	for (Graph::VariableVec::iterator it=inputs.begin();it!=inputs.end();++it)
	{
		f << "    " << m_p->print(*it) << " = _inputs['" << m_p->print(*it) << "']" << m_p->comment2(g,*it) << std::endl;
	}
	f << std::endl;

	// write constants
	f << "    # constants" << std::endl;
	for (Graph::VariableVec::iterator it=constants.begin();it!=constants.end();++it)
	{
		f << "   " << m_p->print(*it) << " = " << m_p->print(g.getEquation(*it)) << m_p->comment2(g,*it) << std::endl;
	}
	f << std::endl;

	// write parameters
	f << "    # parameter" << std::endl;
	for (Graph::VariableVec::iterator it=parameter.begin();it!=parameter.end();++it)
	{
		f << "    " << m_p->print(*it) << " = " << m_p->print(g.getEquation(*it)) << m_p->comment2(g,*it) << std::endl;
	}
	f << std::endl;

	// write equations
	f << "    # sensor equations" << std::endl;
	f << writeEquations(equations);
	f << std::endl;

	// return dictionary with sensor values
	f << "    return {" << std::endl;

	for (Graph::VariableVec::iterator it=sensors.begin();it!=sensors.end();++it)
	{
		f << "            '" << m_p->print(*it) << "' : " << m_p->print(*it) << "," << std::endl;
	}
	f << "           }" << std::endl;

	// close file
	f.close();

    if (m_p->getErrorcount())
        std::cerr << "There have been " << m_p->getErrorcount(true) << " error(s) during generation of " 
                  << m_name << "_sensors.py. Please have a look at that file to see where the error(s) occured." << std::endl;
	return Util::getTime() - t1;
}
/*****************************************************************************/


/*****************************************************************************/
std::string PythonWriter::writeEquations(std::vector<Graph::Assignment> const& equations) const
/*****************************************************************************/
{
	std::stringstream s;

	for (std::vector<Graph::Assignment>::const_iterator it=equations.begin(); it!=equations.end(); ++it)
	{
        if (it->implizit)
            throw InternalError("Implicit equations are not yet implemented in Python!");
		
		// Multi-dimensional equation, i.e. matrix equation (seems to be hardly used)
		for (size_t i=0; i < it->lhs.size(); ++i)
		{
			BasicPtr simple_exp = it->rhs[i]->simplify();
			if (simple_exp.get() == NULL) throw InternalError("PythonWriter: Value of Rhs is not Valid!");

			// Matrix at left hand side (= multiple results)
			if (it->lhs[i]->getType() == Type_Matrix)
			{
				const Matrix *mat = Util::getAsConstPtr<Matrix>(it->lhs[i]);
				Shape n = mat->getShape();
				s << "    ";
				for (size_t j=0; j < n.getDimension(1); j++)
					for (size_t k=0; k < n.getDimension(2); k++)
						s << (j+k ? ", " : "") << m_p->print(mat->get(j,k));
				s << " = " << m_p->print(simple_exp) << ".getA1()" << std::endl;
			} else { // ordinary equation
				s << "    " << m_p->print(it->lhs[i]) << " = " << m_p->print(simple_exp) << std::endl;
				//f << "    print \"" << m_p->print(equations[i].lhs[l]) << ": %s\"%" << m_p->print(equations[i].lhs[l]) << std::endl; //for debug
			}
		}
	}
	return s.str();
}
/*****************************************************************************/
