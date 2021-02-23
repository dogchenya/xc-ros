#include "rpcpp/RobotRpc.h"

#if defined(_MSC_VER)
# pragma warning(disable:4786)    // identifier was truncated in debug info
#endif

#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "rpcpp/RobotRpcServer.h"
#include "rpcpp/RobotRpcServerMethod.h"
#include "rpcpp/XmlRpcValue.h"

using namespace RobotRpc;

// The server
RobotRpcServer s;

class TestBase64 : public RobotRpcServerMethod
{
public:
    TestBase64(RobotRpcServer* server) : RobotRpcServerMethod("TestBase64", server)
    {
    }

    void execute(XmlRpcValue& params, XmlRpcValue& result)
    {
        std::ifstream infile("pngnow.png", std::ios::binary);
        if (infile.fail())
        infile.open("../pngnow.png", std::ios::binary);
        if (infile.fail())
        result = "Could not open file pngnow.png";
        else {
            XmlRpcValue::BinaryData& data = result;
            int n = 0;
            for (;; ++n) {
                char c = infile.get();
                if (infile.eof()) break;
                data.push_back(c);
            }
            std::cerr << "Read " << n << " bytes from pngnow.png\n";
        }
    }
}TestBase64(&s);

int main(int argc, char* argv[])
{
  if (argc != 2) {
    std::cerr << "Usage: TestBase64Server serverPort\n";
    return -1;
  }
  int port = atoi(argv[1]);

  //RobotRpc::setVerbosity(5);

  // Create the server socket on the specified port
  s.bindAndListen(port);

  // Wait for requests indefinitely
  s.work(-1.0);

  return 0;
}