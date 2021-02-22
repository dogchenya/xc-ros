#ifndef _ROBOTRPCUTIL_H_
#define _ROBOTRPCUTIL_H_
//
// XmlRpc++ Copyright (c) 2002-2003 by Chris Morley
//
#if defined(_MSC_VER)
# pragma warning(disable:4786)    // identifier was truncated in debug info
#endif

#ifndef MAKEDEPEND
# include <string>
#endif

#include "rpcpp/RobotRpcDecl.h"

#if defined(_MSC_VER)
# define snprintf	    _snprintf_s
# define vsnprintf    _vsnprintf_s
# define strcasecmp	  _stricmp
# define strncasecmp	_strnicmp
#elif defined(__BORLANDC__)
# define strcasecmp stricmp
# define strncasecmp strnicmp
#endif

namespace RobotRpc {

  //! An interface allowing custom handling of error message reporting.
  class RobotRpcErrorHandler {
  public:
    virtual ~RobotRpcErrorHandler() { }

    //! Returns a pointer to the currently installed error handling object.
    static RobotRpcErrorHandler* getErrorHandler() 
    { return _errorHandler; }

    //! Specifies the error handler.
    static void setErrorHandler(RobotRpcErrorHandler* eh)
    { _errorHandler = eh; }

    //! Report an error. Custom error handlers should define this method.
    virtual void error(const char* msg) = 0;

  protected:
    static RobotRpcErrorHandler* _errorHandler;
  };

  //! An interface allowing custom handling of informational message reporting.
  class RobotRpcLogHandler {
  public:
    virtual ~RobotRpcLogHandler() { }

    //! Returns a pointer to the currently installed message reporting object.
    static RobotRpcLogHandler* getLogHandler() 
    { return _logHandler; }

    //! Specifies the message handler.
    static void setLogHandler(RobotRpcLogHandler* lh)
    { _logHandler = lh; }

    //! Returns the level of verbosity of informational messages. 0 is no output, 5 is very verbose.
    static int getVerbosity() 
    { return _verbosity; }

    //! Specify the level of verbosity of informational messages. 0 is no output, 5 is very verbose.
    static void setVerbosity(int v) 
    { _verbosity = v; }

    //! Output a message. Custom error handlers should define this method.
    virtual void log(int level, const char* msg) = 0;

  protected:
    static RobotRpcLogHandler* _logHandler;
    static int _verbosity;
  };

  //! Returns log message verbosity. This is short for RobotRpcLogHandler::getVerbosity()
  int getVerbosity();
  //! Sets log message verbosity. This is short for RobotRpcLogHandler::setVerbosity(level)
  void setVerbosity(int level);

  //! Version identifier
  extern const char ROBOTRPC_VERSION[];

  //! Utilities for XML parsing, encoding, and decoding and message handlers.
  class ROBOTRPCPP_DECL RobotRpcUtil {
  public:
    // hokey xml parsing
    //! Returns contents between <tag> and </tag>, updates offset to char after </tag>
    static std::string parseTag(const char* tag, std::string const& xml, int* offset);

    //! Returns true if the tag is found and updates offset to the char after the tag
    static bool findTag(const char* tag, std::string const& xml, int* offset);

    //! Returns the next tag and updates offset to the char after the tag, or empty string
    //! if the next non-whitespace character is not '<'
    static std::string getNextTag(std::string const& xml, int* offset);

    //! Returns true if the tag is found at the specified offset (modulo any whitespace)
    //! and updates offset to the char after the tag
    static bool nextTagIs(const char* tag, std::string const& xml, int* offset);


    //! Convert raw text to encoded xml.
    static std::string xmlEncode(const std::string& raw);

    //! Convert encoded xml to raw text
    static std::string xmlDecode(const std::string& encoded);


    //! Dump messages somewhere
    static void log(int level, const char* fmt, ...);

    //! Dump error messages somewhere
    static void error(const char* fmt, ...);

  };
} // namespace XmlRpc

#endif // _XMLRPCUTIL_H_
