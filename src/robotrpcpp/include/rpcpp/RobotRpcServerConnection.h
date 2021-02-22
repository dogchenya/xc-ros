#ifndef _ROBOTRPCSERVERCONNECTION_H_
#define _ROBOTRPCSERVERCONNECTION_H_
//
// XmlRpc++ Copyright (c) 2002-2003 by Chris Morley
//
#if defined(_MSC_VER)
# pragma warning(disable:4786)    // identifier was truncated in debug info
#endif

#ifndef MAKEDEPEND
# include <string>
#endif

#include "rpcpp/XmlRpcValue.h"
#include "rpcpp/RobotRpcSocketSource.h"
#include "rpcpp/RobotRpcDecl.h"

namespace RobotRpc {


  // The server waits for client connections and provides methods
  class RobotRpcServer;
  class RobotRpcServerMethod;

  //! A class to handle Robot RPC requests from a particular client
  class ROBOTRPCPP_DECL RobotRpcServerConnection : public RobotRpcSocketSource {
  public:
    // Static data
    static const char METHODNAME_TAG[];
    static const char PARAMS_TAG[];
    static const char PARAMS_ETAG[];
    static const char PARAM_TAG[];
    static const char PARAM_ETAG[];

    static const std::string SYSTEM_MULTICALL;
    static const std::string METHODNAME;
    static const std::string PARAMS;

    static const std::string FAULTCODE;
    static const std::string FAULTSTRING;

    //! Constructor
    RobotRpcServerConnection(int fd, RobotRpcServer* server, bool deleteOnClose = false);
    //! Destructor
    virtual ~RobotRpcServerConnection();

    // XmlRpcSource interface implementation
    //! Handle IO on the client connection socket.
    //!   @param eventType Type of IO event that occurred. @see XmlRpcDispatch::EventType.
    virtual unsigned handleEvent(unsigned eventType);

  protected:

    bool readHeader();
    bool readRequest();
    bool writeResponse();

    // Parses the request, runs the method, generates the response xml.
    virtual void executeRequest();

    // Parse the methodName and parameters from the request.
    std::string parseRequest(XmlRpcValue& params);

    // Execute a named method with the specified params.
    bool executeMethod(const std::string& methodName, XmlRpcValue& params, XmlRpcValue& result);

    // Execute multiple calls and return the results in an array.
    bool executeMulticall(const std::string& methodName, XmlRpcValue& params, XmlRpcValue& result);

    // Construct a response from the result XML.
    void generateResponse(std::string const& resultXml);
    void generateFaultResponse(std::string const& msg, int errorCode = -1);
    std::string generateHeader(std::string const& body);


    // The RobotRpc server that accepted this connection
    RobotRpcServer* _server;

    // Possible IO states for the connection
    enum ServerConnectionState { READ_HEADER, READ_REQUEST, WRITE_RESPONSE };
    ServerConnectionState _connectionState;

    // Request headers
    std::string _header;

    // Number of bytes expected in the request body (parsed from header)
    int _contentLength;

    // Request body
    std::string _request;

    // Response
    std::string _response;

    // Number of bytes of the response written so far
    int _bytesWritten;

    // Whether to keep the current client connection open for further requests
    bool _keepAlive;
  };
} // namespace RobotRpc

#endif // _ROBOTRPCSERVERCONNECTION_H_
