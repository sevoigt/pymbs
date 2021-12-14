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

#ifndef __SYSTEMHANDLER_H_
#define __SYSTEMHANDLER_H_

#include "EquationSystem.h"
#include "Node.h"

#include "intrusive_ptr.h"

// Forward Declarations
namespace Symbolics
{
  namespace Graph
  {
    class SystemHandler;
  };
};

/*****************************************************************************/
namespace boost
{
  void intrusive_ptr_add_ref( const Symbolics::Graph::SystemHandler *p);
  void intrusive_ptr_release( const Symbolics::Graph::SystemHandler *p);
};
/*****************************************************************************/

namespace Symbolics
{
  namespace Graph
  {
/*****************************************************************************/
    typedef boost::intrusive_ptr<SystemHandler> SystemHandlerPtr;

/*****************************************************************************/
    class SystemHandler
    {
    public:
      SystemHandler(EquationSystemPtr eqsys, NodeVec& nodes): m_eqsys(eqsys),m_nodes(nodes),m_refCount(0) {;};
      virtual ~SystemHandler() {;};

      // generate the internal graph
      virtual double buildGraph() = 0;

      // to GraphML
      virtual void toGraphML( std::string file ) = 0;

    protected: 
      EquationSystemPtr m_eqsys;
      NodeVec& m_nodes;

    private:
      // Boost Intrusive Ptr
      unsigned int m_refCount;
      // friend
      friend void ::boost::intrusive_ptr_add_ref( const SystemHandler* p);
      friend void ::boost::intrusive_ptr_release( const SystemHandler* p);
    };
/*****************************************************************************/
  };
};
/*****************************************************************************/
namespace boost
{
  // increment reference count of object *p
  inline void intrusive_ptr_add_ref( const Symbolics::Graph::SystemHandler* p)
  {
    // Muss sein, da ich sonst keine  Basics in intrusive Ptr legen kann
    Symbolics::Graph::SystemHandler* b = const_cast<Symbolics::Graph::SystemHandler*>(p);
    ++(b->m_refCount);
  }

  // decrement reference count, and delete object when reference count reaches 0
  inline void intrusive_ptr_release( const Symbolics::Graph::SystemHandler* p)
  {
    // Muss sein, da ich sonst keine  Basics in intrusive Ptr legen kann
    Symbolics::Graph::SystemHandler* b = const_cast<Symbolics::Graph::SystemHandler*>(p);
    if (--(b->m_refCount) == 0)
      delete p;
  } 
}; // namespace boost
/*****************************************************************************/
#endif // __SYSTEMHANDLER_H_
