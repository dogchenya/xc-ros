// this file modified by Morgan Quigley on 22 Apr 2008.
// added features: server can be opened on port 0 and you can read back
// what port the OS gave you

#include "rpcpp/RobotRpcServer.h"
#include "rpcpp/RobotRpcServerConnection.h"
#include "rpcpp/RobotRpcServerMethod.h"
#include "rpcpp/RobotRpcSocket.h"
#include "rpcpp/RobotRpcUtil.h"
#include "rpcpp/RobotRpcException.h"


using namespace RobotRpc;


RobotRpcServer::RobotRpcServer()
{
  _introspectionEnabled = false;
  _listMethods = 0;
  _methodHelp = 0;
  _port = 0;
}


RobotRpcServer::~RobotRpcServer()
{
  this->shutdown();
  _methods.clear();
  delete _listMethods;
  delete _methodHelp;
}


// Add a command to the RPC server
void 
RobotRpcServer::addMethod(RobotRpcServerMethod* method)
{
  _methods[method->name()] = method;
}

// Remove a command from the RPC server
void 
RobotRpcServer::removeMethod(RobotRpcServerMethod* method)
{
  MethodMap::iterator i = _methods.find(method->name());
  if (i != _methods.end())
    _methods.erase(i);
}

// Remove a command from the RPC server by name
void 
RobotRpcServer::removeMethod(const std::string& methodName)
{
  MethodMap::iterator i = _methods.find(methodName);
  if (i != _methods.end())
    _methods.erase(i);
}


// Look up a method by name
RobotRpcServerMethod* 
RobotRpcServer::findMethod(const std::string& name) const
{
  MethodMap::const_iterator i = _methods.find(name);
  if (i == _methods.end())
    return 0;
  return i->second;
}


// Create a socket, bind to the specified port, and
// set it in listen mode to make it available for clients.
bool 
RobotRpcServer::bindAndListen(int port, int backlog /*= 5*/)
{
  int fd = RobotRpcSocket::socket();
  if (fd < 0)
  {
    RobotRpcUtil::error("RobotRpcServer::bindAndListen: Could not create socket (%s).", RobotRpcSocket::getErrorMsg().c_str());
    return false;
  }

  this->setfd(fd);

  // Don't block on reads/writes
  if ( ! RobotRpcSocket::setNonBlocking(fd))
  {
    this->close();
    RobotRpcUtil::error("RobotRpcServer::bindAndListen: Could not set socket to non-blocking input mode (%s).", RobotRpcSocket::getErrorMsg().c_str());
    return false;
  }

  //允许立即 重新绑定 此端口，以便不延迟服务器重新启动
  // Allow this port to be re-bound immediately so server re-starts are not delayed
  if ( ! RobotRpcSocket::setReuseAddr(fd))
  {
    this->close();
    RobotRpcUtil::error("RobotRpcServer::bindAndListen: Could not set SO_REUSEADDR socket option (%s).", RobotRpcSocket::getErrorMsg().c_str());
    return false;
  }

  // Bind to the specified port on the default interface
  if ( ! RobotRpcSocket::bind(fd, port))
  {
    this->close();
    RobotRpcUtil::error("RobotRpcServer::bindAndListen: Could not bind to specified port (%s).", RobotRpcSocket::getErrorMsg().c_str());
    return false;
  }

  // Set in listening mode
  if ( ! RobotRpcSocket::listen(fd, backlog))
  {
    this->close();
    std::string sdd = RobotRpcSocket::getErrorMsg().c_str();
    RobotRpcUtil::error("RobotRpcServer::bindAndListen: Could not set socket in listening mode (%s).", RobotRpcSocket::getErrorMsg().c_str());
    return false;
  }

  _port = RobotRpcSocket::get_port(fd);

  RobotRpcUtil::log(2, "RobotRpcServer::bindAndListen: server listening on port %d fd %d", _port, fd);

  // Notify the dispatcher to listen on this source when we are in work()
  _disp.addSource(this, RobotRpcDispatch::ReadableEvent);

  return true;
}


// Process client requests for the specified time
void 
RobotRpcServer::work(double msTime)
{
  RobotRpcUtil::log(2, "RobotRpcServer::work: waiting for a connection");
  _disp.work(msTime);
}



// Handle input on the server socket by accepting the connection
// and reading the rpc request.
unsigned
RobotRpcServer::handleEvent(unsigned)
{
  acceptConnection();
  return RobotRpcDispatch::ReadableEvent;		// Continue to monitor this fd
}


// Accept a client connection request and create a connection to
// handle method calls from the client.
void
RobotRpcServer::acceptConnection()
{
  int s = RobotRpcSocket::accept(this->getfd());
  RobotRpcUtil::log(2, "RobotRpcServer::acceptConnection: socket %d", s);
  if (s < 0)
  {
    //this->close();
    RobotRpcUtil::error("RobotRpcServer::acceptConnection: Could not accept connection (%s).", RobotRpcSocket::getErrorMsg().c_str());
  }
  else if ( ! RobotRpcSocket::setNonBlocking(s))
  {
    RobotRpcSocket::close(s);
    RobotRpcUtil::error("RobotRpcServer::acceptConnection: Could not set socket to non-blocking input mode (%s).", RobotRpcSocket::getErrorMsg().c_str());
  }
  else  // Notify the dispatcher to listen for input on this source when we are in work()
  {
    RobotRpcUtil::log(2, "RobotRpcServer::acceptConnection: creating a connection");
    _disp.addSource(this->createConnection(s), RobotRpcDispatch::ReadableEvent);
  }
}


// Create a new connection object for processing requests from a specific client.
RobotRpcServerConnection*
RobotRpcServer::createConnection(int s)
{
  // Specify that the connection object be deleted when it is closed
  return new RobotRpcServerConnection(s, this, true);
}


void 
RobotRpcServer::removeConnection(RobotRpcServerConnection* sc)
{
  _disp.removeSource(sc);
}


// Stop processing client requests
void 
RobotRpcServer::exit()
{
  _disp.exit();
}


// Close the server socket file descriptor and stop monitoring connections
void 
RobotRpcServer::shutdown()
{
  // This closes and destroys all connections as well as closing this socket
  _disp.clear();
}


// Introspection support
static const std::string LIST_METHODS("system.listMethods");
static const std::string METHOD_HELP("system.methodHelp");
static const std::string MULTICALL("system.multicall");


// List all methods available on a server
class ListMethods : public RobotRpcServerMethod
{
public:
  ListMethods(RobotRpcServer* s) : RobotRpcServerMethod(LIST_METHODS, s) {}

  void execute(XmlRpcValue&, XmlRpcValue& result)
  {
    _server->listMethods(result);
  }

  std::string help() { return std::string("List all methods available on a server as an array of strings"); }
};


// Retrieve the help string for a named method
class MethodHelp : public RobotRpcServerMethod
{
public:
  MethodHelp(RobotRpcServer* s) : RobotRpcServerMethod(METHOD_HELP, s) {}

  void execute(XmlRpcValue& params, XmlRpcValue& result)
  {
    if (params[0].getType() != XmlRpcValue::TypeString)
      throw RobotRpcException(METHOD_HELP + ": Invalid argument type");

    RobotRpcServerMethod* m = _server->findMethod(params[0]);
    if ( ! m)
      throw RobotRpcException(METHOD_HELP + ": Unknown method name");

    result = m->help();
  }

  std::string help() { return std::string("Retrieve the help string for a named method"); }
};

    
// Specify whether introspection is enabled or not. Default is enabled.
void 
RobotRpcServer::enableIntrospection(bool enabled)
{
  if (_introspectionEnabled == enabled)
    return;

  _introspectionEnabled = enabled;

  if (enabled)
  {
    if ( ! _listMethods)
    {
      _listMethods = new ListMethods(this);
      _methodHelp = new MethodHelp(this);
    } else {
      addMethod(_listMethods);
      addMethod(_methodHelp);
    }
  }
  else
  {
    removeMethod(LIST_METHODS);
    removeMethod(METHOD_HELP);
  }
}


void
RobotRpcServer::listMethods(XmlRpcValue& result)
{
  int i = 0;
  result.setSize(_methods.size()+1);
  for (MethodMap::iterator it=_methods.begin(); it != _methods.end(); ++it)
    result[i++] = it->first;

  // Multicall support is built into RobotRpcServerConnection
  result[i] = MULTICALL;
}



