#include <iostream>
#include "Symbolics.h"
#include "Graph.h"

using namespace Symbolics;

int pendulum( int &argc,  char *argv[])
{
  // Beispiel aufbauen
  // subs(exp,old,new)

  Graph::Graph gr;

  // throw SymbolAlreadyExistsError when adding twice
  SymbolPtr L(new Symbol("L",PARAMETER));
  SymbolPtr m(new Symbol("m",PARAMETER));
  SymbolPtr g(new Symbol("g",PARAMETER));
  SymbolPtr x(new Symbol("x"));
  SymbolPtr y(new Symbol("y"));
  SymbolPtr xd(new Symbol("xd"));
  SymbolPtr yd(new Symbol("yd"));
  SymbolPtr xdd(new Symbol("xdd"));
  SymbolPtr ydd(new Symbol("ydd"));
  SymbolPtr F(new Symbol("F"));

  gr.addSymbol(L);
  gr.addSymbol(m); 
  gr.addSymbol(g);
  gr.addSymbol(x,L.get());
  gr.addSymbol(y);
  gr.addSymbol(xd);
  gr.addSymbol(yd);
  gr.addSymbol(xdd);
  gr.addSymbol(ydd);
  gr.addSymbol(F);

  gr.addExpression(L,Int::New(1));
  gr.addExpression(m,Int::New(1));
  gr.addExpression(g,Real::New(9.81));

  gr.addExpression(NULL,Pow::New(x,Int::New(2)) +  Pow::New(y,Int::New(2)) - Pow::New(L,Int::New(2))); // y
  gr.addExpression(NULL,Der::New(x)-xd);  // der(x)
  gr.addExpression(NULL,m*xdd+Util::div(x*F,L)); // F
  gr.addExpression(NULL,m*ydd+m*g+Util::div(F*y,L)); // ydd
  gr.addExpression(NULL,2*(xd*x+yd*y)); // xd
  gr.addExpression(NULL,2*(Pow::New(xd,Int::New(2))+xdd*x+Pow::New(yd,Int::New(2))+ydd*y)); // xdd
  gr.addExpression(NULL,ydd-Der::New(yd)); // der(yd)

  gr.buildGraph(true);

  return 0;
}

int pendulumDAE( int &argc,  char *argv[])
{
  // Beispiel aufbauen
  // subs(exp,old,new)

  Graph::Graph gr;

  // throw SymbolAlreadyExistsError when adding twice
  SymbolPtr L(new Symbol("L",PARAMETER));
  SymbolPtr m(new Symbol("m",PARAMETER));
  SymbolPtr g(new Symbol("g",PARAMETER));
  SymbolPtr x(new Symbol("x"));
  SymbolPtr y(new Symbol("y"));
  SymbolPtr xd(new Symbol("xd"));
  SymbolPtr yd(new Symbol("yd"));
  SymbolPtr F(new Symbol("F"));

  gr.addSymbol(L);
  gr.addSymbol(m); 
  gr.addSymbol(g);
  gr.addSymbol(x,L.get());
  gr.addSymbol(y);
  gr.addSymbol(xd);
  gr.addSymbol(yd);
  gr.addSymbol(F);

  gr.addExpression(L,Int::New(1));
  gr.addExpression(m,Int::New(1));
  gr.addExpression(g,Real::New(9.81));

  gr.addExpression(NULL,Pow::New(x,Int::New(2)) +  Pow::New(y,Int::New(2)) - Pow::New(L,Int::New(2))); 
  gr.addExpression(NULL,Der::New(x)-xd);  
  gr.addExpression(NULL,Der::New(y)-yd);  
  gr.addExpression(NULL,m*Der::New(xd)+Util::div(x*F,L)); 
  gr.addExpression(NULL,m*Der::New(yd)+m*g+Util::div(F*y,L)); 

  gr.buildGraph(true);

  return 0;
}

int main( int argc,  char *argv[])
{
  int res = 0;
  res = pendulum(argc,argv);
  if (res !=0) return res;
  res = pendulumDAE(argc,argv);
  if (res !=0) return res;
  return 0;
}