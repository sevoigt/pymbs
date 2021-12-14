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