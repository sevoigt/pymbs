#ifndef __PENDULUM_H_
#define __PENDULUM_H_

#include "Symbolics.h"
#include "Graph.h"
#include "Writer.h" // Damit die SENSORS definitionen verf�gbar sind

namespace Symbolics
{

    namespace Pendulum
    {
        Graph::Graph getGraph()
        {
            Graph::Graph graph;

            // Beispiel aufbauen
            BasicPtr m = graph.addSymbol(new Symbol("m",PARAMETER));
            BasicPtr g = graph.addSymbol(new Symbol("g",CONSTANT));
            BasicPtr L = graph.addSymbol(new Symbol("L",PARAMETER));
            BasicPtr x = graph.addSymbol(new Symbol("x"),L.get());
            BasicPtr y = graph.addSymbol(new Symbol("y"));
            BasicPtr xd = graph.addSymbol(new Symbol("xd"));
            BasicPtr yd = graph.addSymbol(new Symbol("yd"));
            BasicPtr ydd = graph.addSymbol(new Symbol("ydd"));
            BasicPtr xdd = graph.addSymbol(new Symbol("xdd"));
            BasicPtr Fo = graph.addSymbol(new Symbol("Fo"));

            // Expression definieren
            graph.addExpression(m,BasicPtr(new Real(1)));
            graph.addExpression(g,BasicPtr(new Real(9.81)));
            graph.addExpression(L,BasicPtr(new Real(0.5)));

            graph.addExpression(y,BasicPtr(new Pow(x,2))+BasicPtr(new Pow(y,2))-BasicPtr(new Pow(L,2)),true);
            graph.addExpression(xd,yd*y*BasicPtr(new Pow(x,-1)),false);
            graph.addExpression(BasicPtr(new Der(x)),xd);
            
            BasicPtr exp3 = m*xdd*L*BasicPtr(new Pow(Neg::New(x),-1)); // Fo
            BasicPtr exp4 = Neg::New(m)*g*Neg::New(Fo)*x*BasicPtr(new Pow(m*L,-1)); // ydd
            BasicPtr exp6 = 2*Add::New( BasicPtr(new Pow(x,2)),xdd*x+BasicPtr(new Pow(yd,2))+ydd*y); // xdd

            std::vector<BasicPtr> solveFor;
            solveFor.push_back(ydd);
            solveFor.push_back(Fo);
            solveFor.push_back(xdd);
            std::vector<BasicPtr> exps;
            exps.push_back(exp4);
            exps.push_back(exp3);
            exps.push_back(exp6);
            graph.addExpression(solveFor,exps,true);

            graph.addExpression(Der::New(yd),ydd);

            return graph;
        };

        Graph::Graph getGraphDAE()
        {
            Graph::Graph gr;

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

            return gr;
        };

        Graph::Graph getGraph_expl()
        {
            Graph::Graph gr;

            // Beispiel aufbauen
            BasicPtr m = gr.addSymbol(new Symbol("m",PARAMETER));
            BasicPtr g = gr.addSymbol(new Symbol("g",CONSTANT));
            BasicPtr L = gr.addSymbol(new Symbol("L",PARAMETER));
            BasicPtr in = gr.addSymbol(new Symbol("inertia",PARAMETER));
            BasicPtr q = gr.addSymbol(new Symbol("q"),new Int(1));
            BasicPtr qd = gr.addSymbol(new Symbol("qd"));
            BasicPtr M_ = gr.addSymbol(new Symbol("M_"));
            BasicPtr f = gr.addSymbol(new Symbol("f"));
            BasicPtr qdd = gr.addSymbol(new Symbol("qdd"));

            // Sinnlose Sensoren definieren, damit die Dateien generiert werden
            BasicPtr sen = gr.addSymbol(new Symbol("sens",Writer::SENSOR));
            BasicPtr senvis = gr.addSymbol(new Symbol("sensvis",Writer::SENSOR_VISUAL));

            // Expression definieren
            gr.addExpression(m,BasicPtr(new Real(1)));
            gr.addExpression(g,BasicPtr(new Real(9.81)));
            gr.addExpression(L,BasicPtr(new Real(1)));
            gr.addExpression(in,BasicPtr(new Real(1)));

            gr.addExpression(BasicPtr(new Der(q)), qd);
            BasicPtrVec b1,b2,b3,b4;
            b1.push_back(BasicPtr(new Real(0.25)));
            b1.push_back(BasicPtr(new Pow(L,2)));
            b1.push_back(m);
            b1.push_back(BasicPtr(new Pow(BasicPtr(new Sin(q)),2)));

            b2.push_back(BasicPtr(new Real(0.25)));
            b2.push_back(BasicPtr(new Pow(L,2)));
            b2.push_back(m);
            b2.push_back(BasicPtr(new Pow(BasicPtr(new Cos(q)),2)));

            b3.push_back(in);
            b3.push_back(BasicPtr(new Mul(b1)));
            b3.push_back(BasicPtr(new Mul(b2)));

            gr.addExpression(M_,BasicPtr( new Add(b3)));

            b4.push_back(Real::New(-0.5));
            b4.push_back(L);
            b4.push_back(g);
            b4.push_back(m);
            b4.push_back(Sin::New(q));

            gr.addExpression(f, Neg::New(BasicPtr(new Mul(b4))));

            gr.addExpression(Der::New(qd),Mul::New(f,BasicPtr(new Pow(M_,-1))));

            gr.addExpression(sen, M_);
            gr.addExpression(senvis, f);

            return gr;
        };

    };
};

#endif // __PENDULUM_H_