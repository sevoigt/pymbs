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

#ifndef __GRAPH_ASSIGNMENT_H_
#define __GRAPH_ASSIGNMENT_H_

#include <string>
#include <map>
#include "Symbolics.h"
#include "Node.h"

#include "intrusive_ptr.h"

// Forward Declarations
namespace Symbolics
{
    namespace Graph
    {
        class Assignments;
    };
};

/*****************************************************************************/
namespace boost
{
    void intrusive_ptr_add_ref( const Symbolics::Graph::Assignments *p);
    void intrusive_ptr_release( const Symbolics::Graph::Assignments *p);
};
/*****************************************************************************/

namespace Symbolics
{
    namespace Graph
    {
/*****************************************************************************/
        typedef SymbolPtrVec VariableVec;

/*****************************************************************************/


        typedef boost::intrusive_ptr<Assignments> AssignmentsPtr;
        typedef boost::intrusive_ptr<const Assignments> ConstAssignmentsPtr;

/*****************************************************************************/

        class Assignment
        {
        public:
            Assignment( NodePtr node);
            ~Assignment();
            BasicPtrVec lhs;
            BasicPtrVec rhs;
            Category_Type category;
            bool implizit;
        };

/*****************************************************************************/
        class Assignments
        {
        public:
            Assignments(std::vector<NodePtr> const& nodes);
            ~Assignments();


            VariableVec getVariables(Category_Type Category) const;
            std::vector<Graph::Assignment> getEquations(Category_Type exclude = 0x00) const;
        protected:
            SymbolPtrVec m_symbols;
            std::vector<Graph::Assignment> m_assignments;

    private:
        // Boost Intrusive Ptr
        unsigned int m_refCount;
        // friend
        friend void ::boost::intrusive_ptr_add_ref( const Assignments* p);
        friend void ::boost::intrusive_ptr_release( const Assignments* p);
        };
/*****************************************************************************/
    };
};

/*****************************************************************************/
namespace boost
{
    // increment reference count of object *p
    inline void intrusive_ptr_add_ref( const Symbolics::Graph::Assignments* p)
    {
        // Muss sein, da ich sonst keine  Basics in intrusive Ptr legen kann
        Symbolics::Graph::Assignments* b = const_cast<Symbolics::Graph::Assignments*>(p);
        ++(b->m_refCount);
    }

    // decrement reference count, and delete object when reference count reaches 0
    inline void intrusive_ptr_release( const Symbolics::Graph::Assignments* p)
    {
        // Muss sein, da ich sonst keine  Basics in intrusive Ptr legen kann
        Symbolics::Graph::Assignments* b = const_cast<Symbolics::Graph::Assignments*>(p);
        if (--(b->m_refCount) == 0)
            delete p;
    } 
}; // namespace boost
/*****************************************************************************/

#endif // __GRAPH_ASSIGNMENT_H_
