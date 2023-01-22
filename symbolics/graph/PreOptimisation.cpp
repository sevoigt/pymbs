#include "PreOptimisation.h"
#include "str.h"
#include <iostream>
#include <fstream>

using namespace Symbolics;
using namespace Graph;

/*****************************************************************************/
PreOptimisation::PreOptimisation(EquationSystemPtr eqsys): m_eqsys(eqsys)
/*****************************************************************************/
{
}
/*****************************************************************************/

/*****************************************************************************/
PreOptimisation::~PreOptimisation()
/*****************************************************************************/
{ 
}
/*****************************************************************************/

/*****************************************************************************/
void PreOptimisation::optimize()
/*****************************************************************************/
{
  // Acos
  optimizeUnaryFunctions<Acos>(Type_Acos);
  // Asin
  optimizeUnaryFunctions<Asin>(Type_Asin);
  // Atan
  optimizeUnaryFunctions<Atan>(Type_Atan);
  // Atan2
  optimizeUnaryFunctions<Atan2>(Type_Atan2);
  // Cos
  optimizeUnaryFunctions<Cos>(Type_Cos);
  // Sin
  optimizeUnaryFunctions<Sin>(Type_Sin);
  // Tan
  optimizeUnaryFunctions<Tan>(Type_Tan);
}
/*****************************************************************************/

std::string sin_str="sin";
std::string cos_str="cos";
std::string atan_str="atan";
std::string atan2_str="atan2";
std::string acos_str="acos";
std::string asin_str="asin";
std::string tan_str="tan";
std::string tmp_str="$TMP";

/*****************************************************************************/
std::string& PreOptimisation::Basic_TypetoStr(Symbolics::Basic_Type const& t)
/*****************************************************************************/
{
  switch (t)
  {
  case Type_Sin: return sin_str;
  case Type_Cos: return cos_str;
  case Type_Atan: return atan_str;
  case Type_Atan2: return atan2_str;
  case Type_Acos: return acos_str;
  case Type_Asin: return asin_str;
  case Type_Tan: return tan_str;
  }
  return tmp_str;
}
/*****************************************************************************/

