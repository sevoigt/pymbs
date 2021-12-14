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

#include "FMUWriter.h"
#include "lib/lib_xml_writer.h"
#include "FMUPrinter.h"
#include "str.h"
#include "Element.h"
#include "Filesystem.h"
#include <iostream>
#include <map>
#include <string>
#include <algorithm>
#ifdef WIN32
  #include <time.h>
#else
  #include <sys/time.h>
#endif

using namespace Symbolics;


/*****************************************************************************/
FMUWriter::FMUWriter( std::map<std::string, std::string> &kwds ) : CWriter(kwds)
/*****************************************************************************/
{
	// kwds hier umschreiben
    m_util_folder = "";
	if (kwds.find("util_folder_path") != kwds.end())
		m_util_folder = kwds["util_folder_path"];
    m_compile=true;
    if (kwds.find("compile") != kwds.end())
		m_compile = (kwds["compile"] == "True");
	
	init();
}
/*****************************************************************************/


/*****************************************************************************/
FMUWriter::FMUWriter()
/*****************************************************************************/
{
    m_compile=true;
	init();
}
/*****************************************************************************/

/*****************************************************************************/
std::vector<std::string> FMUWriter::split_path(std::string path)
/*****************************************************************************/
{
    // split path variable at : (Unix) and ; (Windows)
    std::stringstream ss(path);
    std::string item;
    std::vector<std::string> presplit, splitted, subvec;
    while (std::getline(ss, item, ':'))
        presplit.push_back(item);

    for (int i=0; i<presplit.size(); i++)
    {
        std::stringstream ss(presplit[i]);
        while (std::getline(ss, item, ';'))
            splitted.push_back(item);
    }

    return splitted;
}
/*****************************************************************************/

/*****************************************************************************/
void FMUWriter::init()
/*****************************************************************************/
{
	delete m_p; //erst das vom CWriter definierte freigeben
	m_p = new FMUPrinter(&m_valueReferences); //Dann neu belegen
	
    // generate GUID; model name + time stamp
    time_t st = time(NULL);
    //VisualStudio complains about unsafe function - ignore it, otherwise its
    //not cross platform.
    struct tm* gmt=gmtime(&st); 
	char time_string[20];
	strftime(time_string, sizeof(time_string), "%D-%T", gmt);
    std::string prefix = "PyMbs-";
    m_guid = prefix + time_string; 

	m_numberOfStates = 0;

    // search $PATH for 7za
    std::string path = getenv("PATH");
    std::vector<std::string> paths_vec = split_path(path);
    bool found7za = false;
    std::string path7za = "";
    for (unsigned i=0; i<paths_vec.size(); i++){
        found7za = filesystem::exists(paths_vec[i] + "/7za");
        if(found7za)
            path7za = paths_vec[i] + "/7za";
    }

	if (m_util_folder.empty())
	{
		// Versuchen wir mal den Pfad selbst zu finden...
		std::string path = "";
		if (getenv("PYTHONPATH")) 
		{
			path += getenv("PYTHONPATH");
			path += ":";
		}
		if (getenv("PATH")) 
		{
			path += getenv("PATH");
			path += ":";
		}
		for (std::string::iterator it = path.begin(); it != path.end(); ++it)
			if (*it=='\\') 
				*it='/'; 

        std::vector<std::string> paths_vec = split_path(path);
        for (unsigned i=0; i<paths_vec.size(); i++)
        {
			std::string add = "pymbs/utils";
			if (*paths_vec[i].rbegin() != '/') //sollte der Normalfall sein
				add = "/" + add;
			if (filesystem::exists(paths_vec[i] + add)) 
			{
				m_util_folder = paths_vec[i] + add;
				break;
			}
		}
	}
	if (m_util_folder.empty() || ! filesystem::exists(m_util_folder))
		throw InternalError("FMUWriter: Could not find \"utils\" folder. Please specify path to this folder as option \"util_folder_path\" in genCode line");
	if ( ! (filesystem::exists(m_util_folder + "/FMI/include/fmiModelFunctions.h") && filesystem::exists(m_util_folder + "/FMI/include/fmiModelTypes.h") ) )
		throw InternalError("FMUWriter: Could not find FMI headers in " + m_util_folder + "/FMI/include/fmiModelFunctions.h folder. Please specify correct path to \"utils\" folder as option \"util_folder_path\" in genCode line and assure that required header files are in there.");

    if ( ! filesystem::exists(path7za))
		throw InternalError("FMUWriter: 7za could not be found on $PATH. Do "
                "you have 7-zip installed?");
}
/*****************************************************************************/


/*****************************************************************************/
FMUWriter::~FMUWriter()
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
double FMUWriter::generateTarget_Impl(Graph::Graph &g)
/*****************************************************************************/
{
	// Wir wollen alles in einem Unterverzeichnis erstellen
	m_base_path = m_path;
	m_path += "/" + m_name;

	//Verzeichnisstruktur anlegen
	filesystem::create_directory(m_path);
	filesystem::create_directory(m_path + "/binaries");
	#ifdef _WIN32
		filesystem::create_directory(m_path + "/binaries/win32");
	#elif __linux__
		filesystem::create_directory(m_path + "/binaries/linux32");
	#endif
	filesystem::create_directory(m_path + "/sources");

	double t1 = generateXML(g);

	m_path += "/sources"; 

	int dim;
	double t2 = generateModel(g, dim);
	double t3 = generateFunctionmodule(dim);

	//Template Dateien kopieren
	if (filesystem::is_regular(m_path + "/fmuTemplate.h"))
		filesystem::remove(m_path + "/fmuTemplate.h");
	if (filesystem::is_regular(m_path + "/fmuTemplate.c"))
		filesystem::remove(m_path + "/fmuTemplate.c");
	filesystem::copy(m_util_folder + "/FMI/template/fmuTemplate.h", m_path + "/fmuTemplate.h");
	filesystem::copy(m_util_folder + "/FMI/template/fmuTemplate.c", m_path + "/fmuTemplate.c");

	m_path = m_base_path + "/" + m_name;
    if (m_compile) {
	    //Modell kompilieren 
	    std::string cmd = "gcc -shared \"" + m_path + "/sources/" + m_name + ".c\" -I \"" + m_util_folder + "/FMI/include\" -o \"" + m_path + "/binaries/";
	    #ifdef _WIN32
		    cmd += "win32/" + m_name + ".dll\"";
	    #elif __linux__
		    cmd += "linux32/" + m_name + ".so\"";
	    #endif
	    system(cmd.c_str());
    } 
    //TODO: kompiliervorgang überprüfen, evtl. existenz vom gcc prüfen, und evtl. auch alternativ den VS Compiler nutzen

	// ZIP Datei erstellen
	std::string fmu_file = m_base_path + "/" + m_name + ".fmu";
	if (filesystem::is_regular(fmu_file))
		filesystem::remove(fmu_file);
	if (filesystem::exists(fmu_file))
		throw InternalError("FMUWriter: fmu file \"" + fmu_file + "\" already exists and is not removable");

	std::string cmd = "\"\"" + m_7_Zip_app + "\" a -tzip \"" + fmu_file + "\" \"" + m_path + "/*\"\"";
	system(cmd.c_str());


	// TODO: Ordner wieder löschen - oder auch nicht... oder konfigurierbar?

	//Wahrscheinlich sollten alle Dateien/Verzeichnisse einzeln mit remove gelöscht werden. remove_all ist schon relativ böse...
	//Alternativ das Verzeichnis im Temp ordner anlegen und wieder löschen.
    // Nutzung des Temp Ordners konfigurierbar?
	return t1 + t2 + t3;
}
/*****************************************************************************/

/*****************************************************************************/
double FMUWriter::generateXML(Graph::Graph& g)
/*****************************************************************************/
{
	//get timestamp and catch variables from graph
	double t1 = Util::getTime();

	std::ofstream f;
    std::string filename= m_path + "/modelDescription.xml"; 
    f.open(filename.c_str());

	Graph::AssignmentsPtr a = g.getAssignments(DER_STATE|SENSOR|SENSOR_VISUAL);

	Graph::VariableVec states = a->getVariables(STATE);
	//Graph::VariableVec variables = a->getVariables(VARIABLE|SENSOR|SENSOR_VISUAL);
	Graph::VariableVec inputs = a->getVariables(INPUT);
	Graph::VariableVec parameter = a->getVariables(PARAMETER); //Param&Const. zusammenfassen?
	Graph::VariableVec constants = a->getVariables(CONSTANT);
	Graph::VariableVec userexp = a->getVariables(USER_EXP);
//	Graph::VariableVec sensors = a->getVariables(SENSOR); //Da die Ausgabe gleich ist kann zusammengefasst werden:
	Graph::VariableVec sens_vis = a->getVariables(SENSOR|SENSOR_VISUAL);

	// StateVariables Vector sortieren:
	std::sort(states.begin(),states.end(), sortVariableVec);

	time_t st = time(NULL);
    struct tm* gmt=gmtime(&st); //VisualStudio complains about unsafe function - ignore it, otherwise its not cross platform.
	char time_gmt_str[21];
	strftime(time_gmt_str, sizeof(time_gmt_str), "%Y-%m-%dT%H:%M:%SZ", gmt);

	for (Graph::VariableVec::iterator it=states.begin();it != states.end();++it)
		m_numberOfStates += (*it)->getShape().getDimension(1) * (*it)->getShape().getDimension(2);

	xml::writer xw(f);
    {
        xml::element md("fmiModelDescription", xw);
        md.attr("fmiVersion", "1.0");
		md.attr("modelName", m_name);
		md.attr("modelIdentifier", m_name);
		md.attr("guid", "{" + m_guid + "}");
		//md.attr("description", "TODO");
		//md.attr("author", "TODO");
		//md.attr("version", "TODO Modellversion");
		md.attr("generationTool", "PyMbs");
		md.attr("generationDateAndTime", time_gmt_str);
		//md.attr("variableNamingConvention", "TODO (flat?)");
		md.attr("numberOfContinuousStates", m_numberOfStates);
		md.attr("numberOfEventIndicators", "0");

		xml::element mv("ModelVariables", xw);
		for (Graph::VariableVec::iterator it=states.begin();it != states.end();++it)
		{
			size_t m = (*it)->getShape().getDimension(1);
			size_t n = (*it)->getShape().getDimension(2);
			for (size_t i = 0; i < m; ++i)
				for (size_t j = 0; j < n; ++j)
				{
					std::string name = (*it)->getName() + (m>1 ? "_" + str(i) : "") + (n>1 ? "_" + str(j) : "");
					{
						xml::element sv("ScalarVariable", xw);
						sv.attr("name", name);
						sv.attr("valueReference", m_valueReferences.size());
						m_valueReferences[name] = m_valueReferences.size();
						sv.attr("description", g.getComment(*it) + (m>1 ? "[" + str(i) + "]" : "") + (n>1 ? "[" + str(j) + "]" : ""));
						//sv.attr("variability", "continuous"); // cont. is default
						//sv.attr("causality", "output"); // internal is default TODO; was muss hier eigentlich hin??
						xml::element record("Real", xw, false);
                        record.attr("start", ( m>1 || n>1 ? g.getinitVal(*it)->getArg(i*n+j)->toString() : g.getinitVal(*it)->toString()));
						record.attr("fixed", "true"); // TODO: evtl false? true ist default.
					} {
						xml::element sv("ScalarVariable", xw);
						sv.attr("name", "der_" + name); // TODO: evtl könnte man hier auch die Klammervariante wie beim comment nehmen
						sv.attr("valueReference", m_valueReferences.size());
						m_valueReferences["der_" + name] = m_valueReferences.size();
						sv.attr("description", "der(" + g.getComment(*it) + (m>1 ? "[" + str(i) + "]" : "") + (n>1 ? "[" + str(j) + "]" : "") + ")");
						//sv.attr("variability", "continuous"); // cont. is default
						//sv.attr("causality", "internal"); // internal is default
						xml::element record("Real", xw, false);
					}
				}
		}
		for (Graph::VariableVec::iterator it=inputs.begin();it!=inputs.end();++it)
		{
			size_t m = (*it)->getShape().getDimension(1);
			size_t n = (*it)->getShape().getDimension(2);
			for (size_t i = 0; i < m; ++i)
				for (size_t j = 0; j < n; ++j)
				{
					std::string name = (*it)->getName() + (m>1 ? "_" + str(i) : "") + (n>1 ? "_" + str(j) : "");
					xml::element sv("ScalarVariable", xw);
					sv.attr("name", name);
					sv.attr("valueReference", m_valueReferences.size());
					m_valueReferences[name] = m_valueReferences.size();
					sv.attr("description", g.getComment(*it) + (m>1 ? "[" + str(i) + "]" : "") + (n>1 ? "[" + str(j) + "]" : ""));
					//sv.attr("variability", "continuous"); // cont. is default TODO: maybe parameter??
					sv.attr("causality", "input");
					xml::element record("Real", xw, false);
					record.attr("start", ( m>1 || n>1 ? "0" : g.getinitVal(*it)->toString()));
					//m_p->print(g.getinitVal(*it)->getArg(i*n+j))
					//record.attr("fixed", "true"); // true ist default.
				}
		}
		for (Graph::VariableVec::iterator it=parameter.begin();it!=parameter.end();++it)
		{
			size_t m = (*it)->getShape().getDimension(1);
			size_t n = (*it)->getShape().getDimension(2);
			for (size_t i = 0; i < m; ++i) // Parameter sind wohl immer Skalar, da eine Skalarisierung schon im Python Teil durchgeführt wird??
				for (size_t j = 0; j < n; ++j)
				{
					std::string name = (*it)->getName() + (m>1 ? "_" + str(i) : "") + (n>1 ? "_" + str(j) : "");
					xml::element sv("ScalarVariable", xw);
					sv.attr("name", name);
					sv.attr("valueReference", m_valueReferences.size());
					m_valueReferences[name] = m_valueReferences.size();
					sv.attr("description", g.getComment(*it) + (m>1 ? "[" + str(i) + "]" : "") + (n>1 ? "[" + str(j) + "]" : "")
                                                + " = " +  g.getEquation(*it)->toString());
					sv.attr("variability", "parameter");
					//sv.attr("causality", "internal"); // internal is default TODO: correct?
					xml::element record("Real", xw, false);
					if (g.getEquation(*it)->getType() == Type_Real || g.getEquation(*it)->getType() == Type_Int)
                        record.attr("start", g.getEquation(*it)->toString()); 
					else
						record.attr("start","0");  // TODO: erstmal Null gesetzt
					//record.attr("fixed", "true"); // true ist default.
				}
		}
		for (Graph::VariableVec::iterator it=constants.begin();it!=constants.end();++it)
		{
			size_t m = (*it)->getShape().getDimension(1);
			size_t n = (*it)->getShape().getDimension(2);
			for (size_t i = 0; i < m; ++i)
				for (size_t j = 0; j < n; ++j)
				{
					std::string name = (*it)->getName() + (m>1 ? "_" + str(i) : "") + (n>1 ? "_" + str(j) : "");
					xml::element sv("ScalarVariable", xw);
					sv.attr("name", name);
					sv.attr("valueReference", m_valueReferences.size());
					m_valueReferences[name] = m_valueReferences.size();
					sv.attr("description", g.getComment(*it) + (m>1 ? "[" + str(i) + "]" : "") + (n>1 ? "[" + str(j) + "]" : "")
                                                + " = " +  g.getEquation(*it)->toString());
					sv.attr("variability", "parameter");
					//sv.attr("causality", "internal"); // internal is default TODO: correct?
					xml::element record("Real", xw, false);
					if (g.getEquation(*it)->getType() == Type_Real || g.getEquation(*it)->getType() == Type_Int)
						record.attr("start", g.getEquation(*it)->toString()); 
					else
						record.attr("start","0");  // TODO: erstmal Null gesetzt
					//record.attr("fixed", "true"); // true ist default.
				}
		}
		for (Graph::VariableVec::iterator it=userexp.begin();it!=userexp.end();++it)
		{
			size_t m = (*it)->getShape().getDimension(1);
			size_t n = (*it)->getShape().getDimension(2);
			for (size_t i = 0; i < m; ++i)
				for (size_t j = 0; j < n; ++j)
				{
					std::string name = (*it)->getName() + (m>1 ? "_" + str(i) : "") + (n>1 ? "_" + str(j) : "");
					xml::element sv("ScalarVariable", xw);
					sv.attr("name", name);
					sv.attr("valueReference", m_valueReferences.size());
					m_valueReferences[name] = m_valueReferences.size();
					sv.attr("description", g.getComment(*it) + (m>1 ? "[" + str(i) + "]" : "") + (n>1 ? "[" + str(j) + "]" : ""));
					//sv.attr("variability", "continuous");  // cont. is default
					//sv.attr("causality", "internal"); // internal is default
					xml::element record("Real", xw, false);
				}
		}
		for (Graph::VariableVec::iterator it=sens_vis.begin();it!=sens_vis.end();++it)
		{
			size_t m = (*it)->getShape().getDimension(1);
			size_t n = (*it)->getShape().getDimension(2);
			for (size_t i = 0; i < m; ++i)
				for (size_t j = 0; j < n; ++j)
				{
					std::string name = (*it)->getName() + (m>1 ? "_" + str(i) : "") + (n>1 ? "_" + str(j) : "");
					xml::element sv("ScalarVariable", xw);
					sv.attr("name", name);
					sv.attr("valueReference", m_valueReferences.size());
					m_valueReferences[name] = m_valueReferences.size();
					sv.attr("description", g.getComment(*it) + (m>1 ? "[" + str(i) + "]" : "") + (n>1 ? "[" + str(j) + "]" : ""));
					//sv.attr("variability", "continuous");  // cont. is default
					sv.attr("causality", "output"); 
					xml::element record("Real", xw, false);
					//record.attr("start", p.print(g.getinitVal(*it)));
					//record.attr("fixed", "true"); 
				}
		}
    }

	f << std::endl;

	f.close();
	return Util::getTime() - t1;
}
/*****************************************************************************/


/*****************************************************************************/
double FMUWriter::generateModel(Graph::Graph& g, int &dim)
/*****************************************************************************/
{
	//get timestamp and catch variables from graph
	double t1 = Util::getTime();

	std::ofstream f;
    std::string filename= m_path + "/" + m_name + ".c"; 
    f.open(filename.c_str());

	Graph::AssignmentsPtr a = g.getAssignments(DER_STATE|SENSOR|SENSOR_VISUAL);

	Graph::VariableVec states = a->getVariables(STATE);
	Graph::VariableVec variables = a->getVariables(VARIABLE);
	Graph::VariableVec inputs = a->getVariables(INPUT);
	Graph::VariableVec para_const = a->getVariables(PARAMETER|CONSTANT);
//	Graph::VariableVec constants = a->getVariables(CONSTANT);
	Graph::VariableVec userexp = a->getVariables(USER_EXP);
//	Graph::VariableVec sensors = a->getVariables(SENSOR); //Da die Ausgabe gleich ist kann zusammengefasst werden:
	Graph::VariableVec sens_vis = a->getVariables(SENSOR|SENSOR_VISUAL);

	// StateVariables Vector sortieren:
	std::sort(states.begin(),states.end(), sortVariableVec);

	f << "/* " << getHeaderLine() << " */" << std::endl;
	f << "#include <math.h>" << std::endl;
	f << "#include \"functionmodule.c\"" << std::endl;
	f << std::endl;

	f << "// define class name and unique id" << std::endl;
	f << "#define MODEL_IDENTIFIER " << m_name << std::endl;
	f << "#define MODEL_GUID \"{" << m_guid << "}\"" << std::endl;
	f << std::endl;
	f << "// define model size" << std::endl;
	f << "#define NUMBER_OF_REALS " << m_valueReferences.size() << std::endl;
	f << "#define NUMBER_OF_INTEGERS 0" << std::endl;
	f << "#define NUMBER_OF_BOOLEANS 0" << std::endl;
	f << "#define NUMBER_OF_STRINGS 0" << std::endl;
	f << "#define NUMBER_OF_STATES " << m_numberOfStates << std::endl;
	f << "#define NUMBER_OF_EVENT_INDICATORS 0" << std::endl;
	f << std::endl;
	f << "// include fmu header files, typedefs and macros" << std::endl;
	f << "#include \"fmuTemplate.h\"" << std::endl;
	f << std::endl;
	f << "// define all model variables and their value references" << std::endl;
	f << "// conventions used here:" << std::endl;
	f << "// - if x is a variable, then macro x_ is its variable reference" << std::endl;
	f << "// - the vr of a variable is its index in array  r, i, b or s" << std::endl;
	f << "// - if k is the vr of a real state, then k+1 is the vr of its derivative" << std::endl;
	for (std::map<std::string,int>::iterator it = m_valueReferences.begin(); it != m_valueReferences.end(); ++it)
		f << "#define " << it->first << "_  " << it->second << std::endl;
	f << std::endl;
	f << "// define state vector as vector of value references" << std::endl;
	f << "#define STATES {";
	for (Graph::VariableVec::iterator it=states.begin();it!=states.end();++it)
	{
		size_t m = (*it)->getShape().getDimension(1);
		size_t n = (*it)->getShape().getDimension(2);
		dim = m*n;
		for (size_t i = 0; i < m; ++i)
			for (size_t j = 0; j < n; ++j)
				f << (it==states.begin()&&!i&&!j ? " " : ", ") << (*it)->getName()
					<< (m>1 ? "_" + str(i) : "") << (n>1 ? "_" + str(j) : "") << "_" ; 
	}
	f<< " }" << std::endl;
	f << std::endl;
	f << "// called by fmiInstantiateModel" << std::endl;
	f << "// Set values for all variables that define a start value" << std::endl;
	f << "// Settings used unless changed by fmiSetX before fmiInitialize" << std::endl;
	f << "void setStartValues(ModelInstance *comp) {" << std::endl;
	for (Graph::VariableVec::iterator it=states.begin();it!=states.end();++it)
	{
		size_t m = (*it)->getShape().getDimension(1);
		size_t n = (*it)->getShape().getDimension(2);
		for (size_t i = 0; i < m; ++i)
			for (size_t j = 0; j < n; ++j)
				f << "    " << m_p->print(new Element(*it, i, j)) << " = "
				  << ( m>1 || n>1 ? m_p->print(g.getinitVal(*it)->getArg(i*n+j)) : m_p->print(g.getinitVal(*it))) << ";" << std::endl;
	}
	for (Graph::VariableVec::iterator it=inputs.begin();it!=inputs.end();++it)
	{
		size_t m = (*it)->getShape().getDimension(1);
		size_t n = (*it)->getShape().getDimension(2);
		for (size_t i = 0; i < m; ++i)
			for (size_t j = 0; j < n; ++j)
				f << "    " << m_p->print(new Element(*it, i, j)) << " = "
				  << ( m>1 || n>1 ? "0" : m_p->print(g.getinitVal(*it))) << ";" << std::endl;
	}
	for (Graph::VariableVec::iterator it=para_const.begin();it!=para_const.end();++it)
	{
		size_t m = (*it)->getShape().getDimension(1);
		size_t n = (*it)->getShape().getDimension(2);
		for (size_t i = 0; i < m; ++i)
			for (size_t j = 0; j < n; ++j)
				f << "    " << m_p->print(new Element(*it, i, j)) << " = "
				  << m_p->print(new Element(g.getEquation(*it), i, j)) << ";" << std::endl;
	}
	f << "}" << std::endl;
	f << std::endl;
	f << "// called by fmiInitialize() after setting eventInfo to defaults" << std::endl;
	f << "// Used to set the first time event, if any." << std::endl;
	f << "void initialize(ModelInstance* comp, fmiEventInfo* eventInfo) {" << std::endl;
	f << "}" << std::endl;
	f << std::endl;

	f << "void calcDerivate(ModelInstance* comp) {" << std::endl;
	f << "    /* ordinary variables */" << std::endl;
    for (Graph::VariableVec::iterator it=variables.begin();it!=variables.end();++it)
        f << "    double " << m_p->print(*it) << m_p->dimension(*it) << "= " << m_p->print(g.getinitVal(*it)) << "; " << m_p->comment2(g,*it) <<  std::endl;
	f << std::endl;
	f << "    /* calculate state derivative */" << std::endl;
	f << writeEquations(a->getEquations(PARAMETER | CONSTANT | INPUT )) << std::endl;
    f << std::endl;
	f << "} " << std::endl;
	f << std::endl;

	f << "// called by fmiGetReal, fmiGetContinuousStates and fmiGetDerivatives" << std::endl;
	f << "fmiReal getReal(ModelInstance* comp, fmiValueReference vr){" << std::endl;
	f << "    if (newContinousStateSet == fmiTrue) {" << std::endl;
	f << "        calcDerivate(comp);" << std::endl;
	f << "        newContinousStateSet = fmiFalse;" << std::endl;
	f << "    }" << std::endl;
	f << "    " << std::endl;
	f << "    return r(vr);" << std::endl;
	f << "}" << std::endl;
	f << std::endl;
	f << "// Used to set the next time event, if any." << std::endl;
	f << "void eventUpdate(fmiComponent comp, fmiEventInfo* eventInfo) {" << std::endl;
	f << "} " << std::endl;
	f << std::endl;
	f << "// include code that implements the FMI based on the above definitions" << std::endl;
	f << "#include \"fmuTemplate.c\"" << std::endl;
	f << std::endl;


	f << std::endl;

	f.close();

    if (m_p->getErrorcount())
        std::cerr << "There have been " << m_p->getErrorcount(true) << " error(s) during generation of " 
                  << m_name << ".c for FMU Export. Please have a look at that file to see where the error(s) occured." << std::endl;
	return Util::getTime() - t1;
}
/*****************************************************************************/


