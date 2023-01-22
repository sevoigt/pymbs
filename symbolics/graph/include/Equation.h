#ifndef __GRAPH_EQUATION_H_
#define __GRAPH_EQUATION_H_

#include <string>

#include "Symbolics.h"

#include "intrusive_ptr.h"

// Forward Declarations
namespace Symbolics
{
  namespace Graph
  {
    class Equation;
  };
};

/*****************************************************************************/
namespace boost
{
  void intrusive_ptr_add_ref( const Symbolics::Graph::Equation *p);
  void intrusive_ptr_release( const Symbolics::Graph::Equation *p);
};
/*****************************************************************************/

namespace Symbolics
{
  namespace Graph
  {
    /*****************************************************************************/
    typedef unsigned int Category_Type;
    /*****************************************************************************/
    typedef std::pair<size_t,size_t> SizeTPair;
    typedef std::set<SizeTPair> SizeTPairSet;

    typedef struct SymbolElementT {
      SymbolElementKind kind;
      SizeTPairSet elements;
      SymbolElementT(): kind(ALL) {;};
      SymbolElementT(size_t dim1, size_t dim2): kind(SEVERAL) { elements.insert(SizeTPair(dim1,dim2));};
    } SymbolElement;

    typedef std::map< SymbolPtr , SymbolElement > SymbolPtrElemMap;

    typedef std::map<SizeTPair,size_t> SizeTPairSizeTMap;
    typedef struct SymbolElementSizeTT {
      SymbolElementKind kind;
      size_t counter;
      SizeTPairSizeTMap elements;
      SymbolElementSizeTT(): kind(ALL), counter(1) {;};
      SymbolElementSizeTT(size_t dim1, size_t dim2): kind(SEVERAL), counter(0) { elements[SizeTPair(dim1,dim2)]=1;};
    } SymbolElementSizeT;

    typedef std::map< SymbolPtr, SymbolElementSizeT > SymbolPtrElemSizeTMap;

    typedef boost::intrusive_ptr<const Equation> ConstEquationPtr;
    typedef boost::intrusive_ptr<Equation> EquationPtr;
    typedef std::set<EquationPtr> EquationPtrSet;
    typedef std::vector<EquationPtr> EquationPtrVec;

    /*****************************************************************************/
    class Equation
    {
    public:
      // Konstruktor
      Equation(SymbolPtrElemMap const& SolveFor, BasicPtr const& Lhs,  BasicPtr const& Rhs, bool implizit);
      Equation(SymbolPtrElemMap const& SolveFor, BasicPtrVec const& Lhs, BasicPtrVec const& Rhs, bool implizit);
      // Destruktor
      virtual ~Equation();

      // returns the symbols
      inline  SymbolPtrElemSizeTMap& getSymbols()  {return m_symbols; };
      inline  SymbolPtrElemMap& getSolveFor()  {return m_solveFor; };

      // returns the expressions
      inline size_t getLhsSize() { return m_lhs.size(); };
      inline  BasicPtr const& getLhs(size_t i)  { return m_lhs[i].getArg(); };
      inline size_t getRhsSize() { return m_rhs.size(); };
      inline  BasicPtr const& getRhs(size_t i)  { return m_rhs[i].getArg(); };

      // is_Implicit
      inline  bool is_Implicit()  { return m_implizit; };

      // Category
      inline  Category_Type get_Category()  { return m_category; };

      // subs
      void subs(ConstBasicPtr const& old_exp, BasicPtr const& new_exp);

      void simplify();

      void findSymbols();

      void solve(BasicPtr const& exp);

      std::string toString();

      // Gesamtgroesse
      inline size_t getNumEl() const { return m_numElem; };

      // differentiate eqn
      EquationPtr diff();

      void iterateEquation(Basic::Iterator &v);

    protected:

      // implizit
      bool m_implizit;
      // Symbol
      SymbolPtrElemSizeTMap m_symbols;
      // SolveFor
      SymbolPtrElemMap m_solveFor;
      // Expression
      ArgumentPtrVec m_lhs;
      ArgumentPtrVec m_rhs;
      // Category
      Category_Type m_category;
      // Number of Elements
      size_t m_numElem;

      void setSolveFor(SymbolPtrElemMap const& SolveFor);

      class DerivativeScanner: public Basic::Scanner
      {
      public:
        DerivativeScanner(SymbolPtrElemSizeTMap &symbols): atoms(symbols),inder(false),inelement(false) {;}
        ~DerivativeScanner() {;}

        bool process_Arg(BasicPtr const &p, bool &stop);

        SymbolPtrElemSizeTMap &atoms;

      private:
        bool inder;
        bool inelement;
        size_t dim1;
        size_t dim2;
      };

      class SymbolScanner: public Basic::Scanner
      {
      public:
        SymbolScanner(SymbolPtrElemMap &symbols, bool errorIfMoreThanOnce=false): 
            atoms(symbols),inder(false),errorifmorethanonce(errorIfMoreThanOnce),inelement(false),dim1(0),dim2(0) {;}
            ~SymbolScanner() {;}

            bool process_Arg(BasicPtr const &p, bool &stop);

            SymbolPtrElemMap &atoms;

      private:
        bool inder;
        bool errorifmorethanonce;
        bool inelement;
        size_t dim1;
        size_t dim2;

      };

    public:
      class DerRepl: public Basic::Iterator
      {
      public:
        DerRepl() {;}
        ~DerRepl() {;}

        BasicPtr process_Arg(BasicPtr const &p);
      };

    private:
      // Boost Intrusive Ptr
      unsigned int m_refCount;
      // friend
      friend void ::boost::intrusive_ptr_add_ref( const Equation* p);
      friend void ::boost::intrusive_ptr_release( const Equation* p);
    };
    /*****************************************************************************/
  };

};

/*****************************************************************************/
namespace boost
{
  // increment reference count of object *p
  inline void intrusive_ptr_add_ref( const Symbolics::Graph::Equation* p)
  {
    // Muss sein, da ich sonst keine  Basics in intrusive Ptr legen kann
    Symbolics::Graph::Equation* b = const_cast<Symbolics::Graph::Equation*>(p);
    ++(b->m_refCount);
  }

  // decrement reference count, and delete object when reference count reaches 0
  inline void intrusive_ptr_release( const Symbolics::Graph::Equation* p)
  {
    // Muss sein, da ich sonst keine  Basics in intrusive Ptr legen kann
    Symbolics::Graph::Equation* b = const_cast<Symbolics::Graph::Equation*>(p);
    if (--(b->m_refCount) == 0)
      delete p;
  } 
}; // namespace boost
/*****************************************************************************/
#endif // __GRAPH_EQUATION_H_
