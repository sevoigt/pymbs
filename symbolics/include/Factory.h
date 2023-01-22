#ifndef __FACTORY_H_
#define __FACTORY_H_

#include "Symbolics.h"

// Declarations
namespace Symbolics
{
    /*****************************************************************************/
    class Factory
    {
    private:
        Factory();
        ~Factory();

    public:
        static BasicPtr newBasic( Basic_Type type,  BasicPtr &arg, Shape const& shape );
        static BasicPtr newBasic( Basic_Type type,  BasicPtr &arg1, BasicPtr &arg2, Shape const& shape );
        static BasicPtr newBasic( Basic_Type type,  BasicPtrVec &args, Shape const& shape );
    };
    /*****************************************************************************/
};
#endif // __FACTORY_H_