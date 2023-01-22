#ifndef __UNARY_OP_H_
#define __UNARY_OP_H_

#include "Basic.h"
#include <vector>

namespace Symbolics
{

    class UnaryOp: public Basic
    {
    public:
        // Konstruktor
        UnaryOp(  Basic_Type type,  BasicPtrVec const& args );        
        // Konstruktor
        UnaryOp(  Basic_Type type, BasicPtr const& arg );
        // Destruktor
        virtual ~UnaryOp();

        // Vergleichsoperator
        bool operator==( Basic const& rhs) const;
        bool operator<(Basic  const& rhs) const;

        // Argument bekommen
        inline BasicPtr const& getArg() const { return m_arg.getArg(); };

        inline void getAtoms(Basic::BasicSet &atoms) { getArg()->getAtoms(atoms);} ;
        inline void getAtoms(BasicSizeTMap &atoms) { getArg()->getAtoms(atoms);} ;

        // Ersetzen
        BasicPtr subs( ConstBasicPtr const& old_exp,  BasicPtr const& new_exp);

        // args
        inline size_t getArgsSize() const { return 1; };
        inline BasicPtr const& getArg(size_t i) const { return getArg(); };

    protected:
        inline void setArg( BasicPtr const& arg) { m_arg.setArg(arg); };
        inline void simplifyArg() { setArg(getArg()->simplify()); };
    
    private:
        // Arguments
        Argument m_arg;
    };

    typedef boost::intrusive_ptr< UnaryOp> ConstUnaryOpPtr;
    typedef boost::intrusive_ptr<UnaryOp> UnaryOpPtr;

};

#endif // __UNARY_OP_H_
