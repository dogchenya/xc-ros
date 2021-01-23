#include "RobotRpcServerMethod.h"
#include "RobotRpcServer.h"


namespace RobotRpc{


    RobotRpcServerMethod::RobotRpcServerMethod(const std::string& name, RobotRpcServer* server)
    {
        _name = name;
        _server = server;
        if (_server) _server->addMethod(this);
    }

    RobotRpcServerMethod::~RobotRpcServerMethod()
    {
        if (_server) _server->removeMethod(this);
    }

}

