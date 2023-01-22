#include "Pow.h"
#include "Symbolics.h"
#include <math.h>

using namespace Symbolics;


/*****************************************************************************/
Pow::Pow( BasicPtrVec const& args ): BinaryOp(Type_Pow, args)
/*****************************************************************************/
{
	validate();
}
/*****************************************************************************/


/*****************************************************************************/
Pow::Pow( BasicPtr const& arg1, BasicPtr const& arg2 ): BinaryOp(Type_Pow, arg1, arg2)
/*****************************************************************************/
{
	validate();
}
/*****************************************************************************/


/*****************************************************************************/
Pow::Pow( BasicPtr const& arg1, int arg2 ): BinaryOp(Type_Pow, arg1, new Int(arg2))
/*****************************************************************************/
{
	validate();
}
/*****************************************************************************/

/*****************************************************************************/
Pow::Pow( BasicPtr const& arg1, double arg2 ): BinaryOp(Type_Pow, arg1, new Real(arg2))
/*****************************************************************************/
{
	validate();
}
/*****************************************************************************/

/*****************************************************************************/
Pow::~Pow()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
void Pow::validate()
/*****************************************************************************/
{
    // Shape holen
    Shape s = getArg1()->getShape();
    m_shape = s;

    // getArg2() pruefen
    if ((getArg2()->getType() != Type_Int) && (getArg2()->getType() != Type_Real) && ((getArg2()->getType() != Type_Zero) && (getArg2()->is_Scalar())))
        throw InternalError("getArg2() must be either Int or Real!");

    // Matrix
    if (s.getNrDimensions() == 2)
        if (s.getDimension(1) != s.getDimension(2))
            throw ShapeError("Pow is only defined for square matrices!");

    // Vector
    if (s.getNrDimensions() == 1)
        throw ShapeError("Pow is not defined for vectors!");
}
/*****************************************************************************/


/*****************************************************************************/
std::string Pow::toString() const
/*****************************************************************************/
{
#ifdef _DEBUG
    return "Pow(" + getArg1()->toString() + "," + getArg2()->toString() + ")";
#else
    return "(" + getArg1()->toString() + ")^" + getArg2()->toString();
#endif
}
/*****************************************************************************/


/*****************************************************************************/
BasicPtr Pow::simplify()
/*****************************************************************************/
{
    if (m_simplified)
        return BasicPtr(this);
    // Argument vereinfachen
    simplifyArgs();

    // if getArg2() == 0, then return One
    if (getArg2() == Zero::getZero() ) return Int::getOne();
    if (getArg2() == Int::getOne() ) return getArg1();

    // test if exponent is really Real, convert to Int otherwise
    if (getArg2()->getType()==Type_Real)
    {
        int i = 0;
        if (Util::is_Int(getArg2(),i))
            setArg2(BasicPtr(new Int(i)));
    }

    // if we have a constant, then perform the calculation
    switch (getArg1()->getType()) 
    {
    case Type_Zero:
        return Zero::getZero(m_shape);
    case Type_Int:
        {
            const Int *iBase = Util::getAsConstPtr<Int>(getArg1());
            if (iBase->getValue() == 1) return Int::getOne(); 
            // getArg2()
            Real *r = NULL;
            if (getArg2()->getType() == Type_Int)
            {
                const Int *iExponent = Util::getAsConstPtr<Int>(getArg2());
                r = new Real( pow(    static_cast<double>(iBase->getValue()),
                    static_cast<double>(iExponent->getValue())));
            }
            else if (getArg2()->getType() == Type_Real)
            {
                const Real *rExponent = Util::getAsConstPtr<Real>(getArg2());
                r = new Real( pow(    static_cast<double>(iBase->getValue()),
                    rExponent->getValue()));
            } else
                throw InternalError("getArg2() must be either Int or Real!");

            return BasicPtr(r);
        }
        // Real
    case Type_Real:
        {
            const Real *rBase = Util::getAsConstPtr<Real>(getArg1());
            if (rBase->getValue() == 1.0) return Int::getOne(); 
            // getArg2()
            Real *r = NULL;
            if (getArg2()->getType() == Type_Int)
            {
                const Int *iExponent = Util::getAsConstPtr<Int>(getArg2());
                r = new Real( pow (rBase->getValue(),
                    static_cast<double>(iExponent->getValue())));
            }
            else if (getArg2()->getType() == Type_Real)
            {
                const Real *rExponent = Util::getAsConstPtr<Real>(getArg2());
                r = new Real( pow( rBase->getValue(),
                    rExponent->getValue()));
            } else
                throw InternalError("getArg2() must be either Int or Real!");

            return BasicPtr(r);
        }
        // Matrix
    case Type_Matrix:
        {
            const Matrix *base = Util::getAsConstPtr<Matrix>(getArg1());
            if (base->getNumEl() == 1)
            {
                Matrix *mat = new Matrix(*base);
                mat->set(0,Pow::New(mat->getArg(0),getArg2()));
                return BasicPtr(mat);
            }
            if (getArg2()->getType() == Type_Int)
            {
              const Int *iExponent = Util::getAsConstPtr<Int>(getArg2());
              int i = iExponent->getValue();
              if (i>0)
              {
                Matrix *mat = new Matrix(*base);
                for(int j=1;j<i;++j)
                  *mat = (*mat)*(*base);
                return BasicPtr(mat);
              }
              else
              {
                i = -i;
                Matrix *mat = new Matrix(*base);
                for(int j=1;j<i;++j)
                  *mat = (*mat)*(*base);
                return BasicPtr(new Pow(BasicPtr(mat),Int::getMinusOne()));
              }
            }
            else if (getArg2()->getType() == Type_Real)
            {
              int i = 0;
              if (Util::is_Int(getArg2(),i)) //Sollte eigentlich niemals True ergeben, da es oben schon abgefangen wird.
              {
                  if (i>=0)
                  {
                    Matrix *mat = new Matrix(*base);
                    for(int j=1;j<i;++j)
                      *mat = (*mat)*(*base);
                    return BasicPtr(mat);
                  }
                  else
                  {
                    i = -i;
                    Matrix *mat = new Matrix(*base);
                    for(int j=1;j<i;++j)
                      *mat = (*mat)*(*base);
                    return BasicPtr(new Pow(BasicPtr(mat),Int::getMinusOne()));
                  }
              }
            }
            break;
        }
        // merge all pows together
    case Type_Pow:
        {
            const Pow *c = Util::getAsConstPtr<Pow>(getArg1());
            setArg2(getArg2()*c->getExponent()->simplify());
            setArg1(c->getBase());
            m_simplified = true;
            return BasicPtr(this);
        }
        // move neg to front in case of round number remove it
    case Type_Neg:
        {
            const Neg *c = Util::getAsConstPtr<Neg>(getArg1());

            // try arg2 as integer
            int iExp = 0;
            if ( Util::is_Int(getArg2(), iExp) && (iExp%2==0))
            {
                setArg1(c->getArg());
                m_simplified = true;
                return BasicPtr(this);
            }

            // swap neg and pow
            return Neg::New(New(c->getArg(),getArg2()));
        }
    }

    m_simplified = true;
    return BasicPtr(this);
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Pow::New( BasicPtr const& arg1, BasicPtr const& arg2)
/*****************************************************************************/
{
    // if arg2 == 0, then return One
    if (arg2 == Zero::getZero() ) return Int::getOne();
    if (arg2 == Int::getOne() ) return arg1;

    // if we have a constant, then perform the calculation
    switch (arg1->getType()) 
    {
    case Type_Zero:
        return Zero::getZero(arg1->getShape());
    case Type_Int:
        {
            const Int *iBase = Util::getAsConstPtr<Int>(arg1);
            if (iBase->getValue() == 1) return Int::getOne(); 
            // arg2
            Real *r = NULL;
            if (arg2->getType() == Type_Int)
            {
                const Int *iExponent = Util::getAsConstPtr<Int>(arg2);
                r = new Real( pow(    static_cast<double>(iBase->getValue()),
                    static_cast<double>(iExponent->getValue())));
            }
            else if (arg2->getType() == Type_Real)
            {
                const Real *rExponent = Util::getAsConstPtr<Real>(arg2);
                r = new Real( pow(    static_cast<double>(iBase->getValue()),
                    rExponent->getValue()));
            } else
                throw InternalError("arg2 must be either Int or Real!");

            return BasicPtr(r);
        }
        // Real
    case Type_Real:
        {
            const Real *rBase = Util::getAsConstPtr<Real>(arg1);
            if (rBase->getValue() == 1.0) return Int::getOne(); 
            // arg2
            Real *r = NULL;
            if (arg2->getType() == Type_Int)
            {
                const Int *iExponent = Util::getAsConstPtr<Int>(arg2);
                r = new Real( pow(                        rBase->getValue(),
                    static_cast<double>(iExponent->getValue())));
            }
            else if (arg2->getType() == Type_Real)
            {
                const Real *rExponent = Util::getAsConstPtr<Real>(arg2);
                r = new Real( pow(    rBase->getValue(),
                    rExponent->getValue()));
            } else
                throw InternalError("arg2 must be either Int or Real!");

            return BasicPtr(r);
        }
        // Matrix
    case Type_Matrix:
        {
            const Matrix *base = Util::getAsConstPtr<Matrix>(arg1);
            if (base->getNumEl() == 1)
            {
                Matrix *mat = new Matrix(*base);
                mat->set(0,Pow::New(mat->getArg(0),arg2));
                return BasicPtr(mat);
            }
            if (arg2->getType() == Type_Int)
            {
              const Int *iExponent = Util::getAsConstPtr<Int>(arg2);
              int i = iExponent->getValue();
              if (i>0)
              {
                Matrix *mat = new Matrix(*base);
                for(int j=1;j<i;++j)
                  *mat = (*mat)*(*base);
                return BasicPtr(mat);
              }
              else
              {
                i = -i;
                Matrix *mat = new Matrix(*base);
                for(int j=1;j<i;++j)
                  *mat = (*mat)*(*base);
                return BasicPtr(new Pow(BasicPtr(mat),Int::getMinusOne()));
              }
            }
            else if (arg2->getType() == Type_Real)
            {
              int i = 0;
              if (Util::is_Int(arg2,i))
              {
                  if (i>0)
                  {
                    Matrix *mat = new Matrix(*base);
                    for(int j=1;j<i;++j)
                      *mat = (*mat)*(*base);
                    return BasicPtr(mat);
                  }
                  else
                  {
                    i = -i;
                    Matrix *mat = new Matrix(*base);
                    for(int j=1;j<i;++j)
                      *mat = (*mat)*(*base);
                    return BasicPtr(new Pow(BasicPtr(mat),Int::getMinusOne()));
                  }
              }
            }
            break;
        }

        // merge all pows together
    case Type_Pow:
        {
            const Pow *c = Util::getAsConstPtr<Pow>(arg1);
            return New(c->getBase(),Mul::New(arg2,c->getExponent()));
        }
        // move neg to front in case of round number remove it
    case Type_Neg:
        {
            const Neg *c = Util::getAsConstPtr<Neg>(arg1);

            // try arg2 as integer
            BasicPtr arg_1 = c->getArg();
            int iExp = 0;
            if ( Util::is_Int(arg2, iExp) && (iExp%2==0))
            {
                return New(arg_1,arg2);
            }

            // swap neg and pow
            return Neg::New(New(arg_1,arg2));
        }
    }
    return BasicPtr(new Pow(arg1,arg2));
}
/*****************************************************************************/

/*****************************************************************************/
inline BasicPtr Pow::der()
/*****************************************************************************/
{
  // pow(x,y) = der(x)*y*pow(x,y-1)
  return Mul::New(getExponent(),Mul::New(getBase()->der(),New(getBase(),Add::New(getExponent(),Int::getMinusOne()))));
}
/*****************************************************************************/

/*****************************************************************************/
inline BasicPtr Pow::der(BasicPtr const& symbol)
/*****************************************************************************/
{
  // pow(x,y) = der(x)*y*pow(x,y-1)
  return Mul::New(getExponent(),Mul::New(getBase()->der(symbol),New(getBase(),Add::New(getExponent(),Int::getMinusOne()))));
}
/*****************************************************************************/