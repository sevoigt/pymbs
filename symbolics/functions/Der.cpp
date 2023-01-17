#include "Der.h"
#include "Util.h"
#include "Symbol.h"
#include "Int.h"
#include "Matrix.h"

using namespace Symbolics;

/*****************************************************************************/
Der::Der( BasicPtrVec const& args ): UnaryOp(Type_Der, args)
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
Der::Der( BasicPtr const& arg ): UnaryOp(Type_Der, arg)
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
Der::~Der()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
std::string Der::toString() const
/*****************************************************************************/
{
#ifdef _DEBUG
    return "der(" + getArg()->toString() + ")";
#else
    return "der(" + getArg()->toString() + ")";
#endif
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Der::simplify()
/*****************************************************************************/
{
    if (m_simplified)
        return BasicPtr(this);
    // Argument vereinfachen
    simplifyArg();

    // simplify constants
  switch( getArg()->getType())
  {
  case Type_Zero:
  case Type_Eye:
        return getArg();
  case Type_Int:
  case Type_Real:
        return Zero::getZero();
  case Type_Matrix:
    {
        Matrix::UnaryFunctor<Der> f;
        const Matrix *mat = Util::getAsConstPtr<Matrix>(getArg());
        return mat->applyFunctor(f);
    }
  }
  m_simplified = true;
  return BasicPtr(this);
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Der::New( BasicPtr const& arg)
/*****************************************************************************/
{
    // simplify constants
  switch( arg->getType())
  {
  case Type_Zero:
  case Type_Eye:
        return arg;
  case Type_Int:
  case Type_Real:
        return Zero::getZero();
  case Type_Der:
     return BasicPtr(new Der(arg));
  case Type_Matrix:
    {
        Matrix::UnaryFunctor<Der> f;
        const Matrix *mat = Util::getAsConstPtr<Matrix>(arg);
        return mat->applyFunctor(f);
    }
  }
  return arg->der();
}
/*****************************************************************************/

