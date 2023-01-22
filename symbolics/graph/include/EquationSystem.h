#ifndef __GRAPH_EQUATIONSYSTEM_H_
#define __GRAPH_EQUATIONSYSTEM_H_

#include <string>
#include <map>
#include <list>

#include "Symbolics.h"
#include "Equation.h"

// Forward Declarations
namespace Symbolics
{
  namespace Graph
  {
    class EquationSystem;
  };
};

/*****************************************************************************/
namespace boost
{
  void intrusive_ptr_add_ref( const Symbolics::Graph::EquationSystem *p);
  void intrusive_ptr_release( const Symbolics::Graph::EquationSystem *p);
};
/*****************************************************************************/

namespace Symbolics
{
  namespace Graph
  {
/*****************************************************************************/
    // SymbolAlreadyExistsError
    class SymbolAlreadyExistsError: public Exception
    {
    public:
      SymbolAlreadyExistsError(  const char *  &msg ): Exception(msg) {};
      SymbolAlreadyExistsError(  const std::string &msg ): Exception(msg.c_str()) {};
    };
    // VarUnknownError
    class VarUnknownError: public Exception
    {
    public:
      VarUnknownError(  const char *  &msg ): Exception(msg) {};
      VarUnknownError(  const std::string &msg ): Exception(msg.c_str()) {};
    };        
    // SymbolKindError
    class SymbolKindError: public Exception
    {
    public:
      SymbolKindError(  const char *  &msg ): Exception(msg) {};
      SymbolKindError(  const std::string &msg ): Exception(msg.c_str()) {};
    };
    // ExpressionSolveForError
    class ExpressionSolveForError: public Exception
    {
    public:
      ExpressionSolveForError(  const char *  &msg ): Exception(msg) {};
      ExpressionSolveForError(  const std::string &msg ): Exception(msg.c_str()) {};
    };
    // SymbolAlreadySolvedInError
    class SymbolAlreadySolvedInError: public Exception
    {
    public:
      SymbolAlreadySolvedInError(  const char *  &msg ): Exception(msg) {};
      SymbolAlreadySolvedInError(  const std::string &msg ): Exception(msg.c_str()) {};
    };
    // InitalValueError
    class InitalValueError: public Exception
    {
    public:
      InitalValueError(  const char *  &msg ): Exception(msg) {};
      InitalValueError(  const std::string &msg ): Exception(msg.c_str()) {};
    };

/*****************************************************************************/

    typedef boost::intrusive_ptr<const EquationSystem> ConstEquationSystemPtr;
    typedef boost::intrusive_ptr<EquationSystem> EquationSystemPtr;
    typedef std::set<EquationSystemPtr> EquationSystemPtrSet;

/*****************************************************************************/
    class EquationSystem
    {
    public:
      // Konstruktor
      EquationSystem();
      // Destruktor
      ~EquationSystem();


      // add Symbol
      // throws: SymbolAlreadyExistsError, SymbolKindError,InitalValueError
      BasicPtr addSymbol( SymbolPtr const& s,  Basic *initalValue = NULL, std::string comment = "");

      // add Expression : category wird als Bitfolge implementiert 1:an 0:aus 
      // throws: ExpressionSolveForError, SymbolAlreadySolvedInError, VarUnknownError
      void addEquation( BasicPtr const& solveFor, BasicPtr const& exp, bool implicit=false);

      // add Expression
      // throws: ExpressionSolveForError, SymbolAlreadySolvedInError, VarUnknownError
      void addEquation( BasicPtrVec const& solveFor, BasicPtrVec const& exp, bool implicit=false);

      void addEquation( EquationPtr eqn);

      void eraseEquation( EquationPtr eqn);

      // scalar
      EquationSystemPtr makeScalar();

      // getSymbol
      // throws: VarUnknownError
      SymbolPtr getSymbol( std::string const& name);
      inline bool hasSymbol( std::string const& name) { return m_SymbolMap.find(name) != m_SymbolMap.end(); };

      // getVariables
      SymbolPtrVec getVariables(Symbol_Kind kind);

      // initalValue
      BasicPtr getinitVal(SymbolPtr const& s);

      // equation of symbol, does only work for equations with implicit=false and if the hole symbol is solved by the equation 
      BasicPtr getEquation(SymbolPtr const& s);

      // comment
      std::string getComment(SymbolPtr const& s);

      inline bool is_Balanced() { return m_balanced; };

      inline EquationPtrSet getEquations() { return m_equations; };

      typedef std::map< std::string,SymbolPtr> StringSymbolMap;
      inline StringSymbolMap& getSymbols() { return m_SymbolMap; };

      inline SymbolPtrElemMap& getStates() { return m_states; }; 
      inline SymbolPtrElemMap& getDerStates() { return m_derstates; }; 

      void setVariable(SymbolPtr s, size_t dim1, size_t dim2);

      EquationPtr combineEquations(EquationPtrVec equations);

      void dumpSystem(std::string file);

    protected:

      bool m_balanced;

      // [Symbol] = Equation
      typedef std::map<SizeTPair,EquationPtr> SizeTPairEquationPtrMap;
      typedef struct SymbolEquationElementT {
        SymbolElementKind kind;
        EquationPtr eqn;
        SizeTPairEquationPtrMap elements;
        SymbolEquationElementT(): kind(SEVERAL) {;};
        SymbolEquationElementT(EquationPtr e): kind(ALL), eqn(e) {;};
        SymbolEquationElementT(size_t dim1, size_t dim2, EquationPtr e): kind(SEVERAL) { elements[SizeTPair(dim1,dim2)]=e;};
      } SymbolEquationElement;
      typedef std::map<SymbolPtr,SymbolEquationElement> SymbolEquationMap;
      SymbolEquationMap m_symbolsequationmap;
      // ["Symbol"] = Symbol
      StringSymbolMap m_SymbolMap;
      // [Symbol] = startvalue
      typedef std::map<SymbolPtr,BasicPtr> SymbolStartValueMap;
      SymbolStartValueMap m_symbolsvaluemap;
      // [Symbol] = comment
      typedef std::map<SymbolPtr,std::string> SymbolCommentMap;
      SymbolCommentMap m_symbolscommentmap;

      // Equation
      EquationPtrSet m_equations;
      // States
      SymbolPtrElemMap m_states;
      SymbolPtrElemMap m_derstates;

      // parameter
      SymbolPtrSet m_parameter;
      // inputs
      SymbolPtrSet m_inputs;
      // controllers
      SymbolPtrSet m_controller;
      // variables
      SymbolPtrElemMap m_variables;
    protected:
      // constants
      SymbolPtrSet m_constants;

      BasicPtr addSymbol_Internal( SymbolPtr const& s, Basic *initialValue, std::string comment);

      void getSymbolOrDer( SymbolPtrElemMap &res, BasicPtr const& exp, bool inMatrix=false,
        bool inDer=false, bool inElement=false, size_t dim1=0, size_t dim2=0);

      void assignEqnstoSymbols(SymbolPtrElemMap &symbols, EquationPtr eqn);


      class InitValueScanner: public Basic::Scanner
      {
      public:
        InitValueScanner(): variable(false) {;}
        ~InitValueScanner() {;}
                
        bool process_Arg(BasicPtr const &p, bool &stop);

        bool variable;
      };

      class DerivativeScanner: public Basic::Scanner
      {
      public:
          DerivativeScanner():inder(false),inelement(false),dim1(0),dim2(0) {;}
          ~DerivativeScanner() {;}
                
          bool process_Arg(BasicPtr const &p, bool &stop);

          SymbolPtrElemMap atoms;
          SymbolPtrElemMap states;

      private:
          bool inder;
          bool inelement;
          size_t dim1;
          size_t dim2;

          void process_derArg(BasicPtr const &p);
          void addAtom(SymbolPtr sp);
      };

      class SymbolRepl: public Basic::Iterator
      {
      public:
          SymbolRepl(BasicPtrMap &symbolmap):m_symbolmap(symbolmap) {;}
          ~SymbolRepl() {;}
          
          BasicPtrMap &m_symbolmap;

          BasicPtr process_Arg(BasicPtr const &p);
      };

      void checkEquation(DerivativeScanner *scanner,
        BasicPtr const& exp, BasicPtrVec const& expvec, bool implicit, SymbolPtrElemMap &symbols);

    private:
      // Boost Intrusive Ptr
      unsigned int m_refCount;
      // friend
      friend void ::boost::intrusive_ptr_add_ref( const EquationSystem* p);
      friend void ::boost::intrusive_ptr_release( const EquationSystem* p);

    };
/*****************************************************************************/

  };
};

/*****************************************************************************/
namespace boost
{
  // increment reference count of object *p
  inline void intrusive_ptr_add_ref( const Symbolics::Graph::EquationSystem* p)
  {
    // Muss sein, da ich sonst keine  Basics in intrusive Ptr legen kann
    Symbolics::Graph::EquationSystem* b = const_cast<Symbolics::Graph::EquationSystem*>(p);
    ++(b->m_refCount);
  }

  // decrement reference count, and delete object when reference count reaches 0
  inline void intrusive_ptr_release( const Symbolics::Graph::EquationSystem* p)
  {
    // Muss sein, da ich sonst keine  Basics in intrusive Ptr legen kann
    Symbolics::Graph::EquationSystem* b = const_cast<Symbolics::Graph::EquationSystem*>(p);
    if (--(b->m_refCount) == 0)
      delete p;
  } 
}; // namespace boost
/*****************************************************************************/

#endif // __GRAPH_EQUATIONSYSTEM_H_
