#include "Basic.h"
#include "Factory.h"

using namespace Symbolics;

/*****************************************************************************/
Basic::Basic( Basic_Type const& type):
m_type(type), m_refCount(0),m_simplified(false)
/*****************************************************************************/
{
    calcHash();
}
/*****************************************************************************/


/*****************************************************************************/
Basic::Basic( Basic_Type const& type,  Shape const& shape):
m_type(type), m_shape(shape), m_refCount(0),m_simplified(false)
/*****************************************************************************/
{
    calcHash();
}
/*****************************************************************************/


/*****************************************************************************/
Basic::~Basic()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
bool Basic::operator !=( Symbolics::Basic const& rhs) const
/*****************************************************************************/
{
    return !( *this == rhs );
}
/*****************************************************************************/


/*****************************************************************************/
Basic::BasicSet Basic::getAtoms() 
/*****************************************************************************/
{
    BasicSet atoms;
    getAtoms(atoms);
    return atoms;
}
/*****************************************************************************/


/*****************************************************************************/
Basic::BasicSizeTMap Basic::getAtomsAmound() 
/*****************************************************************************/
{
    BasicSizeTMap atoms;
    getAtoms(atoms);
    return atoms;
}
/*****************************************************************************/


/*****************************************************************************/
 BasicPtr Basic::subs( ConstBasicPtr const& old_exp, 
                       BasicPtr const& new_exp)
/*****************************************************************************/
{
    if (*this == *old_exp.get())
        return new_exp;

    // subs args
    BasicPtrVec subs_args;
    subs_args.reserve( getArgsSize() );
    for (size_t i=0; i<getArgsSize(); ++i)
        subs_args.push_back(getArg(i)->subs(old_exp,new_exp));

    return Factory::newBasic( getType(), subs_args, getShape() );
}
/*****************************************************************************/

/*****************************************************************************/
 void Basic::subs( BasicPtr const& new_exp)
/*****************************************************************************/
{
    // brauchen eine Kopie da setArg m_parents veraendert
    ArgumentPtrSet parents = m_parents;
    // subs me
    for (ArgumentPtrSet::iterator ii = parents.begin();ii!=parents.end();++ii)
    {
      (*ii)->setArg(new_exp);
      if ((*ii)->getParent().get() != NULL)
        (*ii)->getParent()->changed();
    }
    // no parents left
    clearParents();
}
/*****************************************************************************/

/*****************************************************************************/
 void Basic::changed()
/*****************************************************************************/
{
  //if (m_simplified)
  {
    m_simplified = false;
    for (ArgumentPtrSet::iterator ii = m_parents.begin();ii!=m_parents.end();++ii)
    {
      if ((*ii)->getParent().get() != NULL)
        (*ii)->getParent()->changed();
    }
  }
}
/*****************************************************************************/

/*****************************************************************************/
 BasicPtr Basic::iterateExp(Symbolics::Basic::Iterator &v)
/*****************************************************************************/
{
    // iterate args
    BasicPtrVec new_args;
    new_args.reserve( getArgsSize() );
    for (size_t i=0; i<getArgsSize(); ++i)
        new_args.push_back(getArg(i)->iterateExp(v));

    BasicPtr newI = Factory::newBasic( getType(), new_args, getShape() );
    return v.process_Arg(newI);
}
/*****************************************************************************/


/*****************************************************************************/
void Basic::scanExp(Symbolics::Basic::Scanner &s)
/*****************************************************************************/
{
    bool stop = false;
    if (!s.process_Arg(BasicPtr(this),stop))
      return;
    if (stop)
      return;
    // iterate args
    for (size_t i=0; i<getArgsSize(); ++i)
      getArg(i)->scanExp(s);
}
/*****************************************************************************/

/*****************************************************************************/
void Basic::calcHash()
/*****************************************************************************/
{
    m_hash = 0;
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Basic::solve(BasicPtr exp)
/*****************************************************************************/
{
    // solve equation "this=0 " for exp
    if (this == exp)
    {
        return Zero::getZero(m_shape);
    }

    if (exp->getType() == Type_Symbol)
    {
      BasicPtr m_der = der(exp)->simplify();
      if (Util::is_Zero(m_der))
      {
          return Zero::getZero(m_shape);
      }

      LookFor lookfor(exp);
      m_der->scanExp(lookfor);
      if (lookfor.found)
      {
          return BasicPtr();
      }
      return Neg::New(Util::div(subs(exp,Zero::getZero(exp->getShape())),m_der));
    }
    else
    {
      SymbolPtr tmp(new Symbol("__tmp__solve__",exp->getShape()));
      BasicPtr tmpexp = subs(exp,tmp);
      BasicPtr solve = tmpexp->solve(tmp);
      if (solve.get() == NULL)
      {
        return BasicPtr();
      }
      else
      {
        // check if a symbol from exp is in solve
        BasicSet atoms = exp->getAtoms();
        LookForMultiple lookfor(atoms);
        solve->scanExp(lookfor);
        if (lookfor.found)
          return BasicPtr();
        return solve;
      }
    }
}
/*****************************************************************************/

/*****************************************************************************/
bool Basic::LookFor::process_Arg(BasicPtr const &p, bool &stop)
/*****************************************************************************/
{
    if (found)
    {
        stop = true;
        return false;
    }
    if (p==m_lookfor)
    {
        found = true;
        stop = true;
        return false;
    }
    return true;
}
/*****************************************************************************/

/*****************************************************************************/
bool Basic::LookForMultiple::process_Arg(BasicPtr const &p, bool &stop)
/*****************************************************************************/
{
    if (found)
    {
        stop = true;
        return false;
    }
    if (p->getType() == Type_Symbol)
    {
      if (m_lookfor.find(p) != m_lookfor.end())
      {
          found = true;
          stop = true;
          return false;
      }
    }
    return true;
}
/*****************************************************************************/