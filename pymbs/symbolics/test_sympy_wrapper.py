'''
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
'''

#-------------------------------------------------------------------------------
# Name:        module1
# Purpose:
#
# Author:      Schubert
#
# Created:     18.07.2011
# Copyright:   (c) Schubert 2011
# Licence:     <your licence>
#-------------------------------------------------------------------------------
#!/usr/bin/env python

from . import symbolics

def main():
	print("Creating Symbols ...")
	a = symbolics.CSymbol("a")
	b = symbolics.CSymbol("b")
	print(a)
	print(b)
	
	
	print("Testing + ...")
	aplusb = a+b
	print(aplusb)
	aplusOne = a+1
	print(aplusOne)	
	aplusOnePointTwo = a+1.2
	print(aplusOnePointTwo)	
	Oneplusb = 1+b
	print(Oneplusb)	
	OnePointTwoplusb = 1.2+b
	print(OnePointTwoplusb)	

	
	print("Testing - ...")
	aminusb = a-b
	print(aminusb)
	aminusOne = a-1
	print(aminusOne)	
	aminusOnePointTwo = a-1.2
	print(aminusOnePointTwo)	
	Oneminusb = 1-b
	print(Oneminusb)	
	OnePointTwominusb = 1.2-b
	print(OnePointTwominusb)	

	
	print("Testing * ...")
	atimesb = a*b
	print(atimesb)
	atimesOne = a*1
	print(atimesOne)	
	atimesOnePointTwo = a*1.2
	print(atimesOnePointTwo)	
	Onetimesb = 1*b
	print(Onetimesb)	
	OnePointTwotimesb = 1.2*b
	print(OnePointTwotimesb)	
	
	
	print("Testing - ...")
	nega = -a
	print(nega)
	
	
	print("Testing abs ...")
	absa = abs(a)
	print(absa)
	
	
	print("Testing pow ...")
	powa = a**2
	print(powa)
	
	
	print("Testing shape ... ")
	print(a.shape())
	A = symbolics.CSymbol('A', (3,4))
	print(A.shape())

	print("Testing Acos ...")
	acos_a = symbolics.acos(a)
	print(acos_a)	

	print("Testing Asin ...")
	asin_a = symbolics.asin(a)
	print(asin_a)	

	print("Testing Atan ...")
	atan_a = symbolics.atan(a)
	print(atan_a)	
	
	print("Testing Atan2 ...")
	atan2_a_b = symbolics.atan2(a,b)
	print(atan2_a_b)	

	print("Testing Cos ...")
	cos_a = symbolics.cos(a)
	print(cos_a)	

	print("Testing Der ...")
	der_a = symbolics.der(a)
	print(der_a)	

	print("Testing Element ...")
	element_A00 = symbolics.element(A,0,0)
	print(element_A00)	

	print("Testing scalar ...")
	scalar_a = symbolics.scalar(a)
	print(scalar_a)	
	
	print("Testing sin ...")
	sin = symbolics.sin(a)
	print(sin)	

	print("Testing skew ...")
	v = symbolics.CMatrix( [1,2,3] )
	skew_v = symbolics.skew(v)
	print(skew_v)		

	print("Testing solve ...")
	solve_a_b = symbolics.solve(a,b)
	print(solve_a_b)	

	print("Testing tan ...")
	tan_a = symbolics.tan(a)
	print(tan_a)	

	print("Testing transpose ...")
	m = symbolics.CMatrix( [[1,2],[3,4]] )
	transpose_m = symbolics.transpose(m)
	print(transpose_m)	
	
	print("Fertig")

if __name__ == '__main__':
    main()
