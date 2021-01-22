#include "include/RobotRpcSocket.h"

int main()
{
    using namespace RobotRpc;

    int fd = RobotRpcSocket::socket();

    std::string ip = "127.0.0.1";
    RobotRpcSocket::connect(fd, ip, 1234);

    std::string output;
    bool bb = true;
    RobotRpcSocket::nbRead(fd, output, &bb);

    return 1;
}