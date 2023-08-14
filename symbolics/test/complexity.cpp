#include <iostream>
#include "Symbolics.h"
#include "str.h"
#include <map>
#include <set>
#include <cstdlib>
#include "complexity.h"
#include <iostream>
#include <fstream>
#include "str.h"

#ifdef WIN32
#define _WIN32_WINNT 0x0501        // = Windows XP, sonst gibt es kein "GetSystemTimes"

#pragma warning(disable: 4251)
#define _CRT_SECURE_NO_DEPRECATE
#include <direct.h>
#include <io.h>
#include <sys/timeb.h>
#include <shlwapi.h>
#include <windows.h>
#else
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <dirent.h>
#include <dlfcn.h>
#endif

#ifdef LINUX
  #include <sys/sysinfo.h>
#endif

#include <stdio.h>

using namespace Symbolics;

static size_t N=14;

/*****************************************************************************/
double getTime()
  /*****************************************************************************/
{
#ifdef WIN32
  LARGE_INTEGER freq, time;
  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&time);
  return (double)(time.QuadPart)/(double)freq.QuadPart;
#else
  struct timespec tp;
  clock_gettime(CLOCK_REALTIME,&tp);
  return (double)(tp.tv_sec)+1e-9*(double)(tp.tv_nsec);  
#endif
}
/*****************************************************************************/


int main( int argc,  char *argv[])
{

  char szInput [256];
  std::cout << "Enter test number: " << std::endl;
  fgets ( szInput, 256, stdin );
  unsigned int test = atoi(szInput);
  if (test < 1)
  {
    std::cout << "Enter Number larger than 0!" << std::endl;
    return -1;
  }
#ifdef WIN32
  // Prozess Prioritaet setzen
  if (!SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS))
    std::cout << "Prozesspriorit�t konnte nicht gesetzt werden!" << std::endl;
#endif

  // Beispiel aufbauen
  double t0;
  double t1;
  size_t add=0;
  std::vector<std::vector<double> > times;

  std::vector<unsigned int> ens;
  unsigned int base=1024; 
  for (size_t i=0;i<N;i++)
  {
    ens.push_back(base);
    base = base*2;
  }

  if (test > 0)
  {
    for (size_t l=0;l<ens.size();l++)
    {
      std::vector<double> res;
      res.push_back(ens[l]);

  /*******************************************************************/
  /** Vector, Set, Map: fill, itterate                               */
  /*******************************************************************/

      std::cout << "Complexity with Elements: " << ens[l] << std::endl;
      /*
      {
        SymbolPtr *symbolarray = new SymbolPtr[i];
        memset((void*)symbolarray,0,sizeof(BasicPtr)*i);

        t0 = getTime();
        for (size_t i=0;i<ens[l];++i)
        {
          SymbolPtr s(new Symbol("a"+str(i)));
          symbolarray[i] = s;
        }
        t1 = getTime();
        std::cout << "Time to fill Array: " << t1-t0 << std::endl;
        res.push_back(t1-t0);


        t0 = getTime();
        add=0;
        for (size_t i=0;i<ens[l];++i)
        {
          add += symbolarray[i]->getType();
        }
        t1 = getTime();
        std::cout << "Time to iterate Array: " << t1-t0 << std::endl;
        res.push_back(t1-t0);


        delete[] symbolarray;
      }
      */
      if (test & 0x1)
      {
        complexity::vector<SymbolPtr> jensvector;

        t0 = getTime();
        for (size_t i=0;i<ens[l];++i)
        {
          SymbolPtr s(new Symbol("a"+str(i)));
          jensvector(s);
        }
        t1 = getTime();
        std::cout << "Time to fill JensVector: " << t1-t0 << std::endl;
        res.push_back(t1-t0);

        t0 = getTime();
        add=0;
        for (size_t i=0;i<ens[l];++i)
        {
          add += jensvector[i]->getType();
        }
        t1 = getTime();
        std::cout << "Time to iterate JensVector: " << t1-t0 << std::endl;
        res.push_back(t1-t0);

      }
      if (test & 0x2)
      {
        std::vector<SymbolPtr> simbolvector;

        t0 = getTime();
        for (size_t i=0;i<ens[l];++i)
        {
          SymbolPtr s(new Symbol("a"+str(i)));
          simbolvector.push_back(s);
        }
        t1 = getTime();
        std::cout << "Time to fill Vector: " << t1-t0 << std::endl;
        simbolvector.clear();
        res.push_back(t1-t0);


        t0 = getTime();
        simbolvector.reserve(ens[l]);
        for (size_t i=0;i<ens[l];++i)
        {
          SymbolPtr s(new Symbol("a"+str(i)));
          simbolvector.push_back(s);
        }
        t1 = getTime();
        std::cout << "Time to fill Vector using reserve: " << t1-t0 << std::endl;
        res.push_back(t1-t0);



        t0 = getTime();
        add=0;
        for (size_t i=0;i<simbolvector.size();++i)
        {
          add += simbolvector[i]->getType();
        }
        t1 = getTime();
        std::cout << "Time to iterate Vector using index: " << t1-t0 << std::endl;
        res.push_back(t1-t0);



        t0 = getTime();
        add=0;
        for (std::vector<SymbolPtr>::iterator ii=simbolvector.begin();ii!=simbolvector.end();++ii)
        {
          add += (*ii)->getType();
        }
        t1 = getTime();
        std::cout << "Time to itterate Vector using iterator: " << t1-t0 << std::endl;
        res.push_back(t1-t0);



        t0 = getTime();
        add=0;
        std::vector<SymbolPtr>::iterator ie=simbolvector.end();
        for (std::vector<SymbolPtr>::iterator ii=simbolvector.begin();ii!=ie;++ii)
        {
          add += (*ii)->getType();
        }
        t1 = getTime();
        std::cout << "Time to itterate Vector using iterator predefined end: " << t1-t0 << std::endl;
        res.push_back(t1-t0);


      }
      if (test & 0x4)
      {
        std::set<SymbolPtr> simbolset;

        t0 = getTime();
        for (size_t i=0;i<ens[l];++i)
        {
          SymbolPtr s(new Symbol("a"+str(i)));
          simbolset.insert(s);
        }
        t1 = getTime();
        std::cout << "Time to fill Set: " << t1-t0 << std::endl;
        res.push_back(t1-t0);


        t0 = getTime();
        add=0;
        for (std::set<SymbolPtr>::iterator ii=simbolset.begin();ii!=simbolset.end();++ii)
        {
          add += (*ii)->getType();
        }
        t1 = getTime();
        std::cout << "Time to itterate Set using iterator: " << t1-t0 << std::endl;
        res.push_back(t1-t0);



        t0 = getTime();
        add=0;
        std::set<SymbolPtr>::iterator ies=simbolset.end();
        for (std::set<SymbolPtr>::iterator ii=simbolset.begin();ii!=ies;++ii)
        {
          add += (*ii)->getType();
        }
        t1 = getTime();
        std::cout << "Time to itterate Set using iterator predefined end: " << t1-t0 << std::endl;
        res.push_back(t1-t0);

      }
      if (test & 0x8)
      {
        std::map<SymbolPtr,BasicPtr> simbolmap;

        t0 = getTime();
        for (size_t i=0;i<ens[l];++i)
        {
          SymbolPtr s(new Symbol("a"+str(i)));
          BasicPtr se(s);
          BasicPtr exp(new Sin(se));
          simbolmap[s] = exp;
        }
        t1 = getTime();
        std::cout << "Time to fill Map: " << t1-t0 << std::endl;
        res.push_back(t1-t0);


        t0 = getTime();
        add=0;
        for (std::map<SymbolPtr,BasicPtr>::iterator ii=simbolmap.begin();ii!=simbolmap.end();++ii)
        {
          add += (*ii).first->getType();
        }
        t1 = getTime();
        std::cout << "Time to itterate Map using iterator: " << t1-t0 << std::endl;
        res.push_back(t1-t0);



        t0 = getTime();
        add=0;
        std::map<SymbolPtr,BasicPtr>::iterator iem=simbolmap.end();
        for (std::map<SymbolPtr,BasicPtr>::iterator ii=simbolmap.begin();ii!=iem;++ii)
        {
          add += (*ii).first->getType();
        }
        t1 = getTime();
        std::cout << "Time to itterate Map using iterator predefined end: " << t1-t0 << std::endl;
        res.push_back(t1-t0);

      }

      if (test & 0x10)
      {
        complexity::MHashTable<SymbolPtr,BasicPtr> m_hashtable;

        t0 = getTime();
        for (size_t i=0;i<ens[l];++i)
        {
          SymbolPtr s(new Symbol("a"+str(i)));
          BasicPtr se(s);
          BasicPtr exp(new Sin(se));
          m_hashtable.insert(s,exp,s->getHash());
        }
        t1 = getTime();
        std::cout << "Time to fill HashTable: " << t1-t0 << std::endl;
        res.push_back(t1-t0);
      }

      times.push_back(res);
    }
  }

  /*******************************************************************/
  /** Map vs. HashMap                                                */
  /*******************************************************************/

  std::vector<double> times0;
  std::vector<double> times1;
  std::vector<double> times2;
  if (test & 0x20)
  {
    std::map<SymbolPtr,BasicPtr> simbolmap;
    std::vector<SymbolPtr> simbolvector;
    complexity::MHashTable<SymbolPtr,BasicPtr> m_hashtable;
    for (size_t l=0;l<ens.size();l++)
    {
      std::cout << "Complexity with Elements: " << ens[l] << std::endl;


      {

        size_t lowerbound = 0;
        if (l>0)
          lowerbound = ens[l-1];
        for (size_t i=lowerbound;i<ens[l];++i)
        {
          SymbolPtr s(new Symbol("a"+str(i)));
          simbolvector.push_back(s);
          BasicPtr se(s);
          BasicPtr exp(new Sin(se));
          simbolmap[s] = exp;
          m_hashtable.insert(s,exp,s->getHash());
        }

        t0 = getTime();

        for (size_t k=0;k<simbolvector.size();++k)
        {
          ++add;
        }
        t1 = getTime();
        times0.push_back(t1-t0);


        t0 = getTime();
        for (size_t k=0;k<simbolvector.size();++k)
        {
          BasicPtr expfound = simbolmap[simbolvector[k]];
          if (expfound.get() == NULL)
            std::cout << "Mapelement not Found: " << ens[l] << "  " << k << std::endl;
        }
        t1 = getTime();
        times1.push_back(t1-t0);

        t0 = getTime();
        for (size_t k=0;k<simbolvector.size();++k)
        {
          BasicPtr expfound = m_hashtable.find(simbolvector[k]->getHash(),simbolvector[k]);
          if (expfound.get() == NULL)
            std::cout << "HashElement not Found: " << ens[l] << "  " << k << std::endl;
        }
        t1 = getTime();
        times2.push_back(t1-t0);
      }
    }
  }


  /*******************************************************************/
  /** Matrix vs. SymmetricMatrix                                     */
  /*******************************************************************/
  if (test & 0x40)
  {
      

  }
  /*******************************************************************/
  /** Ausabe                                                         */
  /*******************************************************************/
if (test > 0)
  {
    std::string filename = "complexety.csv";
    std::cout << "Write Results to file " << filename << std::endl;

    std::ofstream f;
    f.open(filename.c_str());

    if (test < 0x20)
    {
      f << "Complexity with Elements;"; 
        //         "Time to fill Array;";
        //         "Time to iterate Array;";
      if (test & 0x1)
      {
        f <<  "Time to fill JensVector;";
        f <<  "Time to iterate JensVector;";
      }
      if (test & 0x2)
      {
        f <<  "Time to fill Vector;";
        f <<  "Time to fill Vector using reserve;";
        f <<  "Time to iterate Vector using index;";
        f <<  "Time to itterate Vector using iterator;";
        f <<  "Time to itterate Vector using iterator predefined end;";
      }
      if (test & 0x4)
      {

        f <<  "Time to fill Set;";
        f <<  "Time to itterate Set using iterator;";
        f <<  "Time to itterate Set using iterator predefined end;";
      }
      if (test & 0x8)
      {
        f <<  "Time to fill Map;";
        f <<  "Time to itterate Map using iterator;";
        f <<  "Time to itterate Map using iterator predefined end;";
      }
      if (test & 0x10)
      {
        f <<  "Time to fill HashTable;";
      }
      f <<  std::endl;

      for (size_t i=0;i<times.size();i++)
      {
        for (size_t j=0;j<times[i].size();j++)
        {
          double value = times[i][j];
          f << value << ";";
        }
        f << std::endl;
      }

    }
    if (test & 0x20)
    {

      f << ";;;" << std::endl;

      f << "Get;" << "For;" << "Map;" << "HashTable;" << std::endl;
      for (size_t i=0;i<times1.size();i++)
      {
        double value0 = times0[i];
        double value1 = times1[i];
        double value2 = times2[i];
        f << i << ";" << value0 << ";" << value1 << ";" << value2 << ";" << std::endl;

      }
      f << std::endl;
    }

    f.close();
  }

  return 0;
}
