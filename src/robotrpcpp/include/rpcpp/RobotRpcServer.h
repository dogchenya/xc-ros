
#ifndef _ROBOTRPCSERVER_H_
#define _ROBOTRPCSERVER_H_

#if defined(_MSC_VER)
# pragma warning(disable:4786)    // identifier was truncated in debug info
#endif

#ifndef MAKEDEPEND
# include <map>
# include <string>
#endif

#include "rpcpp/RobotRpcDispatch.h"
#include "rpcpp/RobotRpcSocketSource.h"
#include "rpcpp/RobotRpcDecl.h"

namespace RobotRpc {


  // An abstract class supporting XML RPC methods
  class RobotRpcServerMethod;

  // Class representing connections to specific clients
  class RobotRpcServerConnection;

  // Class representing argument and result values
  class XmlRpcValue;


  //! A class to handle XML RPC requests
  class ROBOTRPCPP_DECL RobotRpcServer : public RobotRpcSocketSource {
  public:
    //! Create a server object.
    RobotRpcServer();
    //! Destructor.
    virtual ~RobotRpcServer();

    //! Specify whether introspection is enabled or not. Default is not enabled.
    void enableIntrospection(bool enabled=true);

    //! Add a command to the RPC server
    void addMethod(RobotRpcServerMethod* method);

    //! Remove a command from the RPC server
    void removeMethod(RobotRpcServerMethod* method);

    //! Remove a command from the RPC server by name
    void removeMethod(const std::string& methodName);

    //! Look up a method by name
    RobotRpcServerMethod* findMethod(const std::string& name) const;

    //! Create a socket, bind to the specified port, and
    //! set it in listen mode to make it available for clients.
    bool bindAndListen(int port, int backlog = 5);

    //! Process client requests for the specified time
    void work(double msTime);

    //! Temporarily stop processing client requests and exit the work() method.
    void exit();

    //! Close all connections with clients and the socket file descriptor
    void shutdown();

    //! Introspection support
    void listMethods(XmlRpcValue& result);

    // RobotRpcSocketSource interface implementation

    //! Handle client connection requests
    virtual unsigned handleEvent(unsigned eventType);

    //! Remove a connection from the dispatcher
    virtual void removeConnection(RobotRpcServerConnection*);

    inline int get_port() { return _port; }

    RobotRpcDispatch *get_dispatch() { return &_disp; }

  protected:

    //! Accept a client connection request
    virtual void acceptConnection();

    //! Create a new connection object for processing requests from a specific client.
    virtual RobotRpcServerConnection* createConnection(int socket);

    // Whether the introspection API is supported by this server
    bool _introspectionEnabled;

    // Event dispatcher
    RobotRpcDispatch _disp;

    // Collection of methods. This could be a set keyed on method name if we wanted...
    typedef std::map< std::string, RobotRpcServerMethod* > MethodMap;
    MethodMap _methods;

    // system methods
    RobotRpcServerMethod* _listMethods;
    RobotRpcServerMethod* _methodHelp;

    int _port;

  };
} // namespace RobotRpc

#endif //_ROBOTRPCSERVER_H_
