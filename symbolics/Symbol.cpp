#include "Symbol.h"
#include "Zero.h"
#include "Der.h"
#include "Util.h"
#include "Int.h"
#include "Matrix.h"
#include "Element.h"
#include <stdlib.h>

using namespace Symbolics;


/*****************************************************************************/
#ifdef _DEBUG
std::set<std::string> Symbol::symbol_names;
#endif
BasicPtr Symbol::getArgres;
/*****************************************************************************/

/*****************************************************************************/
Symbol::Symbol( std::string const& name, 
                Shape const& shape, 
                Symbol_Kind category):
Basic(Type_Symbol,shape), m_name(name), m_kind(category)
/*****************************************************************************/
{
  m_statekind=ALL;
  m_state = static_cast<size_t*>(calloc(m_shape.getNumEl(),sizeof(size_t)));
  m_state[0] = NO_STATE;
#ifdef _DEBUG
    if (symbol_names.find(name) != symbol_names.end())
        throw InternalError("Symbol " + name + " constructed twice!");
    symbol_names.insert(name);
#endif
    calcHash();
}
/*****************************************************************************/


/*****************************************************************************/
Symbol::Symbol( std::string const& name,  Symbol_Kind category):
Basic(Type_Symbol), m_name(name), m_kind(category)
/*****************************************************************************/
{
  m_statekind=ALL;
  m_state = static_cast<size_t*>(calloc(m_shape.getNumEl(),sizeof(size_t)));
  m_state[0] = NO_STATE;
#ifdef _DEBUG
    if (symbol_names.find(name) != symbol_names.end())
        throw InternalError("Symbol " + name + " constructed twice!");
    symbol_names.insert(name);
#endif
    calcHash();
}
/*****************************************************************************/


/*****************************************************************************/
Symbol::~Symbol()
/*****************************************************************************/
{
  if (m_state)
    free (m_state);
#ifdef _DEBUG
    if (symbol_names.find(m_name) != symbol_names.end())
    {
        symbol_names.erase(m_name);
    }
#endif
}
/*****************************************************************************/


/*****************************************************************************/
std::string Symbol::toString() const
/*****************************************************************************/
{
#ifdef _DEBUG
    return "Symbol(" + m_name + ")";
#else
    return m_name;
#endif
}
/*****************************************************************************/

/*****************************************************************************/
bool Symbol::operator==(  Basic const& rhs ) const
/*****************************************************************************/
{
    // Versuch zu casten
    const Symbol *symbol = dynamic_cast<const Symbol*>(&rhs);
    if (symbol == NULL) return false;

    return getName() == symbol->getName();
}
/*****************************************************************************/


/*****************************************************************************/
 BasicPtr Symbol::subs( ConstBasicPtr const& old_exp, 
                        BasicPtr const& new_exp)
/*****************************************************************************/
{
    if (*this == *old_exp.get())
        return new_exp;
    return BasicPtr(this);
}
/*****************************************************************************/

/*****************************************************************************/
void Symbol::setState()
/*****************************************************************************/
{
  for (size_t i=0;i<m_shape.getNumEl();i++)
    m_state[i] = STATE | m_state[i];
  m_statekind = ALL;
}
/*****************************************************************************/

/*****************************************************************************/
void Symbol::setState(size_t dim1, size_t dim2)
/*****************************************************************************/
{
  if (m_statekind == ALL)
  {
    if (m_state[0] & STATE)
      return;
    if (m_shape.getNumEl() > 1)
      m_statekind=SEVERAL;
    m_statekind = SEVERAL;
    for (size_t i=1;i<m_shape.getNumEl();i++)
      m_state[i] = m_state[0];
  }
  size_t indx = getIndex(dim1,dim2);
  m_state[indx] = STATE | m_state[indx];
  for (size_t i=1;i<m_shape.getNumEl();i++)
    if (!(m_state[i] & STATE))
      return;
  m_statekind = ALL;
}
/*****************************************************************************/

/*****************************************************************************/
void Symbol::setVariable(size_t dim1, size_t dim2)
/*****************************************************************************/
{
  if (m_statekind == ALL)
  {
    if (m_state[0] & STATE)
    {
      for (size_t i=1;i<m_shape.getNumEl();i++)
        m_state[i] = m_state[0];
      m_state[getIndex(dim1,dim2)] = DUMMY_STATE | (m_state[0]^STATE);
      if (m_shape.getNumEl() > 1)
        m_statekind=SEVERAL;
    }
  }
  else
  {
    m_state[getIndex(dim1,dim2)] = DUMMY_STATE | (m_state[getIndex(dim1,dim2)]^STATE);
    bool allequal=true;
    for (size_t i=1;i<m_shape.getNumEl();i++)
      if (m_state[i] & STATE)
        allequal = false;
    if (allequal)
      m_statekind = ALL;
  }
  SymbolPtr s(getDerivative());
  if (s->m_statekind == ALL)
  {
    if (s->m_state[0] & DER_STATE)
    {
      for (size_t i=1;i<s->m_shape.getNumEl();i++)
        s->m_state[i] = s->m_state[0];
      s->m_state[getIndex(dim1,dim2)] = s->m_state[0]^DER_STATE;
      if (s->m_shape.getNumEl() > 1)
        s->m_statekind=SEVERAL;
    }
  }
  else
  {
    s->m_state[getIndex(dim1,dim2)] = s->m_state[getIndex(dim1,dim2)]^DER_STATE;
    bool allequal=true;
    for (size_t i=1;i<s->m_shape.getNumEl();i++)
      if (s->m_state[i] & DER_STATE)
        allequal = false;
    if (allequal)
      s->m_statekind = ALL;
  }
  BasicPtrVec subparents;
  BasicPtrVec esubparents;
  for (ArgumentPtrSet::iterator ii = m_parents.begin();ii!=m_parents.end();++ii)
  {
    if ((*ii)->getParent().get() != NULL)
    {
      if ((*ii)->getParent()->getType() == Type_Der)
      {
        subparents.push_back((*ii)->getParent());
      }
      else if ((*ii)->getParent()->getType() == Type_Element)
      {
        Element *elem = Util::getAsPtr<Element>((*ii)->getParent());
        if ((elem->getRow() == dim1) && (elem->getCol() == dim2))
        {
          ArgumentPtrSet& epars = elem->getParents();
         for (ArgumentPtrSet::const_iterator jj = epars.begin();jj!=epars.end();jj++)
          {
            if ((*jj)->getParent().get() != NULL)
            {
              if ((*jj)->getParent()->getType() == Type_Der)
              {
                esubparents.push_back((*jj)->getParent());
              }
            }
          } 
        }
      }
    }
  }
  BasicPtr news;
  if (m_statekind == ALL)
  {
    news = BasicPtr(s);
  }
  else
  {
    Matrix *mat = new Matrix(m_shape);
    size_t dim1 = m_shape.getDimension(1);
    size_t dim2 = m_shape.getDimension(2);
    for(size_t i=0;i<dim1;++i)
    {
      for(size_t j=0;j<dim2;++j)
      {
        if (is_State(i,j) & STATE)
          mat->set(i,j,Der::New(Element::New(BasicPtr(this),i,j)));
        else
        {
          mat->set(i,j, Element::New(BasicPtr(s),i,j));
        }
      }
    }
    news = BasicPtr(mat);
  }
  if (subparents.size()>0)
  {
    for (size_t i=0;i<subparents.size();i++)
    {
      subparents[i]->subs(news);
    }
  }
  if (esubparents.size()>0)
  {
    BasicPtr es = Element::New(news,dim1,dim2);
    for (size_t i=0;i<esubparents.size();i++)
    {
      esubparents[i]->subs(es);
    }
  }
}
/*****************************************************************************/

/*****************************************************************************/
 BasicPtr Symbol::iterateExp(Iterator &v)
/*****************************************************************************/
{
    return BasicPtr(v.process_Arg(BasicPtr(this)));
}
/*****************************************************************************/

/*****************************************************************************/
bool Symbol::operator<(Basic  const& rhs) const
/*****************************************************************************/
{
    // Typen vergleichen
    if (getType() != rhs.getType())
        return getType() < rhs.getType();

    // Vergleich Shape
    if (getShape() != rhs.getShape())
        return (getShape() < rhs.getShape());

    // Symbol
    const Symbol &symbol = dynamic_cast<const Symbol &>(rhs);

    // hash vergleichen - vorerst wieder rausgenommen, da es nicht sinnvoll ist
//    if (m_hash != symbol.getHash())
//        return m_hash < symbol.getHash();

    // Name vergleichen
    return m_name < symbol.m_name;
}
/*****************************************************************************/

/*****************************************************************************/
void Symbol::calcHash()
/*****************************************************************************/
{
    // simple hashing algorithm; taken from the Dragon Book
    size_t hash = 0;
    for(size_t i = 0; i < m_name.length(); ++i)
        hash = 65599 * hash + m_name[i];
    m_hash = hash;
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Symbol::der()
/*****************************************************************************/
{
  switch (m_kind)
  {
    case CONSTANT:
    case PARAMETER:
        return Zero::getZero(m_shape);
  }
  return BasicPtr(new Der(BasicPtr(this)));
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Symbol::der(BasicPtr const& symbol)
/*****************************************************************************/
{
    if (*symbol.get() == (*this))
    {
      return Util::eye(m_shape);
    }
    return Zero::getZero(m_shape);
}
/*****************************************************************************/

/*****************************************************************************/
Symbol* Symbol::getDerivative()
/*****************************************************************************/
{
  Symbol *s = static_cast<Symbol*>(getUserData(ID_UD_STATEDERIVATIVE));
  if (s == NULL)
  {
    s = new Symbol("$DER" + getName(),getShape());
    s->setUserData(static_cast<void*>(this),ID_UD_STATE);
    setUserData(static_cast<void*>(s),ID_UD_STATEDERIVATIVE);
    if (m_statekind == ALL)
    {
      if (m_state[0] & STATE)
        s->m_state[0] = DER_STATE;
    }
    else
    {
      s->m_statekind = SEVERAL;
      for (size_t i=0;i<m_shape.getNrDimensions();i++)
      {
        if (m_state[i] & STATE)
          s->m_state[i] = s->m_state[i] | DER_STATE;
        else
          s->m_state[i] = NO_STATE;
      }
    }
  }
  return s;
}
/*****************************************************************************/
