#include <iostream>
#include <cstdlib>
#include "Symbolics.h"
#include "pendulum.h"
#include "ModelicaWriter.h"
#include "FortranWriter.h"
#include "CWriter.h"
#include "PythonWriter.h"
#include "MatlabWriter.h"

using namespace Symbolics;

int pendulum()
{
    
    Graph::Graph g = Pendulum::getGraph();

    g.buildGraph(true);

    ModelicaWriter writer;

    writer.generateTarget("Pendulum","./.",g,true);

    return 0; //�berpr�ft somit nur, ob der Code generell ohne abgefangene Fehler erstellt werden kann.
              //Keine Aussage dar�ber ob sich der Code mit einem Modelica Compiler �bersetzen l�sst.
}

int pendulumDAE( )
{
    
    Graph::Graph g = Pendulum::getGraphDAE();

    g.buildGraph(true);

    ModelicaWriter writer;

    writer.generateTarget("PendulumDAE","./.",g,true);

    return 0; //�berpr�ft somit nur, ob der Code generell ohne abgefangene Fehler erstellt werden kann.
              //Keine Aussage dar�ber ob sich der Code mit einem Modelica Compiler �bersetzen l�sst.
}

int pend_expl_mod( )
{
    
    Graph::Graph g = Pendulum::getGraph_expl();
    g.buildGraph(true);
    ModelicaWriter writer;
    writer.generateTarget("Pendulum_expl","./.",g,true);
    return 0; //�berpr�ft somit nur, ob der Code generell ohne abgefangene Fehler erstellt werden kann.
              //Keine Aussage dar�ber ob sich der Code mit einem Modelica Compiler �bersetzen l�sst.
}

int pend_expl_mat( )
{
    
    Graph::Graph g = Pendulum::getGraph_expl();
    g.buildGraph(true);
    MatlabWriter writer;
    writer.generateTarget("Pendulum_expl","./.",g,true);
    return 0; //�berpr�ft somit nur, ob der Code generell ohne abgefangene Fehler erstellt werden kann.
              //Keine Aussage dar�ber ob sich der Code mit MATLAB ausf�hren l�sst.
}

int pend_expl_c()
{
    Graph::Graph g = Pendulum::getGraph_expl();
    g.buildGraph(true);
    CWriter writer;
    writer.generateTarget("Pendulum_expl","./.",g,true);

    //Modell kompilieren 
    int res = 0;
	std::string cmd = "gcc -shared \"Pendulum_expl_der_state.c\"";
    res += system(cmd.c_str()); 
    cmd = "gcc -shared \"Pendulum_expl_visual.c\"";
    res += system(cmd.c_str()); 
    cmd = "gcc -shared \"Pendulum_expl_sensors.c\"";
    res += system(cmd.c_str()); 

    return res;
}

int pend_expl_fortran()
{
    Graph::Graph g = Pendulum::getGraph_expl();
    g.buildGraph(true);
    FortranWriter writer;
    writer.generateTarget("Pendulum_expl","./.",g,true);

    //Modell kompilieren 
    int res = 0;
	std::string cmd = "gfortran -shared functionmodule.f90 \"Pendulum_expl_der_state.f90\" -ffree-line-length-none";
    res += system(cmd.c_str()); 
    cmd = "gfortran -shared functionmodule.f90 \"Pendulum_expl_visual.f90\" -ffree-line-length-none";
    res += system(cmd.c_str()); 
    cmd = "gfortran -shared functionmodule.f90 \"Pendulum_expl_sensors.f90\" -ffree-line-length-none";
    res += system(cmd.c_str()); 
    
    return res;
}

int pend_expl_python()
{
    Graph::Graph g = Pendulum::getGraph_expl();
    g.buildGraph(true);
    PythonWriter writer;
    writer.generateTarget("Pendulum_expl","./.",g,true);

    //Modell �berpr�fen 
    int res = 0;
	std::string cmd = "python \"Pendulum_expl_der_state.py\"";
    res += system(cmd.c_str()); 
    cmd = "python \"Pendulum_expl_visual.py\"";
    res += system(cmd.c_str()); 
    cmd = "python \"Pendulum_expl_sensors.py\"";
    res += system(cmd.c_str()); 

    
    return res;
}


int main( int argc,  char *argv[])
{
    
    int res=0;
    res = pendulum();
    if (res != 0) return -1;
    res = pendulumDAE();
    if (res != 0) return -2;
    res = pend_expl_mod();
    if (res != 0) return -3;
    res = pend_expl_mat();
    if (res != 0) return -4;
    res = pend_expl_c();
    if (res != 0) return -5;
    res = pend_expl_fortran();
    if (res != 0) return -6;
    res = pend_expl_python();
    if (res != 0) return -7;

    return 0;
}
