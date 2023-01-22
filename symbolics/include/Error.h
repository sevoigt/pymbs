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
