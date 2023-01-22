#ifndef __LIST_H_
#define __LIST_H_

#include "Basic.h"
#include "Error.h"

// Forward Declarations
namespace Symbolics
{
    class Scope;
    class List;
};

namespace boost
{
    void intrusive_ptr_add_ref( const Symbolics::Scope *p);
    void intrusive_ptr_release( const Symbolics::Scope *p);
};

namespace Symbolics
{
/*****************************************************************************/
    typedef boost::intrusive_ptr<Scope> ScopePtr;
/*****************************************************************************/


/*****************************************************************************/
    class Scope 
    {
    public:
        // Konstruktor
        Scope( ): first(NULL), last(NULL), m_refCount(0) {;};
        ~Scope( ) {;};
        List* first;
        List* last;
    private:
        // Boost Intrusive Ptr
        unsigned int m_refCount;
        // friend
        friend void ::boost::intrusive_ptr_add_ref( const Scope* p);
        friend void ::boost::intrusive_ptr_release( const Scope* p);
    };

/*****************************************************************************/
    class List 
    {
    public:
        inline List* getLeft() { return m_left; };
        inline List* getRigth() { return m_right; };

        template< typename Ty >
        static Ty* getAsPtr( List *l )
        {
            if (l == NULL) throw InternalError("Internal Error: List::getAsPtr!");
            // Zeiger casten
            Ty* obj = dynamic_cast<Ty*>(l);
            if (obj == NULL) throw InternalError("Internal Error: dynamic_cast returned NULL!");
            return obj;
        };
    protected:
        List(ScopePtr scope);
        virtual ~List();

        inline void setLeft(List* l) {m_left = l; };
        inline void setRight(List* r) {m_right = r; };
    private:
        List* m_left;
        List* m_right;

        ScopePtr m_scope;
    };
/*****************************************************************************/
}


/*****************************************************************************/
namespace boost
{
    // increment reference count of object *p
    inline void intrusive_ptr_add_ref( const Symbolics::Scope* p)
    {
        // Muss sein, da ich sonst keine  Basics in intrusive Ptr legen kann
        Symbolics::Scope* b = const_cast<Symbolics::Scope*>(p);
        ++(b->m_refCount);
    }

    // decrement reference count, and delete object when reference count reaches 0
    inline void intrusive_ptr_release( const Symbolics::Scope* p)
    {
        // Muss sein, da ich sonst keine  Basics in intrusive Ptr legen kann
        Symbolics::Scope* b = const_cast<Symbolics::Scope*>(p);
        if (--(b->m_refCount) == 0)
            delete p;
    } 

}; // namespace boost
/*****************************************************************************/

#endif // __LIST_H_
