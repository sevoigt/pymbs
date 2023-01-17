#ifndef __FMU_WRITER_H_
#define __FMU_WRITER_H_

#include "CWriter.h"

namespace Symbolics
{
    class FMUWriter: public CWriter
    {
    public:
        // Konstruktor
        FMUWriter( std::map<std::string, std::string> &kwds );
        FMUWriter();
        // Destruktor
        ~FMUWriter();

    protected:
        double generateTarget_Impl(Graph::Graph& g);

		std::string m_base_path;
		std::string m_util_folder, m_7_Zip_app;
        bool m_compile;

    private:
		void init();
        std::vector<std::string> split_path(std::string path);

		double generateXML(Graph::Graph& g);
		double generateModel(Graph::Graph& g, int &dim);

       	int m_numberOfStates;
        std::string m_guid;
        std::map<std::string, int> m_valueReferences;

    };
};

#endif // __FMU_WRITER_H_
