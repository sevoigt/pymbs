#include <iostream>
#include "Symbolics.h"
#include "Graph.h"

using namespace Symbolics;

int addSymbol( int &argc,  char *argv[])
{
    // Beispiel aufbauen
    // subs(exp,old,new)

    Graph::Graph g;
    // throw SymbolAlreadyExistsError when adding twice
    SymbolPtr s(new Symbol("a"));
    g.addSymbol(s);
    try
    {
        g.addSymbol(s);
    }
    catch(Graph::SymbolAlreadyExistsError)
    {
    
    }
    catch(...)
    {
        return -1;
    }
    // throw SymbolKindError when STATE or DER_STATE
    SymbolPtr b(new Symbol("b",Shape(1,1),STATE));
    try
    {
        g.addSymbol(b);
    }
    catch(Graph::SymbolKindError)
    {
    
    }
    catch(...)
    {
        return -2;
    }
    SymbolPtr c(new Symbol("c",Shape(1,1),DER_STATE));
    try
    {
        g.addSymbol(c);
    }
    catch(Graph::SymbolKindError)
    {
    
    }
    catch(...)
    {
        return -3;
    }

    return 0;
}

int addExpression( int &argc,  char *argv[])
{
    // Beispiel aufbauen
    // subs(exp,old,new)

    Graph::Graph g;
    // throw SymbolAlreadyExistsError when adding twice
    SymbolPtr d(new Symbol("d"));

    g.addSymbol(d);

    BasicPtr exp(new Neg(d));
    try
    {
        g.addExpression(d,exp);
    }
    catch(Graph::ExpressionSolveForError)
    {
    
    }
    catch(...)
    {
        return -10;
    }

    return 0;
}

int getEquations( int &argc,  char *argv[])
{
    // Beispiel aufbauen
    // subs(exp,old,new)

    Graph::Graph gr;
    
    // throw SymbolAlreadyExistsError when adding twice
    SymbolPtr e(new Symbol("e",PARAMETER));
    SymbolPtr f(new Symbol("f",PARAMETER));
    SymbolPtr g(new Symbol("g"));
    SymbolPtr h(new Symbol("h"));
    SymbolPtr i(new Symbol("i"));

    gr.addSymbol(e);
    gr.addSymbol(f);
    gr.addSymbol(g);
    gr.addSymbol(h);
    gr.addSymbol(i);

    BasicPtr exp_e(new Real(1.0));
    BasicPtr exp_f(new Real(1.0));
    BasicPtr exp_g = Add::New(e,h);
    BasicPtr exp_h = Add::New(Neg::New(e),f);
    BasicPtr exp_i = Mul::New(g,h);
    gr.addExpression(e,exp_e,false);
    gr.addExpression(f,exp_f,false);
    gr.addExpression(g,exp_g,false);
    gr.addExpression(h,exp_h,false);
    gr.addExpression(i,exp_i,false);
    
    gr.buildGraph(false);

    Graph::ConstAssignmentsPtr a = gr.getAssignments(Symbolics::VARIABLE,Symbolics::PARAMETER);

    Graph::VariableVec symbols = a->getVariables(Symbolics::VARIABLE);
    if (symbols.size() != 3) return -11;

    bool has_h=false;
    bool has_g=false;
    bool has_i=false;
    for (size_t l=0;l<3;l++)
    {
      if ((symbols[l] == h) & (!has_h))
      {
        has_h=true;
        continue;
      }
      if ((symbols[l] == g) & (!has_g))
      {
        has_g=true;
        continue;
      }
      if ((symbols[l] == i) & (!has_i))
      {
        has_i=true;
        continue;
      }
      return -12;
    }

    if (!has_h) return -13;
    if (!has_g) return -14;
    if (!has_i) return -15;

    return 0;
}

int toGraphML( int &argc,  char *argv[])
{
    // Beispiel aufbauen
    // subs(exp,old,new)

    Graph::Graph gr;
    // throw SymbolAlreadyExistsError when adding twice
    SymbolPtr e(new Symbol("e",PARAMETER));
    SymbolPtr f(new Symbol("f",PARAMETER));
    SymbolPtr g(new Symbol("g"));
    SymbolPtr h(new Symbol("h"));
    SymbolPtr i(new Symbol("i"));

    gr.addSymbol(e);
    gr.addSymbol(f);
    gr.addSymbol(g);
    gr.addSymbol(h);
    gr.addSymbol(i);

    BasicPtr exp_e(new Real(1.0));
    BasicPtr exp_f(new Real(1.0));
    BasicPtr exp_g = Add::New(e,f);
    BasicPtr exp_h = Add::New(Neg::New(e),f);
    BasicPtr exp_i = Mul::New(g,h);
    gr.addExpression(e,exp_e,false);
    gr.addExpression(f,exp_f,false);
    gr.addExpression(g,exp_g,false);
    gr.addExpression(h,exp_h,false);
    gr.addExpression(i,exp_i,false);
    gr.toGraphML("test.graphml");

    return 0;
}


int LinearModel( int &argc,  char *argv[])
{
    // Beispiel aufbauen

    // throw SymbolAlreadyExistsError when adding twice
    size_t N=6;

    std::vector<size_t> ens;

    unsigned int base=16384; 
    for (size_t i=0;i<N;i++)
    {
        ens.push_back(base);
        base = base*2;
    }

    std::vector<double> times;

    for (size_t l=0;l<ens.size();l++)
    {
        Graph::Graph g;

        std::vector<SymbolPtr> simbolvector;
        SymbolPtr b(new Symbol("b"));
        simbolvector.push_back(b);
        g.addSymbol(b);
        BasicPtr bexp(new Int(1));
        g.addExpression(b,bexp);
        for (size_t i=0;i<ens[l];++i)
        {
            SymbolPtr s(new Symbol("a"+str(i)));
            simbolvector.push_back(s);
            g.addSymbol(s);
        }

        for (size_t i=1;i<simbolvector.size();++i)
        {
            BasicPtr exp(simbolvector[i-1]);
            g.addExpression(simbolvector[i],exp);
        }
    
        double t = g.buildGraph(true);
        times.push_back(t);

        std::cout << ens[l] << ": " << t << std::endl;
    }

    return 0;
}

int NonLinearModel( int &argc,  char *argv[])
{
    // Beispiel aufbauen

    // throw SymbolAlreadyExistsError when adding twice
    size_t N=6;

    std::vector<size_t> ens;

    unsigned int base=128; 
    for (size_t i=0;i<N;i++)
    {
        ens.push_back(base);
        base = base*2;
    }

    std::vector<double> times;

    for (size_t l=0;l<ens.size();l++)
    {
        Graph::Graph g;

        std::vector<SymbolPtr> simbolvector;
        SymbolPtr b(new Symbol("b"));
        simbolvector.push_back(b);
        g.addSymbol(b);
        BasicPtr bexp(new Int(1));
        g.addExpression(b,bexp);
        for (size_t i=0;i<ens[l];++i)
        {
            SymbolPtr s(new Symbol("a"+str(i)));
            simbolvector.push_back(s);
            g.addSymbol(s);
        }

        for (size_t i=1;i<simbolvector.size()-1;++i)
        {
            BasicPtr exp(simbolvector[i-1]);
            g.addExpression(simbolvector[i],exp);
        }

        BasicPtr exp(new Int(0));
        for (size_t j=0;j<simbolvector.size()-1;++j)
        {
            exp = Add::New(exp,simbolvector[j]);
        }
        g.addExpression(simbolvector[simbolvector.size()-1],exp);
   
        double t = g.buildGraph(true);
        times.push_back(t);

        std::cout << ens[l] << ": " << t << std::endl;
    }

    return 0;
}

int main( int argc,  char *argv[])
{
    int res = 0;
    bool t = true;
    if (t)
    {
        res = addSymbol(argc,argv);
        if (res !=0) return res;
        res = addExpression(argc,argv);
        if (res !=0) return res;
        res = getEquations(argc,argv);
        if (res !=0) return res;
        res = toGraphML(argc,argv);
        if (res !=0) return res;
    }
    if(!t)
    {
        res = LinearModel(argc,argv);
        if (res !=0) return res;
    }
    if(!t)
    {
        res = NonLinearModel(argc,argv);
        if (res !=0) return res;
    }

    

}