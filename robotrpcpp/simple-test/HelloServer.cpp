// HelloServer.cpp : Simple XMLRPC server example. Usage: HelloServer serverPort
//
#include "RobotRpc.h"

#include <iostream>
#include <stdlib.h>

#include "RobotRpcServer.h"
#include "RobotRpcServerMethod.h"
#include "XmlRpcValue.h"

using namespace RobotRpc;

// The server
RobotRpcServer s;

// No arguments, result is "Hello".
class Hello : public RobotRpcServerMethod
{
public:
  Hello(RobotRpcServer* s) : RobotRpcServerMethod("Hello", s) {}

  void execute(XmlRpcValue& params, XmlRpcValue& result)
  {
    result = "Hello";
  }

  std::string help() { return std::string("Say hello"); }

} hello(&s);    // This constructor registers the method with the server


// One argument is passed, result is "Hello, " + arg.
class HelloName : public RobotRpcServerMethod
{
public:
  HelloName(RobotRpcServer* s) : RobotRpcServerMethod("HelloName", s) {}

  void execute(XmlRpcValue& params, XmlRpcValue& result)
  {
    std::string resultString = "Hello, ";
    resultString += std::string(params[0]);
    result = resultString;
  }
} helloName(&s);


// A variable number of arguments are passed, all doubles, result is their sum.
class Sum : public RobotRpcServerMethod
{
public:
  Sum(RobotRpcServer* s) : RobotRpcServerMethod("Sum", s) {}

  void execute(XmlRpcValue& params, XmlRpcValue& result)
  {
    int nArgs = params.size();
    double sum = 0.0;
    for (int i=0; i<nArgs; ++i)
      sum += double(params[i]);
    result = sum;
  }
} sum(&s);


int main(int argc, char* argv[])
{
  if (argc != 2) {
    std::cerr << "Usage: HelloServer serverPort\n";
    return -1;
  }
  int port = atoi(argv[1]);

  //log
  RobotRpc::setVerbosity(5);

  // Create the server socket on the specified port
  s.bindAndListen(port);

  // 内省
  // Enable introspection
  s.enableIntrospection(true);

  // Wait for requests indefinitely
  s.work(-1.0);

  return 0;
}

