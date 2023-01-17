#include "Element.h"
#include "Int.h"
#include "Matrix.h"
#include "Neg.h"
#include "Util.h"
#include "Der.h"
#include "Transpose.h"
#include "Skew.h"
#include "Add.h"
#include "Mul.h"
#include "str.h"

using namespace Symbolics;

/*****************************************************************************/
Element::Element( BasicPtrVec const& args ): NaryOp(Type_Element, args), m_row(0), m_col(0)
/*****************************************************************************/
{
  validate();
}
/*****************************************************************************/


/*****************************************************************************/
Element::Element( BasicPtr const& arg, size_t zeroBasedRow, size_t zeroBasedCol ): 
NaryOp(Type_Element, BasicPtrVec()), m_row(zeroBasedRow), m_col(zeroBasedCol)
/*****************************************************************************/
{
  // Argument speichern
  reserveArgs( 3 );
  addArg( arg );
  addArg( BasicPtr( new Int(m_row) ) );
  addArg( BasicPtr( new Int(m_col) ) );

  validate();
}
/*****************************************************************************/


/*****************************************************************************/
Element::~Element()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
void Element::validate()
/*****************************************************************************/
{
  // Argumente "auspacken"
  if (getArgsSize() != 3) throw InternalError("Element expected args of length 3 but got length " + str(getArgsSize()) + " instead!");
  // Indizes pruefen
  if (getArg(1)->getType() != Type_Int) throw InternalError("Element expected row to be an integer");
  if (getArg(2)->getType() != Type_Int) throw InternalError("Element expected column to be an integer");

  const Int *row = Util::getAsConstPtr<Int>(getArg(1));
  const Int *col = Util::getAsConstPtr<Int>(getArg(2));
  m_row = row->getValue();
  m_col = col->getValue();

  // Pruefung, ob in Ordnung
  // (0-basiert)
  if (m_row >= getArg(0)->getShape().getDimension(1)) 
    throw IndexError("Element: Row outside range!");
  if (m_col >= getArg(0)->getShape().getDimension(2)) 
    throw IndexError("Element: Column outside range!");
}
/*****************************************************************************/


/*****************************************************************************/
std::string Element::toString() const
/*****************************************************************************/
{
#ifdef _DEBUG
  return "Element(" + getArg(0)->toString() + "," + str(m_row) + "," + str(m_col) + ")";
#else
  return getArg(0)->toString() + "[" + str(m_row) + "," + str(m_col) + "]";
#endif
}
/*****************************************************************************/


/*****************************************************************************/
BasicPtr Element::simplify()
/*****************************************************************************/
{
  if (m_simplified)
    return BasicPtr(this);

  // simplify argument
  setArg(0,getArg(0)->simplify());

  if (getArg(0)->is_Scalar())
  {
    if ((m_row==0) && (m_col==0))
      return getArg(0);
    else
      throw InternalError("Cannot get Element[" + str(m_row) + "," + str(m_col) + "] from expression " + getArg(0)->toString());
  }	

  // simplify matrices
  switch(getArg(0)->getType())
  {
  case Type_Neg:
    {
      const Neg *neg = Util::getAsConstPtr<Neg>(getArg(0));
      return Neg::New(New(neg->getArg(),m_row,m_col));
    }
  case Type_Matrix:
    {
      const Matrix *m = Util::getAsConstPtr<Matrix>(getArg(0));
      return BasicPtr( (*m)(m_row,m_col) );
    }
  case Type_Transpose:
    {
      const Transpose *t = Util::getAsConstPtr<Transpose>(getArg(0));
      return New(t->getArg(),m_col,m_row);
    }
  case Type_Skew:
    {
      const Skew *t = Util::getAsConstPtr<Skew>(getArg(0));
      if (m_row==m_col)
        return Zero::getZero();
      else
      {
        if (m_col == 0)
        {
          if (m_row == 1)
            return Element::New(t->getArg(),2,0);
          if (m_row == 2)
            return Neg::New(Element::New(t->getArg(),1,0));
        }
        if (m_col == 1)
        {
          if (m_row == 0)
            return Neg::New(Element::New(t->getArg(),2,0));
          if (m_row == 2)
            return Element::New(t->getArg(),0,0);
        }
        if (m_col == 2)
        {
          if (m_row == 0)
            return Element::New(t->getArg(),1,0);
          if (m_row == 1)
            return Neg::New(Element::New(t->getArg(),0,0));
        }
      }
    }
  case Type_Zero:
    {
      return Int::getZero();
    }
  case Type_Int:
    {
      return getArg(0);
    }
  case Type_Real:
    {
      return getArg(0);
    }
  case Type_Der:
    {
      const Der *der = Util::getAsConstPtr<Der>(getArg(0));
      return Der::New(New(der->getArg(),m_row,m_col));
    }
  case Type_Add:
    {
       const Add *add = Util::getAsConstPtr<Add>(getArg(0));
       BasicPtrVec args;
       for(size_t i=0;i<add->getArgsSize();i++)
       {
         BasicPtr a = add->getArg(i);
         if (a->is_Scalar())
           args.push_back(a);
         else
          args.push_back(Element::New(a,m_row,m_col));
       }
       return BasicPtr(new Add(args));
    }
  case Type_Mul:
    {
      Scalarizer sc;
      BasicPtr a = getArg(0)->iterateExp(sc)->simplify();
      if (a->getType() != Type_Mul)
        return New(a,m_row,m_col);
    }
  }
  m_simplified = true;
  return BasicPtr(this);
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Element::New( BasicPtr const& arg, size_t zeroBasedRow, size_t zeroBasedCol )
/*****************************************************************************/
{
  if (arg->is_Scalar())
  {
    if ((zeroBasedRow==0) && (zeroBasedCol==0))
      return arg;
    else
      throw InternalError("Cannot get Element[" + str(zeroBasedRow) + "," + str(zeroBasedCol) + "] from expression " + arg->toString());
  }
  // simplify matrices
  switch(arg->getType())
  {
  case Type_Neg:
    {
      const Neg *neg = Util::getAsConstPtr<Neg>(arg);
      return Neg::New(New(neg->getArg(),zeroBasedRow,zeroBasedCol));
    }
  case Type_Matrix:
    {
      const Matrix *m = Util::getAsConstPtr<Matrix>(arg);
      return BasicPtr( (*m)(zeroBasedRow,zeroBasedCol) );
    }
  case Type_Transpose:
    {
      const Transpose *t = Util::getAsConstPtr<Transpose>(arg);
      return New(t->getArg(),zeroBasedCol,zeroBasedRow);
    }
  case Type_Skew:
    {
      const Skew *t = Util::getAsConstPtr<Skew>(arg);
      if (zeroBasedRow==zeroBasedCol)
        return Zero::getZero();
      else
      {
        if (zeroBasedCol == 0)
        {
          if (zeroBasedRow == 1)
            return Element::New(t->getArg(),2,0);
          if (zeroBasedRow == 2)
            return Neg::New(Element::New(t->getArg(),1,0));
        }
        if (zeroBasedCol == 1)
        {
          if (zeroBasedRow == 0)
            return Neg::New(Element::New(t->getArg(),2,0));
          if (zeroBasedRow == 2)
            return Element::New(t->getArg(),0,0);
        }
        if (zeroBasedCol == 2)
        {
          if (zeroBasedRow == 0)
            return Element::New(t->getArg(),1,0);
          if (zeroBasedRow == 1)
            return Neg::New(Element::New(t->getArg(),0,0));
        }
      }
    }
  case Type_Zero:
    {
      return Int::getZero();
    }
  case Type_Bool:
    {
      return arg;
    }
  case Type_Int:
    {
      return arg;
    }
  case Type_Real:
    {
      return arg;
    }
  case Type_Der:
    {
      const Der *der = Util::getAsConstPtr<Der>(arg);
      BasicPtr elem = New(der->getArg(),zeroBasedRow,zeroBasedCol);
      if (elem->getType() != Type_Element)
        return Der::New(elem);
      else
        return BasicPtr(new Der(elem));
    }
  case Type_Add:
    {
       const Add *add = Util::getAsConstPtr<Add>(arg);
       BasicPtrVec args;
       for(size_t i=0;i<add->getArgsSize();i++)
       {
         BasicPtr a = add->getArg(i);
         if (a->is_Scalar())
           args.push_back(a);
         else
           args.push_back(Element::New(a,zeroBasedRow,zeroBasedCol));
       }
       return BasicPtr(new Add(args));;
    }
  case Type_Mul:
    {
      Scalarizer sc;
      BasicPtr a = arg->iterateExp(sc)->simplify();
      if (a->getType() != Type_Mul)
        return New(a,zeroBasedRow,zeroBasedCol);
    }
  }
  return BasicPtr(new Element(arg,zeroBasedRow,zeroBasedCol));
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Element::Scalarizer::process_Arg(BasicPtr const &p)
/*****************************************************************************/
{
  if (p->getType() == Type_Symbol)
  {
    if (p->is_Scalar())
      return p;
    BasicPtrMap::iterator ii = scalars.find(p);
    if (ii != scalars.end())
      return ii->second;
    Matrix *mat = new Matrix(p->getShape());
    size_t dim1 = p->getShape().getDimension(1);
    size_t dim2 = p->getShape().getDimension(2);
    for(size_t i=0;i<dim1;++i)
    {
      for(size_t j=0;j<dim2;++j)
      {
        mat->set(i,j, Element::New(p,i,j));
      }
    }
    BasicPtr matpr(mat);
    scalars[p] = matpr;
    return matpr;
  }
  return p;
}
/*****************************************************************************/
