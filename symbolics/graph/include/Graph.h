#ifndef __GRAPH_GRAPH_H_
#define __GRAPH_GRAPH_H_

#include <string>
#include <map>

#include "Symbolics.h"
#include "EquationSystem.h"
#include "SystemHandler.h"
#include "Assignments.h"

namespace Symbolics
{
    namespace Graph
    {

/*****************************************************************************/
        class Graph
        {
        public:
            // Konstruktor
            Graph();
            // Destruktor
            ~Graph();


            // add Symbol
            // throws: SymbolAlreadyExistsError, SymbolKindError,InitalValueError
            BasicPtr addSymbol( SymbolPtr const& s,  Basic *initalValue = NULL, std::string comment = "");

            // add Expression : category wird als Bitfolge implementiert 1:an 0:aus 
            // throws: ExpressionSolveForError, SymbolAlreadySolvedInError, VarUnknownError
            void addExpression( BasicPtr const& solveFor, BasicPtr const& exp, bool implicit=false);

            // add Expression
            // throws: ExpressionSolveForError, SymbolAlreadySolvedInError, VarUnknownError
            void addExpression( BasicPtrVec const& solveFor, BasicPtrVec const& exp, bool implicit=false);

            // scalar
            void makeScalar();

            // 
            double buildGraph(bool optimize);

            // getsolved System
            AssignmentsPtr getAssignments(Category_Type inc, Category_Type exclude=0x00);

            // getSymbol
            // throws: VarUnknownError
            SymbolPtr getSymbol( std::string const& name);

            // getVariables
            SymbolPtrVec getVariables(Symbol_Kind kind);

            // initalValue
            BasicPtr getinitVal(SymbolPtr const& s);

            // equation of symbol, does only work for equations with implicit=false 
            BasicPtr getEquation(SymbolPtr const& s);

            // comment
            std::string getComment(SymbolPtr const& s);

            // Derivative Order
            size_t getDerivativeOrder(SymbolPtr const& s);

            // to GraphML
            void toGraphML( std::string file );

        protected:
          EquationSystemPtr eqsys;
          SystemHandlerPtr m_syshandler;

          NodeVec m_nodes;

          typedef std::map<SymbolPtr, std::pair< size_t, SymbolPtr> > DerivativeOrderMap;
          DerivativeOrderMap m_derivativeOrder;

          class NodeCollector: public Node::Visitor
          {
          public:
              NodeCollector(Category_Type exclude,NodeVec &reqnodes);
              ~NodeCollector();

              void process_Node(NodePtr p);

          protected:
              Category_Type m_exclude;
              // Equations
              NodeVec& m_nodes;
          };

        private:
            BasicPtr m_time;
         };
/*****************************************************************************/
    };
};

#endif // __GRAPH_GRAPH_H_