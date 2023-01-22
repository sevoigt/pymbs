#ifndef __MODELICA_WRITER_H_
#define __MODELICA_WRITER_H_

#include "Writer.h"

namespace Symbolics
{
    class ModelicaWriter: public Writer
    {
    public:
      // Konstruktor
		  ModelicaWriter();
		  ModelicaWriter( std::map<std::string, std::string> &kwds );
      // Destruktor
      ~ModelicaWriter();

    protected:
		  // Settings
		  bool m_InputsAsInputs;
      std::string package;

      double generateTarget_Impl(Graph::Graph& g);

    };
};

#endif // __MODELICA_WRITER_H_
