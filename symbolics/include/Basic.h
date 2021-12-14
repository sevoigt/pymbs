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

#ifndef __BASIC_H_
#define __BASIC_H_

#include <vector>
#include <string>
#include <set>
#include <map>
#include "Shape.h"
#include <cassert>

#include "intrusive_ptr.h"

// Forward Declarations
namespace Symbolics
{
    class Basic;
    class Argument;
};

namespace boost
{
    void intrusive_ptr_add_ref( const Symbolics::Basic *p);
    void intrusive_ptr_release( const Symbolics::Basic *p);
};

// Declarations
namespace Symbolics
{
    /*****************************************************************************/
    typedef boost::intrusive_ptr<Basic> BasicPtr;
    typedef boost::intrusive_ptr<const Basic> ConstBasicPtr;
    typedef std::vector< BasicPtr > BasicPtrVec;
    typedef std::vector< ConstBasicPtr > ConstBasicPtrVec;
    typedef Argument* ArgumentPtr;
    typedef std::vector<Argument> ArgumentPtrVec;

    typedef std::map<BasicPtr, BasicPtr> BasicPtrMap;
    /*****************************************************************************/


    /*****************************************************************************/
    typedef enum _BASIC_TYPE
    {
        Type_Bool,
        Type_Symbol,
        Type_Zero,
        Type_Eye,
        Type_Int,
        Type_Real,
        Type_Matrix,
        Type_Neg,
        Type_Add,
        Type_Mul,
        Type_Pow,
        Type_Sin,
        Type_Cos,
        Type_Der,
        Type_Element,
        Type_Atan,
        Type_Solve,
        Type_Atan2,
        Type_Abs,
        Type_Acos,
        Type_Asin,
        Type_Scalar,
        Type_Skew,
        Type_Transpose,
        Type_Unknown,
        Type_Tan,
        Type_If,
        Type_Greater,
        Type_Less,
        Type_Equal,
        Type_Sign,
        Type_Jacobian,
        Type_Outer,
        Type_Inverse
    } Basic_Type;
    /*****************************************************************************/

    /*****************************************************************************/
    class Basic
    {
    public:
        // Konstruktor
        Basic( Basic_Type const& type );
        Basic( Basic_Type const& type, Shape const& shape );
        // Destruktor
        virtual ~Basic();

        // Getter: Name
        inline Basic_Type const& getType() const { return m_type; };

        // Getter: Shape
        inline Shape const& getShape() const  { return m_shape; };
        // Abkuerzungen fuer Shape
        inline bool is_Scalar() const { return m_shape.getNrDimensions() == 0; };
        inline bool is_Vector() const { return m_shape.getNrDimensions() == 1; };
        inline bool is_Matrix() const { return m_shape.getNrDimensions() == 2; };

        // ToString
        virtual std::string toString() const  = 0;

        // Vereinfachen, wenn unverändert, dann NULL
        virtual BasicPtr simplify()  = 0;

        // Vergleich
        virtual bool operator==(  Basic const& rhs ) const  = 0;
        bool operator!=(  Basic const& rhs ) const;
        virtual bool operator<(Basic  const& rhs) const = 0;

        // Ersetzen
        virtual BasicPtr subs( ConstBasicPtr const& old_exp,  BasicPtr const& new_exp);
        void subs( BasicPtr const& new_exp);

        // atoms
        typedef std::set< BasicPtr > BasicSet;
        BasicSet getAtoms();
        virtual void getAtoms(BasicSet &atoms)  = 0;

        typedef std::map< BasicPtr, size_t > BasicSizeTMap;
        BasicSizeTMap getAtomsAmound();
        virtual void getAtoms(BasicSizeTMap &atoms)  = 0;

        // args
        virtual size_t getArgsSize() const = 0;
        virtual BasicPtr const& getArg(size_t i) const = 0;

        // derivative if time
        virtual BasicPtr der() = 0;
        // derivative of symbol
        virtual BasicPtr der(BasicPtr const& symbol) = 0;

        BasicPtr solve(BasicPtr exp);

        // hash
        virtual inline size_t getHash() const { return m_hash; };

        // iterate through the expression
        class Iterator
        {
        public:
          virtual BasicPtr process_Arg(BasicPtr const &p) = 0;
        };
        virtual  BasicPtr iterateExp(Iterator &v);

        class Scanner
        {
        public:
            virtual bool process_Arg(BasicPtr const &p, bool &stop) = 0;
        };
        // stops if Scanner return false, itterates the expression top-down
        virtual void scanExp(Scanner &s);

  		// Parent
        // Wir halten hier direkt den Pointer, damit der Refcount nicht erhoeht wird und sich keine zirkulaeren Bezuege ergeben
		    inline void addParent( ArgumentPtr parent ) { m_parents.insert( parent ); };
		    inline void removeParent( ArgumentPtr parent ) 
		    { 
			    if(m_parents.erase( parent ) != 1) 
				    throw InternalError("Hier ist was faul mit Papi");
		    };
		    inline void clearParents() { m_parents.clear(); };
		    typedef std::set<ArgumentPtr> ArgumentPtrSet;
		    inline ArgumentPtrSet& getParents() { return m_parents; };
        inline size_t getParentsSize() const { return m_parents.size(); };

        class LookFor: public Scanner
        {
        public:
            LookFor(BasicPtr const &lookfor) :m_lookfor(lookfor),found(false) {;}
            ~LookFor() {;}
            bool process_Arg(BasicPtr const &p, bool &stop);
            bool found;
        protected:
            BasicPtr m_lookfor;
        };

    protected:
        // Name
        Basic_Type m_type;
        // Shape
        Shape m_shape;
        // simplifed Flag
        bool m_simplified;
        // hash
        size_t m_hash;

        virtual void calcHash();

		    // Referenz auf Parent, Referenz damit RefCount nicht veraendert wird und sich damit keine zirulaere Referenz ergibt
		    ArgumentPtrSet m_parents;

        void changed();

        class LookForMultiple: public Scanner
        {
        public:
            LookForMultiple(BasicSet &lookfor) :m_lookfor(lookfor),found(false) {;}
            ~LookForMultiple() {;}
            bool process_Arg(BasicPtr const &p, bool &stop);
            bool found;
        protected:
            BasicSet &m_lookfor;
        };
    private:
        // Boost Intrusive Ptr
        unsigned int m_refCount;
        // friend
        friend void ::boost::intrusive_ptr_add_ref( const Basic* p);
        friend void ::boost::intrusive_ptr_release( const Basic* p);
    };
    /*****************************************************************************/

    /*****************************************************************************/
      class Argument
      {
      public:
        // Konstruktor
        Argument(): m_parent(0)  {;}
        Argument(Basic* parent): m_parent(0) { if(parent) m_parent = parent;}
        Argument(Basic* parent, BasicPtr arg): m_parent(0) { if(parent) m_parent = parent; setArg(arg); }
        Argument(BasicPtr arg): m_parent(0) { setArg(arg); }
        Argument(const Argument& a): m_parent(0)
        {
          setArg(a.m_arg);
          registerParent(a.m_parent);
        };
        // Destruktor
        ~Argument() { unregisterParent(); }
        // Argument
        inline void setArg(BasicPtr const& arg) { if (m_arg.get() != NULL) m_arg->removeParent(this); m_arg = arg; m_arg->addParent(this); }
        inline BasicPtr const& getArg() const { return m_arg; }
        // Parent
        inline void registerParent( Basic* parent ) { if(parent) m_parent = parent; }
        inline void unregisterParent() { m_parent=0; if (m_arg.get() != NULL) m_arg->removeParent(this); m_arg=NULL; }
        inline BasicPtr getParent() const { return BasicPtr(m_parent); };
      protected:
        // Argument
        BasicPtr m_arg;
        // Parent
        Basic* m_parent;
      };
    /*****************************************************************************/
};

/*****************************************************************************/
namespace boost
{
    // increment reference count of object *p
    inline void intrusive_ptr_add_ref( const Symbolics::Basic* p)
    {
        // Muss sein, da ich sonst keine  Basics in intrusive Ptr legen kann
        Symbolics::Basic* b = const_cast<Symbolics::Basic*>(p);
        ++(b->m_refCount);
    }

    // decrement reference count, and delete object when reference count reaches 0
    inline void intrusive_ptr_release( const Symbolics::Basic* p)
    {
        // Muss sein, da ich sonst keine  Basics in intrusive Ptr legen kann
        Symbolics::Basic* b = const_cast<Symbolics::Basic*>(p);
        if (--(b->m_refCount) == 0)
            delete p;
    } 

}; // namespace boost
/*****************************************************************************/

// Damit alle die Basic inkludieren, auch die Operatoren dazu kennen
#include "Operators.h"

#endif // __BASIC_H_
