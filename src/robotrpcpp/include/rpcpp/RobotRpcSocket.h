//service
//***socket()--->>>bind()********
//******************|************
//***accept()<<<---listen()******
//******|************************
//****write()--->>>close()*******



//client
//***socket()--->>>connect()*****
//********************|**********
//***close()<<<---read()*********

#ifndef _XMLRPCSOCKET_H_
#define _XMLRPCSOCKET_H_

#include <string>
#include "rpcpp/RobotRpcDecl.h"

namespace RobotRpc{

    class ROBOTRPCPP_DECL RobotRpcSocket
    {
    private:
        /* data */
    public:

    //functions
        // RobotRpcSocket(/* args */);
        // ~RobotRpcSocket();

        static void RpcTest();

        //! 创建TCP socket. failed if return -1
        static int socket();

        //! 关闭sockets
        static void close(int fd);

        //! 非阻塞I/O
        static bool setNonBlocking(int fd);

        static bool nbRead(int fd, std::string& s, bool *eof);

        static bool nbWrite(int fd, std::string& s, int *bytesSoFar);

        static bool setReuseAddr(int fd);

        //! 绑定指定端口
        static bool bind(int fd, int port);

        static int get_port(int fd);

        static bool listen(int fd, int backlog);

        //! Accept a client connection request
        static int accept(int fd);

        //! Connect a socket to a server (from a client)
        static bool connect(int fd, std::string& host, int port);

        //! Returns last errno
        static int getError();

        //! Returns message corresponding to last error
        static std::string getErrorMsg();

        //! Returns message corresponding to error
        static std::string getErrorMsg(int error);
        
    //variable
        static bool sb_use_ipv6_;


    };
    
}

#endif