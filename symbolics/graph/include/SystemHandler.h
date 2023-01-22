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
