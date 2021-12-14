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

#include "test_abs.h"
#include "test_acos.h"
#include "test_add.h"
#include "test_asin.h"
#include "test_atan.h"
#include "test_atan2.h"
#include "test_cos.h"
#include "test_der.h"
#include "test_element.h"
#include "test_equal.h"
#include "test_greater.h"
#include "test_if.h"
#include "test_less.h"
#include "test_mul.h"
#include "test_neg.h"
#include "test_pow.h"
#include "test_scalar.h"
#include "test_sign.h"
#include "test_sin.h"
#include "test_skew.h"
#include "test_solve.h"
#include "test_tan.h"
#include "test_transpose.h"
#include "test_unknown.h"
#include "test_outer.h"
#include "test_jacobian.h"

using namespace Symbolics;

int main( int argc,  char *argv[])
{
    // Returncode
    int rc;

    // Einzeltests aufrufen
    if ( (rc = test_abs(argc, argv)) != 0 )
        return -1000 + rc;
    if ( (rc = test_acos(argc, argv)) != 0 )
        return -2000 + rc;
    if ( (rc = test_add(argc, argv)) != 0 )
        return -3000 + rc;
    if ( (rc = test_asin(argc, argv)) != 0 )
        return -4000 + rc;
    if ( (rc = test_atan(argc, argv)) != 0 )
        return -5000 + rc;
    if ( (rc = test_atan2(argc, argv)) != 0 )
        return -6000 + rc;
    if ( (rc = test_cos(argc, argv)) != 0 )
        return -7000 + rc;
    if ( (rc = test_der(argc, argv)) != 0 )
        return -8000 + rc;
    if ( (rc = test_element(argc, argv)) != 0 )
        return -9000 + rc;
    if ( (rc = test_mul(argc, argv)) != 0 )
        return -11000 + rc;
    if ( (rc = test_neg(argc, argv)) != 0 )
        return -12000 + rc;
    if ( (rc = test_pow(argc, argv)) != 0 )
        return -13000 + rc;
    if ( (rc = test_scalar(argc, argv)) != 0 )
        return -14000 + rc;
    if ( (rc = test_sin(argc, argv)) != 0 )
        return -15000 + rc;
    if ( (rc = test_skew(argc, argv)) != 0 )
        return -16000 + rc;
    if ( (rc = test_solve(argc, argv)) != 0 )
        return -17000 + rc;
    if ( (rc = test_transpose(argc, argv)) != 0 )
        return -18000 + rc;
    if ( (rc = test_unknown(argc, argv)) != 0 )
        return -19000 + rc;
    if ( (rc = test_tan(argc, argv)) != 0 )
        return -20000 + rc;
    if ( (rc = test_equal(argc, argv)) != 0 )
        return -21000 + rc;
    if ( (rc = test_greater(argc, argv)) != 0 )
        return -22000 + rc;
    if ( (rc = test_if(argc, argv)) != 0 )
        return -23000 + rc;
    if ( (rc = test_less(argc, argv)) != 0 )
        return -24000 + rc;
    if ( (rc = test_sign(argc, argv)) != 0 )
        return -25000 + rc;
    if ( (rc = test_outer(argc, argv)) != 0 )
        return -26000 + rc;
    if ( (rc = test_jacobian(argc, argv)) != 0 )
        return -27000 + rc;
}