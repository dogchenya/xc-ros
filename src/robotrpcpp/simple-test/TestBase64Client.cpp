#include "rpcpp/RobotRpc.h"

#if defined(_MSC_VER)
# pragma warning(disable:4786)    // identifier was truncated in debug info
#endif

#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "rpcpp/RobotRpcClient.h"
#include "rpcpp/RobotRpcServerMethod.h"
#include "rpcpp/XmlRpcValue.h"

using namespace RobotRpc;

int main(int argc, char* argv[])
{
  if (argc != 4) {
    std::cerr << "Usage: TestBase64Client serverHost serverPort outputFile\n";
    return -1;
  }
  int port = atoi(argv[2]);

  //RobotRpc::setVerbosity(5);
  RobotRpcClient c(argv[1], port);

  XmlRpcValue noArgs, result;
  if (c.execute("TestBase64", noArgs, result))
  {
    try
    {
        const XmlRpcValue::BinaryData& data = result;
        std::ofstream outfile(argv[3], std::ios::binary | std::ios::trunc);
        if (outfile.fail())
        std::cerr << "Error opening " << argv[3] << " for output.\n";
        else
        {
            int n = int(data.size());
            for (int i=0; i<n; ++i)
                outfile << data[i];
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
  }
  else
    std::cout << "Error calling 'TestBase64'\n\n";

  return 0;
}