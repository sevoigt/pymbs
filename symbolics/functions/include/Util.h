#ifndef __Util_H_
#define __Util_H_

#include "Basic.h"
#include "Error.h"

namespace Symbolics
{
/*****************************************************************************/
    class Util 
    {
    public:
        static double getTime();

        static BasicPtr intDbl( BasicPtr const& cint);

        static BasicPtr pow( BasicPtr const& lhs, BasicPtr const& rhs);
        static BasicPtr pow( BasicPtr const& lhs, double const& rhs);
        static BasicPtr pow( BasicPtr const& lhs, int const& rhs);

        static BasicPtr div( BasicPtr const& lhs, BasicPtr const& rhs);

        static BasicPtr sqrt( BasicPtr const& arg);

        template< typename Ty >
        static Ty* getAsPtr( const BasicPtr & basic )
        {
            // Zeiger holen
            Basic *b = basic.get();
            if (b == NULL) throw InternalError("getAsPtr: get() returned NULL!");
            // Zeiger casten
            Ty* obj = dynamic_cast<Ty*>(b);
            if (obj == NULL) throw InternalError("getAsPtr: dynamic_cast returned NULL!");
            return obj;
        }

        template< typename Ty >
        static const Ty* getAsConstPtr( ConstBasicPtr const& basic )
        {
            // Zeiger holen
            const Basic *b = basic.get();
            if (b == NULL) throw InternalError("getAsConstPtr: get() returned NULL!");
            // Zeiger casten
            const Ty* obj = dynamic_cast<const Ty*>(b);
            if (obj == NULL) throw InternalError("getAsConstPtr: dynamic_cast returned NULL!");
            return obj;
        }
        
        static bool is_Zero( BasicPtr const& cint);

        static bool is_One( BasicPtr const& cint);

        static bool is_Const( BasicPtr const& arg);
        
        static bool has_Function( BasicPtr const& exp, Basic_Type const& type);

        static bool is_Int(BasicPtr const& basic, int &i_Out);

        static BasicPtr eye( Shape const& s);

        static void newScope();
    };
/*****************************************************************************/

}

#endif // __Util_H_