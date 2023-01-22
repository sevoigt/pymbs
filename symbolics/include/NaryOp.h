#ifndef __NARY_OP_H_
#define __NARY_OP_H_

#include "Basic.h"
#include <vector>

namespace Symbolics
{

    class NaryOp: public Basic
    {
    public:
        // Konstruktor
        NaryOp( Basic_Type type, BasicPtrVec const& args );
		    NaryOp( Basic_Type type, BasicPtr const& arg1, BasicPtr const& arg2 );

        // Destruktor
        virtual ~NaryOp();

        // Vergleich
        bool operator==( Basic const& rhs) const;
        bool operator<(Basic  const& rhs) const;

        // Atoms
        void getAtoms(Basic::BasicSet& atoms);
        void getAtoms(BasicSizeTMap &atoms);

        // Ersetzen
        BasicPtr subs( ConstBasicPtr const& old_exp,  BasicPtr const& new_exp);

        // args
        inline size_t getArgsSize() const { return m_args.size(); };
        inline BasicPtr const& getArg(size_t i) const { if (i > m_args.size()) throw InternalError("Args Index out of bounce!"); return m_args[i].getArg(); }; 

    protected:

        // join arguments, by seperator
        std::string join( std::string const& sep) const;
        std::string join( std::string const& posSep,
                           std::string const& negSep) const;

        inline void setArg(size_t i, BasicPtr const& arg) { m_args[i].setArg(arg); };
        inline void clearArgs() {	m_args.clear(); };
        inline void reserveArgs(size_t n) { m_args.reserve(n); };
        inline void addArg( BasicPtr const& arg ) { m_args.push_back(Argument(this,arg)); };
        inline void simplifyArgs() { for (size_t i=0; i<m_args.size(); ++i) setArg(i,m_args[i].getArg()->simplify()); };
        
    private:
        // Arguments
        ArgumentPtrVec m_args;
    };

    typedef boost::intrusive_ptr< NaryOp> ConstNaryOpPtr;
    typedef boost::intrusive_ptr<NaryOp> NaryOpPtr;

};

#endif // __NARY_OP_H_
