#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "include/RobotRpcSocket.h"

int main()
{
    using namespace RobotRpc;

    int fd = RobotRpcSocket::socket();
    
    RobotRpcSocket::bind(fd, 1234);

    //进入监听状态，等待用户发起请求
    RobotRpcSocket::listen(fd, 20);

    RobotRpcSocket::accept(fd);

    //向客户端发送数据
    std::string str = "hello world";
    RobotRpcSocket::nbWrite(fd, str, 0);

    RobotRpcSocket::close(fd);

    return 1;
}
