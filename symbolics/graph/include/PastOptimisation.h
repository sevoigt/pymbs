#ifndef __PASTOPTIMISATION_H_
#define __PASTOPTIMISATION_H_

#include <string>
#include <map>

#include "Symbolics.h"
#include "Node.h"

#include "EquationSystem.h"


namespace Symbolics
{
    namespace Graph
    {

/*****************************************************************************/
        class PastOptimisation
        {
        public:
            // Konstruktor
            PastOptimisation(EquationSystemPtr eqsys, NodeVec &nodes);
            // Destruktor
            ~PastOptimisation();

            // run Optimization
            void optimize();

        protected:

          EquationSystemPtr m_eqsys;
          NodeVec &m_nodes;

          class NodeOptimizer: public Node::Visitor, public Basic::Scanner
          {
          public:
              NodeOptimizer(BasicPtr const& T);
              ~NodeOptimizer();
                
              void process_Node(NodePtr p);

              bool process_Arg(BasicPtr const &p, bool &stop);
              double removed;

              BasicPtr oneparentsymbol;
              size_t symbolcount;

              NodeVec removed_nodes;
              SymbolPtrVec removed_symbols;
          protected:
              BasicPtr time;
              double start;

              void subs_Node(NodePtr p, SymbolPtr const& symbol, BasicPtr const& exp);
          };

        private:
            BasicPtr m_time;
         };
/*****************************************************************************/
    };
};

#endif // __GRAPH_GRAPH_H_