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

int test_Constructor( int &argc,  char *argv[])
{
    // Beispiel aufbauen
    Shape a;
    if (a.getNrDimensions() != 0) return -1; 
    try
    {
        if (a.getDimension(1) != 1) return -2; 
    }
    catch(ShapeError)
    {
    }
    catch(...)
    {
        return -3;
    }

    try
    {
      if (a.getDimension(2) != 1) return -4; 
    }
    catch(ShapeError)
    {
    }
    catch(...)
    {
        return -5;
    }


    Shape b(1,1);
    if (b.getNrDimensions() != 2) return -6; 
    if (b.getDimension(1) != 1) return -7; 
    if (b.getDimension(2) != 1) return -8; 


    Shape c(1);
    if (c.getNrDimensions() != 1) return -9; 
    if (c.getDimension(1) != 1) return -10; 
    try
    {
      if (c.getDimension(2) != 1) return -11; 
    }
    catch(ShapeError)
    {
    }
    catch(...)
    {
        return -12;
    }

    try
    {
      Shape d(0);
    }
    catch(ShapeError)
    {
    }
    catch(...)
    {
        return -13;
    }

    return 0;
}


int test_OperatorAdd( int &argc,  char *argv[])
{
    // Beispiel aufbauen
    Shape a;
    Shape b;
    Shape c = a+b;
    if (c.getNrDimensions() != 0) return -14; 
    try
    {
      if (c.getDimension(1) != 1) return -15; 
    }
    catch(ShapeError)
    {
    }
    catch(...)
    {
        return -16;
    }
    try
    {
      if (c.getDimension(2) != 1) return -17; 
    }
    catch(ShapeError)
    {
    }
    catch(...)
    {
        return -18;
    }

    Shape d(1);
    Shape e;
    Shape f=d+e;
    if (f.getNrDimensions() != 1) return -19; 
    if (f.getDimension(1) != 1) return -20; 
    try
    {
      if (f.getDimension(2) != 1) return -21; 
    }
    catch(ShapeError)
    {
    }
    catch(...)
    {
        return -22;
    }

    Shape ff=e+d;
    if (ff.getNrDimensions() != 1) return -23; 
    if (ff.getDimension(1) != 1) return -24; 
    try
    {
      if (ff.getDimension(2) != 1) return -25; 
    }
    catch(ShapeError)
    {
    }
    catch(...)
    {
        return -26;
    }

    Shape g(1,1);
    Shape h;
    Shape i=g+h;
    if (i.getNrDimensions() != 2) return -27; 
    if (i.getDimension(1) != 1) return -28; 
    if (i.getDimension(2) != 1) return -29; 

    Shape ii=h+g;
    if (ii.getNrDimensions() != 2) return -30; 
    if (ii.getDimension(1) != 1) return -31; 
    if (ii.getDimension(2) != 1) return -32; 
    


    Shape j(1);
    Shape k(1,1);
    try
    {
      Shape l=j+k;
    }
    catch(ShapeError)
    {
    }
    catch(...)
    {
        return -33;
    }

    return 0;
}

int test_OperatorMul( int &argc,  char *argv[])
{
    // Beispiel aufbauen
    Shape a;
    Shape b;
    Shape c = a*b;
    if (c.getNrDimensions() != 0) return -34; 
    try
    {
      if (c.getDimension(1) != 1) return -35; 
    }
    catch(ShapeError)
    {
    }
    catch(...)
    {
        return -36;
    }
    try
    {
      if (c.getDimension(2) != 1) return -37; 
    }
    catch(ShapeError)
    {
    }
    catch(...)
    {
        return -38;
    }

    Shape d(1);
    Shape e;
    Shape f=d*e;
    if (f.getNrDimensions() != 1) return -39; 
    if (f.getDimension(1) != 1) return -40; 
    try
    {
      if (f.getDimension(2) != 1) return -41; 
    }
    catch(ShapeError)
    {
    }
    catch(...)
    {
        return -42;
    }

    Shape ff=e*d;
    if (ff.getNrDimensions() != 1) return -43; 
    if (ff.getDimension(1) != 1) return -44; 
    try
    {
      if (ff.getDimension(2) != 1) return -45; 
    }
    catch(ShapeError)
    {
    }
    catch(...)
    {
        return -46;
    }

    Shape g(1,1);
    Shape h;
    Shape i=g*h;
    if (i.getNrDimensions() != 2) return -47; 
    if (i.getDimension(1) != 1) return -48; 
    if (i.getDimension(2) != 1) return -49; 

    Shape ii=h*g;
    if (ii.getNrDimensions() != 2) return -50; 
    if (ii.getDimension(1) != 1) return -51; 
    if (ii.getDimension(2) != 1) return -52; 
    


    Shape j(1);
    Shape k(1,1);
    Shape l=j*k;
    if (l.getNrDimensions() != 1) return -53; 
    if (l.getDimension(1) != 1) return -54; 
    try
    {
      if (l.getDimension(2) != 1) return -55; 
    }
    catch(ShapeError)
    {
    }
    catch(...)
    {
        return -56;
    }
    Shape m=k*j;
    if (m.getNrDimensions() != 1) return -57; 
    if (m.getDimension(1) != 1) return -58; 
    try
    {
      if (m.getDimension(2) != 1) return -59; 
    }
    catch(ShapeError)
    {
    }
    catch(...)
    {
        return -60;
    }


    Shape n(1,1,3);
    Shape o(3,2);
    Shape p=n*o;
    if (p.getNrDimensions() != 1) return -61; 
    if (p.getDimension(1) != 1) return -62; 
    try
    {
      if (p.getDimension(2) != 2) return -63; 
    }
    catch(ShapeError)
    {
    }
    catch(...)
    {
        return -64;
    }

    try
    {
        Shape q=o*n;
    }
    catch(ShapeError)
    {
    }
    catch(...)
    {
        return -68;
    }

    Shape r(3,3);
    Shape s(3,3);
    Shape t = r*s;
    if (t.getNrDimensions() != 2) return -69; 
    if (t.getDimension(1) != 3) return -70; 
    if (t.getDimension(2) != 3) return -71; 

    return 0;
}


int test_OperatorCompareLess( int &argc,  char *argv[])
{
    // Beispiel aufbauen
    Shape a;
    Shape b;
    if (a != b) return -100;
    if (a < b) return -101;
    if (b < a) return -102;

    Shape d(1);
    Shape e;
    Shape f(1);
    if (a == d) return -103;
    if (e < a) return -104;
    if (f < d) return -105;
    if (d < f) return -106;
    if (d != f) return -107;

    Shape g(1,1);
    Shape h(1,1);

    if (a == g) return -108;
    if (g < a) return -109;
    if (g < d) return -110;
    if (h < g) return -111;
    if (g < h) return -112;
    if (g != h) return -113;
    if (!(a < g)) return -114;

    return 0;
}

int main( int argc,  char *argv[])
{

    int ret=0;
    ret = test_Constructor(argc,argv);
    if( ret != 0) return ret;
    ret = test_OperatorAdd(argc,argv);
    if( ret != 0) return ret;
    ret = test_OperatorMul(argc,argv);
    if( ret != 0) return ret;
    ret = test_OperatorCompareLess(argc,argv);
    if( ret != 0) return ret;

    return 0;
}