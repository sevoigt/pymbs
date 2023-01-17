#ifndef __UNMATCHEDSYSTEM_H_
#define __UNMATCHEDSYSTEM_H_

#include <string>
#include <map>
#include <list>
#include "Symbolics.h"
#include "SystemHandler.h"

namespace Symbolics
{
  namespace Graph
  {
  /*****************************************************************************/
    class UnMatchedSystem: public SystemHandler
    {
    public:
      // Konstruktor
      UnMatchedSystem(EquationSystemPtr eqsys, NodeVec& nodes);
      // Destruktor
      ~UnMatchedSystem();

      // 
      double buildGraph();

      // to GraphML
      void toGraphML( std::string file );
      void toGraphML_Sorted( std::string file );

      void dumpComps( std::string file );

    protected:

      struct MSymbol
      {
        SymbolPtr symbol;
        size_t eqn;
        size_t dim1;
        size_t dim2;
        bool state;
        bool parameter;
        MSymbol(SymbolPtr s, size_t d1, size_t d2): symbol(s), eqn(0),dim1(d1),dim2(d2), state(false), parameter(false) {;};
      };
      typedef MSymbol* MSymbolPtr;
      typedef std::set<MSymbolPtr> MSymbolPtrSet;
      typedef std::vector<MSymbolPtr> MSymbolPtrVec;

      struct MEquation
      {
        EquationPtr eqn;
        BasicPtr lhs;
        BasicPtr rhs;
        size_t dim1;
        size_t dim2;
        MSymbolPtrSet symbols;
        MSymbolPtrSet states;
        size_t number;
        size_t lowlink;
        MSymbol *symbol;
        bool onStack;
        double t;
        size_t c;
        size_t c1;
        MEquation(EquationPtr e, size_t d1, size_t d2): eqn(e),dim1(d1),dim2(d2),number(0),lowlink(0),symbol(NULL),onStack(false),t(0.0),c(0),c1(0) {;}; 
      };
      typedef std::vector<MEquation*> MEquationPtrVec;
      MEquationPtrVec m_equations;
      typedef std::map<EquationPtr, MEquationPtrVec > EquationPtrMEquationMap;
      EquationPtrMEquationMap m_eqnptrmeqns;
      typedef std::set<MEquation*> MEquationPtrSet;
      typedef std::vector<MEquationPtrVec> MEquationPtrVecVec;
      MEquationPtrVecVec m_comps;
      typedef std::map<EquationPtr, MEquationPtrSet > EquationPtrMEquationSetMap;

      typedef std::map<EquationPtr,EquationPtr> EquationPtrMap;
      typedef std::map<EquationPtr,SizeTPairSet > EquationPtrSizeTSet;
      typedef std::map<SymbolPtr,SizeTPairSet > SymbolPtrSizeTSet;

      typedef std::map<MSymbol*,MEquationPtrVec> IncidenceMatrixT;
      IncidenceMatrixT m_incidencematrixT;
      MSymbolPtrSet m_states;
      MSymbolPtrSet m_parameter;

      std::map<BasicPtr,BasicPtr > symbolreplacemap;

      typedef std::map<MSymbol*,size_t> SymbolEqnMap;
      typedef std::map<size_t,MSymbol*> EqnSymbolMap;

      double matchSystem();

      typedef std::set<size_t> SizeTSet;
      bool pathFound(size_t e, SizeTSet &eqn_mark, MSymbolPtrSet &sym_mark,size_t &c);

      typedef std::vector<size_t> SizeTVec;
      typedef std::list<size_t> SizeTList;
      void strongConnect(size_t &i, MEquation* eqn, MEquationPtrVec &stack);

      void buildNodes();

      void addSymbolToIncidenceMatrix(SymbolPtr s);

      void addEquationToIncidenceMatrix(MEquation* wrapper);

      class DerRepl: public Basic::Iterator
      {
      public:
          DerRepl() {;}
          ~DerRepl() {;}

          /* Keep replaced Symbols for speed up */
          typedef std::pair<BasicPtr, SymbolElementKind> BasicPtrKindPair;
          std::map<SymbolPtr, BasicPtrKindPair > dummy_der;
          /* new Symbols for the equation system */
          SymbolPtrSet newsyms;

          BasicPtr process_Arg(BasicPtr const &p);
      };

      class SymbolRepl: public Basic::Iterator
      {
      public:
          SymbolRepl(BasicPtrMap &symbolmap):m_symbolmap(symbolmap) {;}
          ~SymbolRepl() {;}
          
          BasicPtrMap &m_symbolmap;

          BasicPtr process_Arg(BasicPtr const &p);
      };

      class SymbolScanner: public Basic::Scanner
      {
      public:
          SymbolScanner(MEquation* equation,
            BasicPtr time, IncidenceMatrixT &incidencematrixT):
            m_equation(equation), m_time(time),
            m_incidencematrixT(incidencematrixT),inder(false) {;}
          ~SymbolScanner() {;}
          bool process_Arg(BasicPtr const &p, bool &stop);

      protected:
          bool inder;
          MEquation* m_equation;
          BasicPtr m_time;
          IncidenceMatrixT &m_incidencematrixT;
      };
    private:
      SymbolPtr m_time;

    };
  };
/*****************************************************************************/
};


#endif // __UNMATCHEDSYSTEM_H_