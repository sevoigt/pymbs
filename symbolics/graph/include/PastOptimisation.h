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