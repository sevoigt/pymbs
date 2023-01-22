#include "Mul.h"
#include "Add.h"
#include "Int.h"
#include "Matrix.h"
#include "Neg.h"
#include "Util.h"
#include "Pow.h"
#include <list>

using namespace Symbolics;

/*****************************************************************************/
Mul::Mul( BasicPtr const& arg1, BasicPtr const& arg2 ): NaryOp(Type_Mul, arg1, arg2)
/*****************************************************************************/
{
    // Shape berechnen
    m_shape = arg1->getShape() * arg2->getShape();
}
/*****************************************************************************/

/*****************************************************************************/
Mul::Mul( BasicPtrVec const& args ): NaryOp(Type_Mul, args)
/*****************************************************************************/
{
    // Alle Argumente den "Argumenten" hinzufuegen
	size_t num = getArgsSize();
	for(size_t i=0; i<num; ++i )
		m_shape *= getArg(i)->getShape();
}
/*****************************************************************************/

/*****************************************************************************/
Mul::~Mul()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
std::string Mul::toString() const
/*****************************************************************************/
{
#ifdef _DEBUG
    return "Mul(" + join(",") + ")";
#else
    return "(" + join(" * ") + ")";
#endif
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Mul::simplify()
/*****************************************************************************/
{
    if (m_simplified)
        return BasicPtr(this);
    // call arg
    simplifyArgs();

    // merge all muls together
    // move neg to front
    bool negate_me = false;
    BasicPtrVec new_args;
    new_args.reserve(getArgsSize());
    std::list<BasicPtr> scalar_args;
    for (size_t i=0; i<getArgsSize(); ++i)
    {
        if (getArg(i)->getType() == Type_Zero)
        {
            // Zeros
            clearArgs();
            addArg(Zero::getZero(m_shape));
            return getArg(0);
        }
        else if (getArg(i)->getType() == Type_Mul)
        {
            const Mul *a = Util::getAsConstPtr<Mul>(getArg(i));
            size_t argssize = a->getArgsSize();
            for (size_t j=0; j<argssize; ++j)
            {
              if (a->getArg(j)->is_Scalar())
                scalar_args.push_back(a->getArg(j));
              else
                new_args.push_back(a->getArg(j));
            }
            continue;
        }
        else if (getArg(i)->getType() == Type_Neg)
        {
            const Neg *n = Util::getAsConstPtr<Neg>(getArg(i));
            BasicPtr narg = n->getArg();
            negate_me = !negate_me;
            if (narg->getType() == Type_Mul)
            {
                const Mul *a = Util::getAsConstPtr<Mul>(narg);
                size_t argssize = a->getArgsSize();
                for (size_t j=0; j<argssize; ++j)
                {
                  if (a->getArg(j)->is_Scalar())
                    scalar_args.push_back(a->getArg(j));
                  else
                    new_args.push_back(a->getArg(j));
                }
                continue;
            }
            if (narg->is_Scalar())
              scalar_args.push_back(narg);
            else
              new_args.push_back(narg);
            continue;
        }
        if (getArg(i)->is_Scalar())
          scalar_args.push_back(getArg(i));
        else
          new_args.push_back(getArg(i));
    }
    scalar_args.sort();
    std::list<BasicPtr>::iterator ie = scalar_args.end();
    for (std::list<BasicPtr>::iterator ii=scalar_args.begin();ii!=ie;++ii)
      new_args.push_back(*ii);
    if (new_args.size() == 1)
    {
        clearArgs();
        if (negate_me)
            addArg(Neg::New(new_args[0]));
        else
            addArg(new_args[0]);
        return getArg(0);
    }
    // combine constant args
    BasicPtr val( Int::getOne() );
    BasicPtrVec dst;
    dst.reserve(new_args.size());
    // Alle Argumente durchlaufen
    for (size_t i=0; i<new_args.size(); ++i)
    {
        // Falls ein Argument 0 ist, dann sofort 0 zurueckgeben
        if (Util::is_Zero(new_args[i]))
        {
            clearArgs();
            addArg(Zero::getZero(m_shape));
            return Zero::getZero(m_shape);
        }

        if ((Util::is_One(new_args[i])) && new_args[i]->is_Scalar())
            continue;

        // Falls wir einen Int oder Real gefunden haben, diesen multiplizieren
        if ((new_args[i]->getType() == Type_Real) || (new_args[i]->getType() == Type_Int))
        {
            val = New(val,new_args[i]);
        }
        else
        // Sonst fuegen wir es der Liste hinzu
            dst.push_back(new_args[i]);
    }
    // Wenn der Wert davor nicht 1 ist, dann die Konstante vorn an die Liste anfuegen
    if (!Util::is_One(val))
    {
        // Falls es eine -1 ist, einfach negieren
        if (Util::is_One(Neg::New(val)))
            negate_me = !negate_me;
        else
            dst.insert(dst.begin(),val);
    }
    // Wenn wir keine Argumente mehr haben, dann eine eine 1 zurueckgeben. Shape koennen wir hier getrost ignorieren
    if (dst.size() == 0)
    {
        clearArgs();
        if(is_Scalar())
        {
          if (negate_me)
            addArg(Int::getMinusOne());
          else
            addArg(Int::getOne());
          return getArg(0);
        }
        BasicPtr one = Util::eye(m_shape);
        if (negate_me)
          addArg(Neg::New(one));
        else
          addArg(one);
        return getArg(0);
    }
    clearArgs();
    reserveArgs( dst.size() );
    // combine Matrix args
    BasicPtr lastarg = dst[0];
    for (size_t i=1; i<dst.size(); ++i)
    {
      if ( ( (lastarg->getType() == Type_Matrix) && ( (dst[i]->getType() == Type_Matrix) || (dst[i]->is_Scalar()) ) ) ||
           ( (dst[i]->getType() == Type_Matrix) && (lastarg->is_Scalar()) ) ) 
        {
            // mul them
            lastarg = New(lastarg,dst[i]);
        }
        else if (Util::is_One(lastarg) && (!dst[i]->is_Scalar()))
        {
          lastarg=dst[i];
        }
        else if (Util::is_One(dst[i]) && (!lastarg->is_Scalar()))
        {
          continue;
        }
        else if(lastarg->getType() == Type_Pow)
        {
            const Pow *c = Util::getAsConstPtr<Pow>(lastarg);
            if (dst[i] == c->getBase())
              lastarg = BasicPtr(new Pow(dst[i],Add::New(c->getExponent(),Int::getOne())));
            else
            {
              addArg(lastarg->simplify());
              lastarg = dst[i];
            }
        }
        else if(dst[i]->getType() == Type_Pow)
        {
            const Pow *c = Util::getAsConstPtr<Pow>(dst[i]);
            if (lastarg == c->getBase())
              lastarg = BasicPtr(new Pow(lastarg,Add::New(c->getExponent(),Int::getOne())));
            {
              addArg(lastarg->simplify());
              lastarg = dst[i];
            }
        }
        else if(lastarg == dst[i])
        {
            lastarg = BasicPtr(new Pow(lastarg,Int::New(2)));
        }
        else
        {
            addArg(lastarg->simplify());
            lastarg = dst[i];
        }
    }
    addArg(lastarg->simplify());
    if (getArgsSize() == 1)
    {
        if (negate_me)
        {
            setArg(0,Neg::New(getArg(0)));
        }
        return getArg(0);
    }
    if (negate_me)
    {
        BasicPtrVec args;
        args.reserve(getArgsSize());
        for (size_t i=0;i<getArgsSize();++i)
            args.push_back(getArg(i));
        setArg(0,Neg::New(getArg(0)));
        return Neg::New(BasicPtr(new Mul(args)));
    }
    m_simplified = true;
    return BasicPtr(this);
}
/*****************************************************************************/

/*****************************************************************************/
bool Mul::operator==(  Basic const& rhs ) const
/*****************************************************************************/
{
    // Versuch zu casten
    const Mul *mul = dynamic_cast<const Mul*>(&rhs);
    if (mul == NULL) return false;

      // Anzahl der Argumente pruefen
    if (getArgsSize() != mul->getArgsSize()) return false;

    // aufteilen in skalar und nicht scalar
    BasicPtrVec m_scalars;
    BasicPtrVec m_nonscalars;
    BasicPtrVec rhs_scalars;
    BasicPtrVec rhs_nonscalars;
    for (size_t i=0;i<getArgsSize();++i)
        if (getArg(i)->is_Scalar())
            m_scalars.push_back(getArg(i));
        else
            m_nonscalars.push_back(getArg(i));

    for (size_t i=0;i<mul->getArgsSize();++i)
        if (mul->getArg(i)->is_Scalar())
            rhs_scalars.push_back(mul->getArg(i));
        else
            rhs_nonscalars.push_back(mul->getArg(i));

      // Anzahl der Argumente pruefen
    if (m_scalars.size() != rhs_scalars.size()) return false;
    if (m_nonscalars.size() != rhs_nonscalars.size()) return false;

      // Alle Argumente pruefen
    for (size_t i=0; i<m_nonscalars.size(); ++i)
        if (*m_nonscalars[i] != *rhs_nonscalars[i]) return false;

    // Abkuerzung
    switch (m_scalars.size())
    {
    case 0:
        return true;
    case 1:
        return (m_scalars[0] == mul->getArg(0));
    case 2:
        return (((m_scalars[0] == rhs_scalars[0]) && (m_scalars[1] == rhs_scalars[1])) ||
                ((m_scalars[0] == rhs_scalars[1]) && (m_scalars[1] == rhs_scalars[0])));
    default:
        {
            // Liste anlegen
            std::list< BasicPtr> remoteArgs( rhs_scalars.begin(), rhs_scalars.end() );
            // Elemente suchen und loeschen
            for (size_t i=0;i<m_scalars.size();++i)
            {
                BasicPtr arg(m_scalars[i]);
                bool found=false;
                for (std::list< BasicPtr>::iterator it=remoteArgs.begin(); it!=remoteArgs.end(); ++it)
                {
                    // Auf Gleichheit pruefen
                     BasicPtr tmp(*it);
                    if (arg == tmp)
                    {
                        // und loeschen
                        remoteArgs.erase( it );
                        found = true;
                        break;
                    }
                }
                // Wenn ein Argument nicht gefunden wurde, muessen sie verschieden sein
                if (!found) return false;
            }
        }
    }
    // Wenn er hier ankommt, dann sind sie wirklich gleich
    return true;
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Mul::New( BasicPtr const& lhs, BasicPtr  const& rhs )
/*****************************************************************************/
{
	// Shape ermitteln
    Shape shape = lhs->getShape()*rhs->getShape();

	// Shape beibehalten
    if (Util::is_Zero(lhs))
	    return Zero::getZero(shape);
    if (Util::is_Zero(rhs))
	    return Zero::getZero(shape);
	if ((Util::is_One(lhs)) && (rhs->getShape() == shape))
        return rhs;
	if ((Util::is_One(rhs)) && (lhs->getShape() == shape))
		return lhs;


	// TODO: Muss hier noch etwas zum Shape behalten getan werden?
    if (lhs->getType() == Type_Int) 
    {
        const Int *clhs = Util::getAsConstPtr<Int>(lhs);
        if (rhs->getType() == Type_Int) 
        {
            const Int *crhs = Util::getAsConstPtr<Int>(rhs);
            return Int::New(clhs->getValue()*crhs->getValue());
        }
        else if (rhs->getType() == Type_Real) 
        {
            const Real *crhs = Util::getAsConstPtr<Real>(rhs);
            return BasicPtr(new Real(clhs->getValue()*crhs->getValue()));
        }
        else if (rhs->getType() == Type_Matrix) 
        {
            const Matrix *crhs = Util::getAsConstPtr<Matrix>(rhs);
            BasicPtr res(new Matrix((*clhs)*(*crhs)));
            if (res->is_Scalar())
              return res->getArg(0);
            else
              return res;
        }
    }
    else if (lhs->getType() == Type_Real) 
    {
        const Real *clhs = Util::getAsConstPtr<Real>(lhs);
        if (rhs->getType() == Type_Int) 
        {
            const Int *crhs = Util::getAsConstPtr<Int>(rhs);
            return BasicPtr(new Real(clhs->getValue()*crhs->getValue()));
        }
        else if (rhs->getType() == Type_Real) 
        {
            const Real *crhs = Util::getAsConstPtr<Real>(rhs);
            return BasicPtr(new Real(clhs->getValue()*crhs->getValue()));
        }
        else if (rhs->getType() == Type_Matrix) 
        {
            const Matrix *crhs = Util::getAsConstPtr<Matrix>(rhs);
            BasicPtr res(new Matrix((*clhs)*(*crhs)));
            if (res->is_Scalar())
              return res->getArg(0);
            else
              return res;
        }
    }
    else if (lhs->getType() == Type_Matrix) 
    {
        const Matrix *clhs = Util::getAsConstPtr<Matrix>(lhs);
        if (rhs->getType() == Type_Int) 
        {
            const Int *crhs = Util::getAsConstPtr<Int>(rhs);
            BasicPtr res(new Matrix((*clhs)*(*crhs)));
            if (res->is_Scalar())
              return res->getArg(0);
            else
              return res;
        }
        else if (rhs->getType() == Type_Real) 
        {
            const Real *crhs = Util::getAsConstPtr<Real>(rhs);
            BasicPtr res(new Matrix((*clhs)*(*crhs)));
            if (res->is_Scalar())
              return res->getArg(0);
            else
              return res;
        }
        else if (rhs->getType() == Type_Matrix) 
        {
            const Matrix *crhs = Util::getAsConstPtr<Matrix>(rhs);
            BasicPtr res(new Matrix((*clhs)*(*crhs)));
            if (res->is_Scalar())
              return res->getArg(0);
            else
              return res;
        }
        else if (rhs->is_Scalar()) 
        {
            Matrix *c = new Matrix( *clhs );
            for (size_t i=0; i<c->getNumEl(); ++i)
              c->set(i,c->get(i)*rhs);
            return BasicPtr(c);
        }
    }
    if (rhs->getType() == Type_Matrix)
    {
        if (lhs->is_Scalar()) 
        {
            const Matrix *crhs = Util::getAsConstPtr<Matrix>(rhs);
            Matrix *c = new Matrix( *crhs );
            for (size_t i=0; i<c->getNumEl(); ++i)
              c->set(i,lhs*c->get(i));
            return BasicPtr(c);
        }
    }
    if (lhs->getType() == Type_Mul)
    {
        BasicPtrVec mulargs;
        const Mul *a = Util::getAsConstPtr<Mul>(lhs);
        size_t argssize = a->getArgsSize();
        for (size_t j=0; j<argssize; ++j)
          mulargs.push_back(a->getArg(j));

        if (rhs->getType() == Type_Mul)
        {
          const Mul *b = Util::getAsConstPtr<Mul>(rhs);
          size_t argssize = b->getArgsSize();
          for (size_t j=0; j<argssize; ++j)
            mulargs.push_back(b->getArg(j));
        }
        else
            mulargs.push_back(rhs);
        return BasicPtr(new Mul(mulargs));
    }
    if (rhs->getType() == Type_Mul)
    {
        BasicPtrVec mulargs;
        mulargs.push_back(lhs);
        const Mul *a = Util::getAsConstPtr<Mul>(rhs);
        size_t argssize = a->getArgsSize();
        for (size_t j=0; j<argssize; ++j)
          mulargs.push_back(a->getArg(j));
        return BasicPtr(new Mul(mulargs));
    }
    return BasicPtr(new Mul(lhs,rhs));
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Mul::der()
/*****************************************************************************/
{
    // add args
    BasicPtrVec addargs;
    addargs.reserve(getArgsSize());

    BasicPtrVec mulargs;
    mulargs.reserve(getArgsSize());
    for (size_t j=0;j<getArgsSize();++j)
      mulargs.push_back(getArg(j));
    // Nun alles differenzieren
    for(size_t i=0; i<getArgsSize(); ++i)
    {
      BasicPtrVec tmp(mulargs);
      tmp[i] = tmp[i]->der();

      addargs.push_back(BasicPtr(new Mul(tmp)));
    }

    // fertig
    return BasicPtr(new Add(addargs));
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Mul::der(BasicPtr const& symbol)
/*****************************************************************************/
{
    // add args
    BasicPtrVec addargs;
    addargs.reserve(getArgsSize());

    BasicPtrVec mulargs;
    mulargs.reserve(getArgsSize());
    for (size_t j=0;j<getArgsSize();++j)
      mulargs.push_back(getArg(j));
    // Nun alles differenzieren
    for(size_t i=0; i<getArgsSize(); ++i)
    {
      BasicPtrVec tmp(mulargs);
      tmp[i] = tmp[i]->der(symbol);

      addargs.push_back(BasicPtr(new Mul(tmp)));
    }

    // fertig
    return BasicPtr(new Add(addargs));
}
/*****************************************************************************/