#ifndef __PREOPTIMISATION_H_
#define __PREOPTIMISATION_H_

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
        class PreOptimisation
        {
        public:
            // Konstruktor
            PreOptimisation(EquationSystemPtr eqsys);
            // Destruktor
            ~PreOptimisation();

            // run Optimization
            void optimize();

        protected:

          EquationSystemPtr m_eqsys;

        private:

            template< typename Ty >
            void optimizeUnaryFunctions(Basic_Type type )
            {
              std::map<BasicPtr,size_t> repl;
              std::map<BasicPtr,size_t>::iterator ii;
              // 
              ScopePtr scope = Ty::getScope();
              List *list = scope->first;
              while (list != NULL)
              {
                // cast
                Ty *unary = List::getAsPtr<Ty>(list);
                BasicPtr p(unary);
                ii = repl.find(p);
                if (ii != repl.end())
                {
                  ii->second = ii->second + p->getParentsSize();
                  p->subs(ii->first);
                }
                else
                {
                  repl[p] = p->getParentsSize();
                }
                // next
                list = list->getRigth();
              }

              std::map<BasicPtr,size_t>::iterator re = repl.end();
              size_t n=0;
              for (ii =repl.begin(); ii != re;++ii)
              {
                if (ii->second > 1)
                {
                  std::string name = Basic_TypetoStr(type);
                  if (ii->first->getArg(0)->getType() == Type_Symbol)
                    name += ii->first->getArg(0)->toString();
                  else
                    name += str(n++);
                  // check for free name
                  if (m_eqsys->hasSymbol(name))
                    name += str(n++);
                  SymbolPtr sp(new Symbol(name,ii->first->getShape()));
                  m_eqsys->addSymbol(sp);
                  ii->first->subs(sp);
                  m_eqsys->addEquation(sp,ii->first,false);
                }
              }
            };
            std::string& Basic_TypetoStr(Basic_Type const& t);
        };
/*****************************************************************************/
    };
};

#endif // __MATCHEDSYSTEM_H_