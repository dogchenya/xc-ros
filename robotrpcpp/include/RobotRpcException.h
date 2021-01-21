
#ifndef _ROBOTRPCEXCEPTION_H_
#define _ROBOTRPCEXCEPTION_H_

#if defined(_MSC_VER)
# pragma warning(disable:4786)    // identifier was truncated in debug info
#endif

#ifndef MAKEDEPEND
# include <string>
#endif

#include "RobotRpcDecl.h"


namespace RobotRpc {

  //! A class representing an error.
  //! If server methods throw this exception, a fault response is returned
  //! to the client.
  class ROBOTRPCPP_DECL RobotRpcException {
  public:
    //! Constructor
    //!   @param message  A descriptive error message
    //!   @param code     An integer error code
    RobotRpcException(const std::string& message, int code=-1) :
        _message(message), _code(code) {}

    //! Return the error message.
    const std::string& getMessage() const { return _message; }

    //! Return the error code.
    int getCode() const { return _code; }

  private:
    std::string _message;
    int _code;
  };

}

#endif	// _ROBOTRPCEXCEPTION_H_
