#include <iostream>

#include <RobotRpcSocket.h>

int main()
{
    std::cout<< "hello RobotRpcpp" <<std::endl;

    using namespace RobotRpc;
    RobotRpcSocket::RpcTest();

    return 0;
}