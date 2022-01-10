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

#ifndef __PRINTER_H_
#define __PRINTER_H_

#include <string>
#include <vector>
#include "Symbolics.h"
#include "Graph.h"

namespace Symbolics
{
    class Printer
    {
    public:
        // Konstruktor
        Printer();
        // Destruktor
        virtual ~Printer();

        // Methode die alle Arten von Ausdr�cken in Strings in der korrekten Syntax der Ausgabesprache verwandelt
        std::string print( BasicPtr const& basic );

        // Gibt den �bergebenen String mit Kommentarzeichen oder einen leeren String zur�ck
        // muss in abgeleiteten Klassen �berschrieben werden
        virtual std::string comment( std::string const& comment ) const = 0;
        
        // Gibt den Kommentar (mit Kommentarzeichen) zum �bergebenen Symbol Pointer oder einen leeren String zur�ck
        // ist ein Wrapper f�r obige Methode, wegen "name hiding" muss sie einen anderen Namen haben.
        virtual std::string comment2( Graph::Graph& g, SymbolPtr const& s ) const;

        // Gibt die Dimension des �bergebenen Ausdrucks als String zur�ck. Wird beim deklarieren von Variablen gebraucht
        // Gibt standardm��ig nur eine Fehlermeldung aus, muss somit f�r Sprachen in denen Variablen deklariert werden m�ssen �berschrieben werden
        virtual std::string dimension( BasicPtr const& basic );

        // Abfragen und Zur�cksetzen der Anzahl aufgetretener Fehlermeldungen
        int getErrorcount( bool reset=false ); 

    protected:

        // Essentielle Funktionen f�r die es keinen sinnvollen Standard gibt, 
        // da sie in jeder Programmiersprache eine andere Syntax haben
        // Beispielimplementierungen sind auskommentiert in Printer.cpp
        virtual std::string print_Element( const Element *e ) = 0;
        virtual std::string print_Matrix( const Matrix *c ) = 0;
        virtual std::string print_Pow( const Pow *pow ) = 0;
        virtual std::string print_Skew( const Skew *s ) = 0;
        virtual std::string print_Zero( const Zero *z ) = 0;
        virtual std::string print_Bool( const Bool *b ) = 0;


        // Funktionen die �berschreiben werden k�nnen wenn sie von der jeweiligen Sprache unterst�tzt werden sollen.
        // Standardm��ig geben diese Funktionen nur eine Fehlermeldung aus
        virtual std::string print_If( const If *e );
        virtual std::string print_Inverse( const Inverse *c );
        virtual std::string print_Jacobian( const Jacobian *j );
        virtual std::string print_Scalar( const Scalar *s ); //k�nnte evtl. auch nach unten, aber wo und wof�r wird es �berhaupt gebraucht?
        virtual std::string print_Solve( const Solve *s );


        // Funktionen die hier mit einem sinnvollen Standard implementiert sind, 
        // der für viele Sprachen passen sollte:

        //allgemeine Funktionen
        virtual std::string print_Abs( const Abs *s );
        virtual std::string print_Add( const Add *add );
        virtual std::string print_Der( const Der *d );
        virtual std::string print_Int( const Int *c );
        virtual std::string print_Mul( const Mul *mul );
        virtual std::string print_Neg( const Neg *neg );
        virtual std::string print_Outer( const Outer *o );
        virtual std::string print_Real( const Real *c );
        virtual std::string print_Sign( const Sign *s );
        virtual std::string print_Symbol( const Symbol *symbol );
        virtual std::string print_Transpose( const Transpose *s );
        virtual std::string print_Unknown( const Unknown *u );

        //trigonometrische Funktionen
        virtual std::string print_Acos( const Acos *s );
        virtual std::string print_Asin( const Asin *s );
        virtual std::string print_Atan( const Atan *s );
        virtual std::string print_Atan2( const Atan2 *s );
        virtual std::string print_Cos( const Cos *c );
        virtual std::string print_Sin( const Sin *s );
        virtual std::string print_Tan( const Tan *s );

        //Vergleichsoperationen
        virtual std::string print_Equal( const Equal *e );
        virtual std::string print_Greater( const Greater *e );
        virtual std::string print_Less( const Less *e );


        // Helferlein
        // std::vector von Basics mit Trennzeichen zu einem String zusammenfuegen
        std::string join( ConstBasicPtr const& arg,  std::string const& sep);
        std::string join( ConstBasicPtr const& arg,  std::string const& posSep,  std::string const& negSep);
        
        // Eine Matrix (Constant_Dbl, Constant_Int, Matrix) mit definierten Trennzeichen ausgeben:
		// Matrix = <leftConstr> <leftRowBracket> ... <rightRowBracket> <rowSep> <leftRowBracket> ... <rightRowBracket> <rightConstr>
		// Vektoren werden bei unterschiedlichem rowSep und colSep wie folgt ausgegeben:
		//     Spaltenvektor = <leftConstr> ... <rowSep> ... <rightConstr>
		//     Zeilenvektor  = <leftConstr> ... <colSep> ... <rightConstr>
		// Bei identischem rowSep und colSep werden Vektoren wie Matrizen ausgeben, au�er es ist forceNoMatrixVectors gesetzt,
		// womit allerdings die Information ob es sich um einen Zeilen oder Spaltenvektor handelt verloren geht.
        // 
        std::string print_Matrix( const Matrix *mat, 
                                  char colSep,  char rowSep, 
                                  char leftConstr,  char rightConstr,
                                  char leftRowBracket,  char rightRowBracket,
								  std::string matrixLineBreak = "\n        ",
								  bool forceNoMatrixVectors = false );
        
        // Gibt Fehlermeldungen auf der Standardfehlerausgabe aus, R�ckgabewert ist erweiterte errorMessage
        std::string error(std::string errorMessage);

        int m_errorcount; // Z�hler f�r Fehlermeldungen
    };
};

#endif // __PRINTER_H_
