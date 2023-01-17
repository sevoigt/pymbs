#ifndef __MATCHEDSYSTEM_H_
#define __MATCHEDSYSTEM_H_

#include <string>
#include <map>
#include "Symbolics.h"

#include "EquationSystem.h"
#include "SystemHandler.h"

namespace Symbolics
{
    namespace Graph
    {

/*****************************************************************************/
        class MatchedSystem: public SystemHandler
        {
        public:
            // Konstruktor
            MatchedSystem(EquationSystemPtr eqsys, NodeVec& nodes);
            // Destruktor
            ~MatchedSystem();

            // generate the internal graph
            double buildGraph();

            // to GraphML
            void toGraphML( std::string file );

            // sort equations
            // throws: InternalError
            void sortEquations();

        private:
            SymbolPtr m_time;

            class NodeSorter: public Node::Visitor
            {
            public:
              NodeSorter() {;};
              ~NodeSorter() {;};
                
              inline void process_Node(NodePtr p) {m_nodes.push_back(p);};
              inline NodeVec getNodes() { return m_nodes;};

            protected:
              NodeVec m_nodes;
            };
        };
/*****************************************************************************/
    };
};

#endif // __MATCHEDSYSTEM_H_