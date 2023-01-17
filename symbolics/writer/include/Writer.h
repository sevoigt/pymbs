#ifndef __WRITER_H_
#define __WRITER_H_

#include <string>
#include <vector>
#include "Symbolics.h"
#include "Graph.h"

namespace Symbolics
{
    class Writer
    {
    public:
/*****************************************************************************/
        typedef enum _CATEGORY_
        {
            SENSOR = 0x100,                  // Expressions Defined by the User
            SENSOR_VISUAL = 0x200,           // Expressions Defined by the User
        } Category;
/*****************************************************************************/

    protected:
        // privater Konstruktor, keine Objekte erstellen
        Writer(bool makeScalar);
        // Destruktor
        virtual ~Writer();

        // file
        std::string m_path;
        std::string m_name;

        virtual double generateTarget_Impl(Graph::Graph& g) = 0;

        std::string getHeaderLine();

		// Vielleicht ist folgende Funktion logisch sinnvoller in einer anderen Klasse aufgehoben - wenn jemand wei� wo, einfach �ndern.
		// Die Funktion ist als Parameter f�r die std::sort Funktion gedacht, um Graph::VariableVec Vektoren zu sortieren.
		// Die Funktion muss static sein, da sie als functionpointer verwendet wird
		static bool sortVariableVec (boost::intrusive_ptr<Symbolics::Symbol> a, boost::intrusive_ptr<Symbolics::Symbol> b);

    public:
        double generateTarget(std::string name, std::string path, Graph::Graph& g, bool optimize);

    private:
        bool m_scalar;
        

    };
};

#endif // __WRITER_H_
