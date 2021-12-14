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

#ifndef __SYMBOL_H_
#define __SYMBOL_H_

#ifdef _DEBUG
#include <set>
#endif
#include "Basic.h"

namespace Symbolics
{

/*****************************************************************************/
    typedef size_t Symbol_Kind;
/*****************************************************************************/
    typedef enum _CATEGORY_
    {
        VARIABLE = 0x01,                   // Normal Variables
        CONSTANT = 0x02,                   // A Variable which never changes its value
        PARAMETER = 0x04,                  // Parameters
        INPUT = 0x08,                      // Input
        CONTROLLER = 0x400,                // Controller
        USER_EXP = 0x40,                   // Expressions Defined by the User
    } Category;
/*****************************************************************************/
    typedef enum _STATECATEGORY_
    {
      NO_STATE = 0x00,                     // no State, normal Variable
      STATE = 0x10,                        // Map State to Generalised Coordinates/Constraint Forces/Lagrangian Multipliers
      DER_STATE = 0x20,                    // State Derivative 
      DUMMY_STATE = 0x40                   // was a State
    } STATECATEGORY;
/*****************************************************************************/
    typedef enum _ID_USERDATA_
    {
        ID_UD_STATEDERIVATIVE = 0x00,
        ID_UD_STATE = 0x01,
        ID_UD_SYMBOL = 0x02,
        ID_UD_NODE = 0x03,
    } ID_USERDATA;
/*****************************************************************************/
    typedef enum SymbolElementKindT {
      ALL = 2,
      // RANGE = 4, Not relay usefull
      SEVERAL = 8,
    } SymbolElementKind;
/*****************************************************************************/

    class Symbol: public Basic
    {
    public:
        // Konstruktor
        Symbol( std::string const& name,  Shape const& shape,  Symbol_Kind category = VARIABLE );
        // Konstruktor (Skalar)
        Symbol( std::string const& name,  Symbol_Kind category = VARIABLE );
        // Destruktor
        ~Symbol();

        // toString
        std::string toString() const ; 
        inline std::string const& getName() const {return m_name;} ;

        // Vereinfachen, wenn unverändert, dann NULL
        inline BasicPtr simplify() { return BasicPtr(this); };

        // Vergleich
        bool operator== ( Basic const& rhs) const;
        bool operator<(Basic  const& rhs) const;

        // Ersetzen
        BasicPtr subs( ConstBasicPtr const& old_exp,  BasicPtr const& new_exp);

        inline Symbol_Kind getKind() const { return m_kind; };

        void setState();
        void setState(size_t dim1, size_t dim2);
        void setVariable(size_t dim1, size_t dim2);

        Symbol* getDerivative();

        inline SymbolElementKind stateKind() const { return m_statekind; };
        inline size_t is_State(size_t dim1, size_t dim2) const { return m_state[getIndex(dim1,dim2)]; };

        // Atoms
        inline void getAtoms(Basic::BasicSet& atoms) { atoms.insert(BasicPtr(this)); };
        inline void getAtoms(BasicSizeTMap &atoms) { BasicSizeTMap::iterator ii=atoms.find(BasicPtr(this)); if (ii!=atoms.end()){ (*ii).second++ ;} else { atoms[BasicPtr(this)]=1;}; };

        // args
        inline size_t getArgsSize() const { return 0; };
        inline BasicPtr const& getArg(size_t i) const { return getArgres; };

        // iterate through the expression
         BasicPtr iterateExp(Iterator &v);

        // user Data
        inline void setUserData(void *d, size_t id) { while(m_userData.size()<=id) { m_userData.push_back(NULL);}; m_userData[id]=d; };
        inline  void* getUserData(size_t id) const { return (id<m_userData.size())?m_userData[id]:NULL; };

        // derivative
        BasicPtr der();
        BasicPtr der(BasicPtr const& symbol);

    protected:
        // Name
        std::string m_name;
        // Kind
        Symbol_Kind m_kind;
        // State
        SymbolElementKind m_statekind;
        size_t *m_state;
        inline size_t getIndex(size_t row, size_t col) const { return row*m_shape.getDimension(2) + col; };

        // user Data
        std::vector<void*> m_userData;
        void calcHash();
#ifdef _DEBUG
        static std::set<std::string> symbol_names;
#endif
    private:
        static BasicPtr getArgres;
};
/*****************************************************************************/
    typedef boost::intrusive_ptr<const Symbol> ConstSymbolPtr;
    typedef boost::intrusive_ptr<Symbol> SymbolPtr;
    typedef std::vector<SymbolPtr> SymbolPtrVec;
    typedef std::set<SymbolPtr> SymbolPtrSet;
    typedef std::map< SymbolPtr, size_t > SymbolSizeTMap;
    typedef std::map<SymbolPtr,SymbolPtr> SymbolMap;
/*****************************************************************************/
};



#endif // __SYMBOL_H_
