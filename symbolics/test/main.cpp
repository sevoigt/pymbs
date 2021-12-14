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
#include <map>
#include <set>

#ifndef __GNUC__
#include <unordered_map>
#endif

#include "ModelicaPrinter.h"


using namespace Symbolics;


// The following class defines a hash function for strings 
#ifndef __GNUC__
class ConstBasichasher : public stdext::hash_compare <BasicPtr>
{
public:
  /**
   * Required by 
   * Inspired by the java.lang.String.hashCode() algorithm 
   * (it's easy to understand, and somewhat processor cache-friendly)
   * @param The string to be hashed
   * @return The hash value of s
   */
  size_t operator() ( BasicPtr const& s) const
  {
      return s->getHash();
  }

  /**
   * 
   * @param s1 The first string
   * @param s2 The second string
   * @return true if the first string comes before the second in lexicographical order
   */
  bool operator() ( BasicPtr const& s1,  BasicPtr const& s2) const
  {
      return s1->getHash() < s2->getHash();
  }
};
#endif

int main( int argc,  char *argv[])
{

	//// Parents
	//{
	//	Symbol* a = new Symbol("a");
	//	Symbol* b = new Symbol("a");
	//	Symbol* c = new Symbol("c");

	//	BasicPtr pa1(a);
	//	BasicPtr pa2(a);
	//	BasicPtr pb1(b);
	//	BasicPtr pb2(b);
	//	BasicPtr pc1(c);

	//	Basic::BasicPtrSet set;
	//	set.insert( &pa1 );
	//	if (set.size() != 1) return -1;
	//	set.insert( &pb1 );
	//	if (set.size() != 2) return -2;
	//	set.insert( &pc1 );
	//	if (set.size() != 3) return -3;
	//	set.erase( &pa2 );
	//	if (set.size() != 3) return -4;
	//}


    ModelicaPrinter p; //ModelicaPrinter gewählt da es meistens der vollständigste ist
    // Beispiel aufbauen
    BasicPtr a(new Symbol("a"));
    BasicPtr b(new Symbol("b"));
    BasicPtr c(new Symbol("c"));

    {
        // Expression definieren
        BasicPtr exp = a*(b+1-c);
    }

    // Expression definieren
    BasicPtr exp = a*(b+1-c);

    // Klappt es so?
    std::cout << p.print(exp) << std::endl;

    // atoms
    std::set< BasicPtr> atoms = exp->getAtoms();

    if (atoms.size() != 3) return -1;
    if (atoms.find(a) == atoms.end()) return -2;
    if (atoms.find(b) == atoms.end()) return -3;
    if (atoms.find(c) == atoms.end()) return -4;

    // Expression definieren
    BasicPtr exp1 = exp->subs(a,c);

    ArgumentPtrVec myvec;
    myvec.push_back(Argument(a));
    myvec.push_back(Argument(b));
    myvec.push_back(Argument(c));
    a->subs(b);
    if (myvec[0].getArg() != b) return -1000;

    // atoms
    std::set< BasicPtr> atoms1 = exp1->getAtoms();
    if (atoms1.size() != 2) return -5;
    if (atoms1.find(a) != atoms1.end()) return -6;
    if (atoms1.find(b) == atoms1.end()) return -7;
    if (atoms1.find(c) == atoms1.end()) return -8;


    // std::map
    std::map<BasicPtr,BasicPtr> mymap;
    BasicPtr exp2(new Sin(a));
    BasicPtr exp4(new Cos(a));
    BasicPtr exp3(new Sin(a));
    BasicPtr exp5(new Cos(a));
    mymap[exp2] = a;
    mymap[exp4] = a;
    // Vergleiche testen
    if (exp2 != exp3) return -10;
    if (exp2 < exp3) return -11;
    if (exp3 < exp2) return -12;
    // less testen
    std::less<BasicPtr> comp;
    if (comp(exp2,exp3)) return -13;
    if (comp(exp3,exp2)) return -14;

    std::map<BasicPtr,BasicPtr>::iterator ii = mymap.find(exp3);
    if (ii == mymap.end()) return -15;
    ii = mymap.find(exp5);
    if (ii == mymap.end()) return -16;


    // std::set
    std::set<BasicPtr> myset;
    myset.insert(exp2);
    std::set<BasicPtr>::iterator jj = myset.find(exp3);
    if (jj == myset.end()) return -20;


    // hash_map
#ifndef __GNUC__	
    std::unordered_map<BasicPtr,BasicPtr,ConstBasichasher> m_hashmap;
    m_hashmap[a] = exp2;
    m_hashmap[b] = exp3;
    m_hashmap[c] = exp4;

    std::unordered_map<BasicPtr,BasicPtr,ConstBasichasher>::iterator kk = m_hashmap.find(b);
    if (kk == m_hashmap.end()) return -21;
#endif



    // Matrix
    BasicPtrVec matvec;
    matvec.push_back(a);
    matvec.push_back(b);
    matvec.push_back(c);
    Matrix mat(matvec,Shape(3));

    BasicPtr mat2(new Matrix(mat));

    mat.set(0, c);

	/************************************************************************/
	/* Beeinflussung von Symbolen untereinander								*/
	/************************************************************************/
	BasicPtr nega(new Neg(a));
	BasicPtr nega2(new Neg(a));
	BasicPtr mulnab( new Mul(nega,b));
	BasicPtr mulnab2( new Mul(nega2,b));
	BasicPtr addnabc( new Add(mulnab,c) );

	BasicPtr sinmulnab( new Sin(mulnab) );
	BasicPtr sinmulnab2( new Sin(mulnab2) );

	// Vorher
	std::cout << addnabc->toString() << std::endl;
	std::cout << sinmulnab->toString() << std::endl;

	// Simplify ist schuld 
	addnabc->simplify();

	// Nachher
	std::cout << addnabc->toString() << std::endl;
	std::cout << sinmulnab->toString() << std::endl;

	// Vergleich
	if (sinmulnab != sinmulnab2) return -30;
	/************************************************************************/

    return 0;
}
