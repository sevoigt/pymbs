#ifndef __UNKNOWN_H_
#define __UNKNOWN_H_

#include "NaryOp.h"

namespace Symbolics
{
    class Unknown: public NaryOp
    {
    public:
        // Konstruktor
        Unknown(  std::string const& name, BasicPtrVec const& args );
        // Destruktor
        ~Unknown();
    
        // Name
        inline std::string getName() const  { return m_name; };

        // toString
        std::string toString() const;

        // Vereinfachen, wenn unver�ndert, dann NULL
        BasicPtr simplify();

        // Ersetzen
         BasicPtr subs( ConstBasicPtr const& old_exp,  BasicPtr const& new_exp);
        // Iterate
         BasicPtr iterateExp( Iterator &v );

		    static BasicPtr New( std::string const& name, BasicPtrVec const& args );

        // derivative
        BasicPtr der();
        BasicPtr der(BasicPtr const& symbol);

    protected:
        // Name der Unbekannten Funktion
         std::string m_name;
    };

    typedef boost::intrusive_ptr< Unknown> ConstUnknownPtr;
    typedef boost::intrusive_ptr<Unknown> UnknownPtr;
};

#endif // __UNKNOWN_H_