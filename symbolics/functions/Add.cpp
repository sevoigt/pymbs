/*
This file is part of PyMbs.

PyMbs is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as 
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

PyMbs is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public 
License along with PyMbs.
If not, see <http://www.gnu.org/licenses/>.

Copyright 2011, 2012 Carsten Knoll, Christian Schubert, 
                     Jens Frenkel, Sebastian Voigt
*/

#include <stdarg.h>
#include <map>
#include "Add.h"
#include "Int.h"
#include "Matrix.h"
#include "Neg.h"
#include "Mul.h"
#include "Util.h"
#include <list>

using namespace Symbolics;


/*****************************************************************************/
Add::Add( BasicPtr const& arg1, BasicPtr const& arg2 ): NaryOp(Type_Add, arg1, arg2)
/*****************************************************************************/
{
    // Shape berechnen
    m_shape = arg1->getShape() + arg2->getShape();
}
/*****************************************************************************/


/*****************************************************************************/
Add::Add( BasicPtrVec const& args ): NaryOp(Type_Add, args)
/*****************************************************************************/
{
    // Alle Argumente den "Argumenten" hinzufuegen
    size_t num = getArgsSize();
    for(size_t i=0; i<num; ++i )
		m_shape += getArg(i)->getShape();
}
/*****************************************************************************/


/*****************************************************************************/
Add::~Add()
/*****************************************************************************/
{
}
/*****************************************************************************/


/*****************************************************************************/
std::string Add::toString() const
/*****************************************************************************/
{
#ifdef _DEBUG
    return "Add(" + join(",") + ")";
#else
    return "(" + join(" + ", " - ") + ")";
#endif
}
/*****************************************************************************/


/*****************************************************************************/
BasicPtr Add::simplify()
/*****************************************************************************/
{
    if (m_simplified)
        return BasicPtr(this);
    // call arg
    simplifyArgs();

    // merge all adds together
    BasicPtrVec new_args;
    new_args.reserve(getArgsSize());
    for (size_t i=0; i<getArgsSize(); ++i)
    {
      if ((getArg(i)->getType() == Type_Zero) && (getArg(i)->is_Scalar()))
        {
            // skip Zeros
            continue;
        }
        else if (getArg(i)->getType() == Type_Add)
        {
            const Add *a = Util::getAsConstPtr<Add>(getArg(i));
            size_t argssize = a->getArgsSize();
            for (size_t j=0; j<argssize; ++j)
              new_args.push_back(a->getArg(j));
            continue;
        }
        else if (getArg(i)->getType() == Type_Neg)
        {
            const Neg *n = Util::getAsConstPtr<Neg>(getArg(i));
            BasicPtr narg = n->getArg();
            if (narg->getType() == Type_Add)
            {
                const Add *a = Util::getAsConstPtr<Add>(narg);
                size_t argssize = a->getArgsSize();
                for (size_t j=0; j<argssize; ++j)
                {
                    new_args.push_back(Neg::New(a->getArg(j)));
                }
                continue;
            }
            if (narg->getType() == Type_Matrix)
            {
                Matrix::UnaryFunctor<Neg> f;
                const Matrix *mat = Util::getAsConstPtr<Matrix>(narg);
                new_args.push_back(mat->applyFunctor(f));
                continue;
            }
        }
        new_args.push_back(getArg(i));
    }
    if (new_args.size() == 1)
        return new_args[0];
    // combine constant args
    BasicPtrVec other_args;
    BasicPtr val( Zero::getZero(m_shape) );
    bool has_val = false;
    for (size_t i=0; i<new_args.size(); ++i)
    {
        if ((new_args[i]->getType() == Type_Zero) && (new_args[i]->is_Scalar()))
        {
            // skip Zeros
            continue;
        }
        else if ((new_args[i]->getType() == Type_Real) || (new_args[i]->getType() == Type_Int))
        {
            val = New(val,new_args[i]);
            has_val = true;
        }
        else if ((new_args[i]->getType() == Type_Matrix) || (Util::is_Const(new_args[i])))
        {
            val = New(val,new_args[i]);
            has_val = true;
        }
        else
            other_args.push_back(new_args[i]);
    }
    if (has_val)
    {
        if (other_args.size() == 0)
        {
            clearArgs();
            addArg(val);
            return val;
        }
        if (!(Util::is_Zero(val) && val->is_Scalar()))
            other_args.insert(other_args.begin(),val);
    }
    if (other_args.size() == 0)
    {
        clearArgs();
        addArg(val);
        return val;
    }
    // combine equal args
    BasicPtrMap argmap;
    for (size_t i=0; i<other_args.size(); ++i)
    {
        BasicPtr v(new Int(1));
        if (other_args[i]->getType() == Type_Neg)
        {
            const Neg *c = Util::getAsConstPtr<Neg>(other_args[i]);
            other_args[i] = c->getArg();
            v = Neg::New(v);
        }
        if (other_args[i]->getType() == Type_Mul)
        {
            const Mul *c = Util::getAsConstPtr<Mul>(other_args[i]);
            size_t mulargssize = c->getArgsSize();
            if (mulargssize == 2)
            {
              BasicPtr mularg0 = c->getArg(0);
              BasicPtr mularg1 = c->getArg(1);
              if ((mularg0->getType() == Type_Real) || (mularg0->getType() == Type_Int))
                {
                    Shape mcs = mularg0->getShape();
                    if (mcs.getNrDimensions() == 0)
                    {
                        v = v*mularg0;
                        other_args[i] = mularg1;
                    }
                }
                else if ((mularg1->getType() == Type_Real) || (mularg1->getType() == Type_Int))
                {
                    Shape mcs = mularg1->getShape();
                    if (mcs.getNrDimensions() == 0)
                    {
                        v = v*mularg1;
                        other_args[i] = mularg0;
                    }
                }
            }
        }
        BasicPtrMap::iterator iter = argmap.find(other_args[i]);
        if (iter == argmap.end())
        {
            argmap[other_args[i]] = v;
        }
        else
        {
            argmap[other_args[i]] = New(argmap[other_args[i]],v);
        }
    }
    BasicPtrMap::iterator iter;
    BasicPtrVec other_args1;
    other_args1.reserve( argmap.size() );
    for(iter=argmap.begin();iter!=argmap.end();++iter)
    {
        if (!Util::is_Zero(iter->second))
        {
            if (Util::is_One(iter->second))
            {
              other_args1.push_back(iter->first);
            }
            else if (Util::is_One(Neg::New(iter->second)))
            {
                BasicPtr arg = iter->first;
                if (arg->getType() == Type_Real)
                    other_args1.push_back(iter->first);
                else if (arg->getType() == Type_Int)
                    other_args1.push_back(iter->first);
                else
                {
                    BasicPtr arg = Neg::New(iter->first);
                    other_args1.push_back(arg);
                }
            }
            else
            {
                other_args1.push_back(Mul::New(iter->second,iter->first));
            }
        }
    }
    // Wenn wir keine Argumente mehr haben, dann eine eine 1 zurueckgeben
    if (other_args1.size() == 0)
    {
        if(is_Scalar())
        {
           clearArgs();
           addArg(Int::getZero());
           return getArg(0);
        }
        clearArgs();
        addArg(Zero::getZero(m_shape));
        return getArg(0);
    }
    clearArgs();
    reserveArgs( other_args1.size() );
    // combine Matrix args
    if (!is_Scalar())
    {
      BasicPtrVec scalars;
      BasicPtrVec matrixes;
      bool hasMatZero;
      for (size_t i=0; i<other_args1.size(); ++i)
      {
        if (other_args1[i]->getType() == Type_Matrix)
        {
          matrixes.push_back(other_args1[i]);
          continue;
        }
        else if (other_args1[i]->is_Scalar())
        {
          scalars.push_back(other_args1[i]);
          continue;
        }
        else if (other_args1[i]->getType() == Type_Zero)
        {
          hasMatZero=true;
          continue;
        }
        else
          addArg(other_args1[i]);
      }

      if (matrixes.size() > 0)
      {
        if (matrixes.size() == 1)
        {
          if (scalars.size() > 0)
          {
            BasicPtr s;
            if (scalars.size() > 1)
              s = BasicPtr(new Add(scalars));
            else
              s = scalars[0];
            Matrix *c = new Matrix( *(Util::getAsConstPtr<Matrix>(matrixes[0])) );
            for (size_t i=0; i<c->getNumEl(); ++i)
              c->set(i,c->get(i)+s);
          }
          else
            addArg(matrixes[0]);
        }
        else
        {
          Matrix *mat = new Matrix( *(Util::getAsConstPtr<Matrix>(matrixes[0])) );
          for (size_t i=1; i<matrixes.size(); ++i)
          {
            const Matrix *m = Util::getAsConstPtr<Matrix>(matrixes[i]);
            *mat *= (*m); 
          }
          if (scalars.size() > 0)
          {
            BasicPtr s;
            if (scalars.size() > 1)
              s = BasicPtr(new Add(scalars));
            else
              s = scalars[0];
            for (size_t i=0; i<mat->getNumEl(); ++i)
              mat->set(i,mat->get(i)+s);
          }
          addArg(BasicPtr(mat));
        }
      }
      else if(hasMatZero)
      {
        if (scalars.size() > 0)
        {
          BasicPtr s;
          if (scalars.size() > 1)
            s = BasicPtr(new Add(scalars));
          else
            s = scalars[0];
          Matrix *mat = new Matrix(m_shape);
          for (size_t i=0; i<mat->getNumEl(); ++i)
            mat->set(i,s);
          addArg(BasicPtr(mat));
        }
      }
      else
      {
        for (size_t i=0;i<scalars.size();++i)
          addArg(scalars[i]);
      }
    }
    else
    {
      for (size_t i=0;i<other_args1.size();++i)
        addArg(other_args1[i]);
    }
    // TODO add more simplification
    if (getArgsSize() == 0)
    {
        if(is_Scalar())
        {
           addArg(Int::getZero());
           return getArg(0);
        }
        addArg(Zero::getZero(m_shape));
        return getArg(0);
    }
    // only one arg -> return arg
    if (getArgsSize() == 1)
    {
        return getArg(0);
    }
    m_simplified = true;
    return BasicPtr(this);
}
/*****************************************************************************/


/*****************************************************************************/
bool Add::operator==(  Basic const& rhs ) const
/*****************************************************************************/
{
    // Versuch zu casten
    const Add *add = dynamic_cast<const Add*>(& rhs);
    if (add == NULL) return false;

    // Anzahl der Argumente pruefen
    if (getArgsSize() != add->getArgsSize()) return false;

    // Abkuerzung
    switch (getArgsSize())
    {
    case 0:
        return true;
    case 1:
        return (getArg(0) == add->getArg(0));
    case 2:
        return (((getArg(0) == add->getArg(0)) && (getArg(1) == add->getArg(1))) ||
                ((getArg(0) == add->getArg(1)) && (getArg(1) == add->getArg(0))));
    default:
        {
            // Liste anlegen
            std::list< BasicPtr> remoteArgs;
            for (size_t i=0;i<add->getArgsSize();i++)
              remoteArgs.push_back(add->getArg(i));
            // Elemente suchen und loeschen
            for (size_t i=0;i<getArgsSize();++i)
            {
                BasicPtr arg(getArg(i));
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
BasicPtr Add::New( BasicPtr const& lhs, BasicPtr const& rhs )
/*****************************************************************************/
{
	// Shape ermitteln
    Shape shape = lhs->getShape() + rhs->getShape();

	// Shape beibehalten
	if ((Util::is_Zero(lhs)) && (rhs->getShape() == shape))
        return rhs;
	if ((Util::is_Zero(rhs)) && (lhs->getShape() == shape))
		return lhs;


	// TODO: Muss hier noch etwas zum Shape behalten getan werden?
    if (lhs->getType() == Type_Int) 
    {
        const Int *clhs = Util::getAsConstPtr<Int>(lhs);
        if (rhs->getType() == Type_Int) 
        {
            const Int *crhs = Util::getAsConstPtr<Int>(rhs);
            return Int::New(clhs->getValue()+crhs->getValue());
        }
        else if (rhs->getType() == Type_Real) 
        {
            const Real *crhs = Util::getAsConstPtr<Real>(rhs);
            return BasicPtr(new Real(clhs->getValue()+crhs->getValue()));
        }
        else if (rhs->getType() == Type_Matrix) 
        {
            const Matrix *crhs = Util::getAsConstPtr<Matrix>(rhs);
            return BasicPtr(new Matrix((*clhs)+(*crhs)));
        }
    }
    else if (lhs->getType() == Type_Real) 
    {
        const Real *clhs = Util::getAsConstPtr<Real>(lhs);
        if (rhs->getType() == Type_Int) 
        {
            const Int *crhs = Util::getAsConstPtr<Int>(rhs);
            return BasicPtr(new Real(clhs->getValue()+crhs->getValue()));
        }
        else if (rhs->getType() == Type_Real) 
        {
            const Real *crhs = Util::getAsConstPtr<Real>(rhs);
            return BasicPtr(new Real(clhs->getValue()+crhs->getValue()));
        }
        else if (rhs->getType() == Type_Matrix) 
        {
            const Matrix *crhs = Util::getAsConstPtr<Matrix>(rhs);
            return BasicPtr(new Matrix((*clhs)+(*crhs)));
        }
    }
    else if (lhs->getType() == Type_Matrix) 
    {
        const Matrix *clhs = Util::getAsConstPtr<Matrix>(lhs);
        if (rhs->getType() == Type_Int) 
        {
            const Int *crhs = Util::getAsConstPtr<Int>(rhs);
            return BasicPtr(new Matrix((*clhs)+(*crhs)));
        }
        else if (rhs->getType() == Type_Real) 
        {
            const Real *crhs = Util::getAsConstPtr<Real>(rhs);
            return BasicPtr(new Matrix((*clhs)+(*crhs)));
        }
        else if (rhs->getType() == Type_Matrix) 
        {
            const Matrix *crhs = Util::getAsConstPtr<Matrix>(rhs);
            return BasicPtr(new Matrix((*clhs)+(*crhs)));
        }
        else if (rhs->is_Scalar()) 
        {
            Matrix *c = new Matrix( *clhs );
            for (size_t i=0; i<c->getNumEl(); ++i)
              c->set(i,c->get(i)+rhs);
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
              c->set(i,lhs+c->get(i));
            return BasicPtr(c);
        }
    }
    if (lhs->getType() == Type_Add)
    {
        BasicPtrVec addargs;
        const Add *a = Util::getAsConstPtr<Add>(lhs);
        size_t argssize = a->getArgsSize();
        for (size_t j=0; j<argssize; ++j)
          addargs.push_back(a->getArg(j));

        if (rhs->getType() == Type_Add)
        {
          const Add *b = Util::getAsConstPtr<Add>(rhs);
          size_t argssize = b->getArgsSize();
          for (size_t j=0; j<argssize; ++j)
            addargs.push_back(b->getArg(j));
        }
        else
            addargs.push_back(rhs);
        return BasicPtr(new Add(addargs));
    }
    if (rhs->getType() == Type_Add)
    {
        BasicPtrVec addargs;
        addargs.push_back(lhs);
        const Add *a = Util::getAsConstPtr<Add>(rhs);
        size_t argssize = a->getArgsSize();
        for (size_t j=0; j<argssize; ++j)
          addargs.push_back(a->getArg(j));
        return BasicPtr(new Add(addargs));
    }
    return BasicPtr(new Add(lhs,rhs));
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Add::der()
/*****************************************************************************/
{
    BasicPtrVec args;
    args.reserve(getArgsSize());

    // Nun alles differenzieren
    for(size_t i=0; i<getArgsSize(); ++i)
      args.push_back(getArg(i)->der());

    // fertig
    return BasicPtr(new Add(args));
}
/*****************************************************************************/

/*****************************************************************************/
BasicPtr Add::der(BasicPtr const& symbol)
/*****************************************************************************/
{
    BasicPtrVec args;
    args.reserve(getArgsSize());

    // Nun alles differenzieren
    for(size_t i=0; i<getArgsSize(); ++i)
      args.push_back(getArg(i)->der(symbol));

    // fertig
    return BasicPtr(new Add(args));
}
/*****************************************************************************/