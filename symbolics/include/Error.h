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

#ifndef __ERROR_H_
#define __ERROR_H_

#include <exception>
#include <string>

namespace Symbolics
{

#ifdef WIN32
    class Exception: public std::exception
    {
    public:
        Exception( const char *  &msg ): std::exception(msg) {};
        Exception( const std::string &msg ): std::exception(msg.c_str()) {};
    };
#else
    class Exception: public std::exception
    {
    public:
        Exception(  const char *  &msg ): m_msg(msg) {};
        Exception(  const std::string &msg ): m_msg(msg) {};

        ~Exception() throw() {};

        const char* what()  throw() 
        {
            return m_msg.c_str();        
        }
    private:
        std::string m_msg;
    };

#endif

/*****************************************************************************/
        // InternalError
        class InternalError: public Exception
        {
        public:
            InternalError(  const char *  &msg ): Exception(msg) {};
            InternalError(  const std::string &msg ): Exception(msg.c_str()) {};
        };
/*****************************************************************************/


/*****************************************************************************/
    class ShapeError: public Exception
    {
    public:
        ShapeError( const char*  &msg): Exception(msg) {};
        ShapeError( const std::string &msg): Exception(msg.c_str()) {};
    };
/*****************************************************************************/


/*****************************************************************************/
        // IndexException
        class IndexError: public Exception
        {
        public:
            IndexError(  const char *  &msg ): Exception(msg) {};
            IndexError(  const std::string &msg ): Exception(msg.c_str()) {};
        };
/*****************************************************************************/
};

#endif // __ERROR_H_
