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

#include <iostream>
#include "Symbolics.h"

using namespace Symbolics;

int main( int argc,  char *argv[])
{
    // Beispiel aufbauen
    // subs(exp,old,new)

    // subs(1.0,1.0,2.0) = 2.0
    BasicPtr cdbl(new Real(1.0));
    BasicPtr cdbl_two(new Real(2.0));
    BasicPtr subs_exp = cdbl->subs(cdbl,cdbl_two);
    if (subs_exp != cdbl_two) return -1;

    // subs(1,1,2) = 2
    BasicPtr cint(new Int(1));
    BasicPtr cint_two(new Int(2));
    BasicPtr subs_exp1 = cint->subs(cint,cint_two);
    if (subs_exp1 != cint_two) return -2;

    // subs(a,a,b)
    BasicPtr a(new Symbol("a"));
    BasicPtr b(new Symbol("b"));
    BasicPtr subs_exp2 = a->subs(a,b);
    if (subs_exp2 != b) return -3;

    // subs(-1.0,1.0,2.0) = -2.0
    BasicPtr exp(new Neg(cdbl));
    BasicPtr subs_exp3 = exp->subs(cdbl,cdbl_two)->simplify();
    BasicPtr exp1 = Neg::New(cdbl_two);
    if (subs_exp3 != exp1) return -4;

    // subs(a+b,a,b) = b+b
    BasicPtr exp2 = Add::New(a,b);
    BasicPtr subs_exp4 = exp2->subs(a,b)->simplify();
    BasicPtr exp3 = Mul::New(cint_two,b);
    if (subs_exp4 != exp3) return -5;

    // subs(a*b,a,b) = b*b
    BasicPtr exp4 = Mul::New(a,b);
    BasicPtr subs_exp5 = exp4->subs(a,b);
    BasicPtr exp5 = Mul::New(b,b);
    if (subs_exp5 != exp5) return -6;

    // subs(1.0,1,2) = 1.0
    BasicPtr subs_exp_6 = cdbl->subs(cint,cint_two);
    if (subs_exp_6 != cint_two) return -7;

    // subs(a,b,1.0) = a
    BasicPtr subs_exp_7 = a->subs(b,cdbl);
    if (subs_exp_7 != a) return -8;

    // subs(a+b,c,1.0) = a
    BasicPtr c(new Symbol("c"));
    BasicPtr subs_exp_8 = exp2->subs(c,cdbl);
    if (subs_exp_8 != exp2) return -9;

    // subs(a*b,c,1.0) = a
    BasicPtr subs_exp_9 = exp4->subs(c,cdbl);
    if (subs_exp_9 != exp4) return -10;

    // subs(-a,c,1.0) = -a
    BasicPtr exp6 = Neg::New(a);
    BasicPtr subs_exp_10 = exp6->subs(c,cdbl);
    if (subs_exp_10 != exp6) return -11;

    // Subs(A+B, A, Skew(a3)) = Skew(a)+B
    BasicPtr A( new Symbol("A", Shape(3,3)));
    BasicPtr B( new Symbol("B", Shape(3,3)));
    BasicPtr a3( new Symbol("a", Shape(3)));
    BasicPtr AplusB( new Add(A,B) );
    BasicPtr skewa( new Skew(a3) );
    BasicPtr skewaplusB( new Add(skewa, B) );
    BasicPtr exp11( AplusB->subs(A, skewa) );
    if (exp11 != skewaplusB) return -12;


    // 
    BasicPtr exp12 = Add::New(a,Mul::New(a,Util::div(a,Pow::New(Util::sqrt(a),Int::New(2)))));
    a->subs(b);
    Basic::BasicSet atoms =exp12->getAtoms();
    Basic::BasicSet::iterator found = atoms.find(a);
    if (found != atoms.end()) return -20;

    found = atoms.find(b);
    if (found == atoms.end()) return -21;

    // subs(der(a),b)
    BasicPtr dera = Der::New(a);
    BasicPtr exp13 = dera->subs(Der::New(a),b);
    if (exp13 != b) return -22;

    return 0;
}