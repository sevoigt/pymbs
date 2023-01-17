#ifndef __OPERATORS_H_
#define __OPERATORS_H_

#include "Basic.h"

namespace Symbolics
{
    // Operatoren mit Basics
    BasicPtr operator+ (BasicPtr const& lhs, BasicPtr  const& rhs);
    BasicPtr operator- (BasicPtr const& lhs, BasicPtr  const& rhs);
    BasicPtr operator* (BasicPtr const& lhs, BasicPtr  const& rhs);
    BasicPtr operator- (BasicPtr const& op);
    bool operator== (BasicPtr const& lhs, BasicPtr  const& rhs);
    bool operator!= (BasicPtr const& lhs, BasicPtr  const& rhs);
    // Vergleich zum Sortieren in std::map, keine inhaltliche Bedeutung!
    bool operator<( BasicPtr const& lhs, BasicPtr  const& rhs);
    bool operator>( BasicPtr const& lhs, BasicPtr  const& rhs);

    // Operatoren mit doubles
    BasicPtr operator+ (double lhs, BasicPtr  const& rhs);
    BasicPtr operator+ (BasicPtr const& lhs, double rhs);
    BasicPtr operator- (double lhs, BasicPtr  const& rhs);
    BasicPtr operator- (BasicPtr const& lhs, double rhs);
    BasicPtr operator* (double lhs, BasicPtr  const& rhs);
    BasicPtr operator* (BasicPtr const& lhs, double rhs);
    bool operator== (double const& lhs, BasicPtr  const& rhs);
    bool operator!= (double const& lhs, BasicPtr  const& rhs);
    bool operator== (BasicPtr const& lhs, double  const& rhs);
    bool operator!= (BasicPtr const& lhs, double  const& rhs);
    // Operatoren mit ints
    BasicPtr operator+ (int lhs, BasicPtr  const& rhs);
    BasicPtr operator+ (BasicPtr const& lhs, int rhs);
    BasicPtr operator- (int lhs, BasicPtr  const& rhs);
    BasicPtr operator- (BasicPtr const& lhs, int rhs);
    BasicPtr operator* (int lhs, BasicPtr  const& rhs);
    BasicPtr operator* (BasicPtr const& lhs, int rhs);
    bool operator== (int const& lhs, BasicPtr  const& rhs);
    bool operator!= (int const& lhs, BasicPtr  const& rhs);
    bool operator== (BasicPtr const& lhs, int  const& rhs);
    bool operator!= (BasicPtr const& lhs, int  const& rhs);

};

#endif