#ifndef _ROBOTRPCSERVERMETHOD_H_
#define _ROBOTRPCSERVERMETHOD_H_

#if defined(_MSC_VER)
# pragma warning(disable:4786)    // identifier was truncated in debug info
#endif

#include "RobotRpcDecl.h"

#ifndef MAKEDEPEND
# include <string>
#endif

namespace RobotRpc{

    class XmlRpcValue;

    class RobotRpcServer;
    
    class ROBOTRPCPP_DECL RobotRpcServerMethod{
    
    public:
        RobotRpcServerMethod(const std::string& name, RobotRpcServer* server = 0);

        virtual ~RobotRpcServerMethod();

        std::string& name() {return _name;}

        virtual void execute(XmlRpcValue& params, XmlRpcValue& result) = 0;

        virtual std::string help() { return std::string(); }

    protected:
        std::string _name;
        
        RobotRpcServer* _server;

    };
}// namespace RobotRpc



#endif