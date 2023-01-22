#include <iostream>
#include "test_element.h"

using namespace Symbolics;

int test_element( int &argc,  char *argv[])
{
  // Symbole und Matrix anlegen
  BasicPtr a( new Symbol("a") );
  BasicPtr b( new Symbol("b") );
  BasicPtr c( new Symbol("c") );
  BasicPtr d( new Symbol("d") );
  BasicPtr A( new Symbol("A", Shape(3,2) ) );
  Matrix *matB = new Matrix( Shape(2,2) );
  *matB =    a, b, 
    c, d;
  BasicPtr B( matB );

  Matrix *matC = new Matrix( Shape(2,2) );
  *matC =    1, 2, 
    3, 4;
  BasicPtr C( matC );

  Matrix *matD = new Matrix( Shape(2,2) );
  *matD =    1.1, 2.2, 
    3.3, 4.4;
  BasicPtr D( matD );

  // Na dann mal los
  Element *elA = new Element(A, 2, 1);
  if (elA->getType() != Type_Element) return -1;
  if (elA->getShape() != Shape()) return -2;

  // Exceptions testen
  try
  {
    Element *el2 = new Element(A, 3, 1);
    return -3;
  }
  catch( IndexError ) {}
  catch ( ... ) { return -4; };

  try
  {
    Element *el2 = new Element(A, 2, 2);
    return -5;
  }
  catch( IndexError ) {}
  catch ( ... ) { return -6; };
  try
  {
    Element *el2 = new Element(A, 3, 2);
    return -6;
  }
  catch( IndexError ) {}
  catch ( ... ) { return -7; };

  // Vereinfachung testen
  for (size_t i=0; i<2; ++i)
    for (size_t j=0; j<2; ++j)
    {
      BasicPtr elB( new Element(B, i, j) );
      BasicPtr simpleElB( elB->simplify() );
      if (simpleElB != (*matB)(i,j) ) return -12;

      BasicPtr elC( new Element(C, i, j) );
      BasicPtr simpleElC( elC->simplify() );
      BasicPtr elementC = (*matC)(i,j);
      if (simpleElC != elementC) return -13;

      BasicPtr elD( new Element(D, i, j) );
      BasicPtr simpleElD( elD->simplify() );
      BasicPtr elementD = (*matD)(i,j);
      if (simpleElD != elementD) return -14;

      BasicPtr eltransD( new Element(Transpose::New(D), i, j) );
      BasicPtr simpleEltransD( eltransD->simplify() );
      BasicPtr elementD1 = (*matD)(j,i);
      if (simpleEltransD != elementD1) return -15;
    }

  return 0;
}