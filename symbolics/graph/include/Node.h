#ifndef __GRAPH_NODE_H_
#define __GRAPH_NODE_H_

#include <string>

#include "Symbolics.h"
#include "Equation.h"

#include "intrusive_ptr.h"

// Forward Declarations
namespace Symbolics
{
  namespace Graph
  {
    class Node;
  };
};

/*****************************************************************************/
namespace boost
{
  void intrusive_ptr_add_ref( const Symbolics::Graph::Node *p);
  void intrusive_ptr_release( const Symbolics::Graph::Node *p);
};
/*****************************************************************************/

namespace Symbolics
{
  namespace Graph
  {
    /*****************************************************************************/
    typedef unsigned int Category_Type;
    /*****************************************************************************/


    typedef boost::intrusive_ptr<const Node> ConstNodePtr;
    typedef boost::intrusive_ptr<Node> NodePtr;
    typedef std::set<NodePtr> NodePtrSet;
    typedef std::vector<NodePtr> NodeVec;


    typedef std::map<SizeTPair,NodePtr> SizeTPairNodePtrMap;
      typedef struct SymbolNodeElementT {
        SymbolElementKind kind;
        NodePtr node;
        SizeTPairNodePtrMap elements;
        SymbolNodeElementT(): kind(SEVERAL) {;};
        SymbolNodeElementT(NodePtr n): kind(ALL), node(n) {;};
        SymbolNodeElementT(size_t dim1, size_t dim2, NodePtr n): kind(SEVERAL) { elements[SizeTPair(dim1,dim2)]=n;};
      } SymbolNodeElement;

    /*****************************************************************************/
    class Node
    {
    public:
      // Konstruktor
      Node(EquationPtr eqn,SymbolPtr time);
      // Destruktor
      virtual ~Node();

      // returns the symbols
      inline  SymbolPtrElemSizeTMap& getSymbols()  {return m_eqn->getSymbols(); };
      inline  SymbolPtrElemMap& getSolveFor()  {return m_eqn->getSolveFor(); };

      // returns the expressions
      inline size_t getLhsSize() { return m_eqn->getLhsSize(); };
      inline  BasicPtr const& getLhs(size_t i)  { return m_eqn->getLhs(i); };
      inline size_t getRhsSize() { return m_eqn->getRhsSize(); };
      inline  BasicPtr const& getRhs(size_t i)  { return m_eqn->getRhs(i); };
      inline  bool is_Implicit()  { return m_eqn->is_Implicit(); };
      inline  Category_Type get_Category()  { return m_eqn->get_Category(); };

      // return the parents
      inline  SymbolPtrElemSizeTMap& getParentsSymbols()  { return m_parentssymbols; };
      inline  NodePtrSet& getParents()  { return m_parents; };

      // return childs
      inline  NodePtrSet& getChilds()  { return m_childs; } ;

      // add parents
      void addParent(NodePtr p);
      // remove parent
      void removeParent(NodePtr p);

      // add child
      void addChild(NodePtr c);
      // remove child
      void removeChild(NodePtr c);
      // clear
      inline void clearChilds() { m_childs.clear(); };

      // visited
      bool visited;

      void simplify();
      bool m_subsed;

      void findParentsNodes();

      void clear();

      // subs
      void subs(ConstBasicPtr const& old_exp, BasicPtr const& new_exp);

      class Visitor
      {
      public:
        virtual void process_Node(NodePtr p) = 0;
      protected:
        Visitor() {};
        virtual ~Visitor() {};
      };

      static void node_iterate_depth_first(NodePtr p, Visitor &v);

      inline std::string toString() { return m_eqn->toString();};

      inline EquationPtr getEqn() { return m_eqn; };

    protected:
      SymbolPtr m_time;
      // Equation
      EquationPtr m_eqn;
      // ParentSymbols
      SymbolPtrElemSizeTMap m_parentssymbols;
      // Parents
      NodePtrSet m_parents;

      // Childs
      NodePtrSet m_childs;

      void findParentsSymbols();

      void setSolveFor(SymbolPtrElemMap& solveFor);

    private:
      // Boost Intrusive Ptr
      unsigned int m_refCount;
      // friend
      friend void ::boost::intrusive_ptr_add_ref( const Node* p);
      friend void ::boost::intrusive_ptr_release( const Node* p);
    };
    /*****************************************************************************/
  };

};

/*****************************************************************************/
namespace boost
{
  // increment reference count of object *p
  inline void intrusive_ptr_add_ref( const Symbolics::Graph::Node* p)
  {
    // Muss sein, da ich sonst keine  Basics in intrusive Ptr legen kann
    Symbolics::Graph::Node* b = const_cast<Symbolics::Graph::Node*>(p);
    ++(b->m_refCount);
  }

  // decrement reference count, and delete object when reference count reaches 0
  inline void intrusive_ptr_release( const Symbolics::Graph::Node* p)
  {
    // Muss sein, da ich sonst keine  Basics in intrusive Ptr legen kann
    Symbolics::Graph::Node* b = const_cast<Symbolics::Graph::Node*>(p);
    if (--(b->m_refCount) == 0)
      delete p;
  } 
}; // namespace boost
/*****************************************************************************/
#endif // __GRAPH_NODE_H_
