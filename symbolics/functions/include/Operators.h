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