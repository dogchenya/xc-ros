#include"RobotRpcSocket.h"

# include <iostream>

#ifndef MAKEDEPEND

#if defined(_WINDOWS)
# include <stdio.h>
# include <winsock2.h>
# include <ws2tcpip.h>
//# pragma lib(WS2_32.lib)

// MS VS10 actually has these definitions (as opposed to earlier versions),
// so if present, temporarily disable them and reset to WSA codes for this file only.
#ifdef EAGAIN
  #undef EAGAIN
#endif
#ifdef EINTR
  #undef EINTR
#endif
#ifdef EINPROGRESS
  #undef EINPROGRESS
#endif
#ifdef  EWOULDBLOCK
  #undef EWOULDBLOCK
#endif
#ifdef ETIMEDOUT
  #undef ETIMEDOUT
#endif
# define EAGAIN		WSATRY_AGAIN
# define EINTR			WSAEINTR
# define EINPROGRESS	WSAEINPROGRESS
# define EWOULDBLOCK	WSAEWOULDBLOCK
# define ETIMEDOUT	    WSAETIMEDOUT
#else
extern "C" {
# include <unistd.h>
# include <stdio.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
# include <errno.h>
# include <fcntl.h>
# include <string.h>
# include <stdlib.h>
# include <arpa/inet.h>
}
#endif  // _WINDOWS

#endif // MAKEDEPEND

// MSG_NOSIGNAL does not exists on OS X
#if defined(__APPLE__) || defined(__MACH__)
# ifndef MSG_NOSIGNAL
#   define MSG_NOSIGNAL SO_NOSIGPIPE
# endif
#endif

using namespace RobotRpc;

bool RobotRpcSocket::sb_use_ipv6_ = false;

#if defined(_WINDOWS)

static void initWinSock()
{
  static bool wsInit = false;
  if (! wsInit)
  {
    WORD wVersionRequested = MAKEWORD( 2, 0 );
    WSADATA wsaData;
    WSAStartup(wVersionRequested, &wsaData);
    wsInit = true;
  }
}

#else

#define initWinSock()

#endif // _WINDOWS

// These errors are not considered fatal for an IO operation; the operation will be re-tried.
static inline bool
nonFatalError()
{
  int err = RobotRpcSocket::getError();
  // errno == EINTR,表示系统当前中断了
  // 如果errno == EAGAIN 或者 EWOULDBLOCK，非阻塞socket直接忽略;如果是阻塞的socket,一般是读写操作超时了，还未返回
  // 如果 errno == EINPROGRESS，表示正在处理中，否则表示连接出错了，需要关闭重连
  return (err == EINPROGRESS || err == EAGAIN || err == EWOULDBLOCK || err == EINTR);
}

int RobotRpcSocket::socket()
{
    initWinSock();
    //SOCK_STREAM : 表示面向连接的数据传输方式 TCP
    //SOCK_DGRAM  : 无连接，不安全 但效率高 UDP 
    //注意：SOCK_DGRAM 没有想象中的糟糕，不会频繁的丢失数据，数据错误只是小概率事件。
    return (int) ::socket(sb_use_ipv6_ ? AF_INET6 : AF_INET, SOCK_STREAM, 0);
}

void RobotRpcSocket::close(int fd)
{
    //RobotRpcUtil::log(4, "RobotRpcSocket::close: fd %d.", fd);
    #if defined(_WINDOWS)
    closesocket(fd);
    #else
    ::close(fd);
    #endif // _WINDOWS
}

bool RobotRpcSocket::setReuseAddr(int fd)
{
    int sflag = 1;
    //level = SOL_SOCKET:套接字级别设置
    //optname = SO_REUSEADDR:打开或关闭地址复用功能
    //后两位 参数长度和值
    return (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&sflag, sizeof(sflag)) == 0);
}

bool RobotRpcSocket::bind(int fd, int port)
{
    sockaddr_storage ss;
    socklen_t ss_len;
    memset(&ss, 0, sizeof(ss));

    if (sb_use_ipv6_)
    {
        sockaddr_in6 *address = (sockaddr_in6 *)&ss;
        ss_len = sizeof(sockaddr_in6);

        address->sin6_family = AF_INET6;
        address->sin6_addr = in6addr_any;
        address->sin6_port = htons((u_short) port);
    }
    else
    {
        sockaddr_in *address = (sockaddr_in *)&ss;
        ss_len = sizeof(sockaddr_in);

        address->sin_family = AF_INET;
        address->sin_addr.s_addr = htonl(INADDR_ANY);
        address->sin_port = htons((u_short) port);
    }

    return (::bind(fd, (sockaddr*)&ss, ss_len) == 0);
}

bool RobotRpcSocket::listen(int fd, int backlog)
{
    //backlog : 等待连接队列的最大长度
    return (::listen(fd, backlog));
}

int RobotRpcSocket::accept(int fd)
{
    struct sockaddr_in addr;
    #if defined(_WINDOWS)
    int
    #else
    socklen_t
    #endif
        addrlen = sizeof(addr);
    // accept will truncate the address if the buffer is too small.
    // As we are not using it, no special case for IPv6
    // has to be made.
    return (int) ::accept(fd, (struct sockaddr*)&addr, &addrlen);
}

bool RobotRpcSocket::connect(int fd, std::string& host, int port)
{
    sockaddr_storage ss;
    socklen_t ss_len;
    memset(&ss, 0, sizeof(ss));

    struct addrinfo* addr;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    if (getaddrinfo(host.c_str(), NULL, &hints, &addr) != 0)
    {
        fprintf(stderr, "Couldn't find an %s address for [%s]\n", sb_use_ipv6_ ? "AF_INET6" : "AF_INET", host.c_str());
        return false;
    }

    bool found = false;
    struct addrinfo* it = addr;

    for (; it; it = it->ai_next)
    {
        if (!sb_use_ipv6_ && it->ai_family == AF_INET)
        {
        sockaddr_in *address = (sockaddr_in *)&ss;
        ss_len = sizeof(sockaddr_in);

        memcpy(address, it->ai_addr, it->ai_addrlen);
        address->sin_family = it->ai_family;
        address->sin_port = htons((u_short) port);

        //XmlRpcUtil::log(5, "found host as %s\n", inet_ntoa(address->sin_addr));
        found = true;
        break;
        }
        if (sb_use_ipv6_ && it->ai_family == AF_INET6)
        {
        sockaddr_in6 *address = (sockaddr_in6 *)&ss;
        ss_len = sizeof(sockaddr_in6);

        memcpy(address, it->ai_addr, it->ai_addrlen);
        address->sin6_family = it->ai_family;
        address->sin6_port = htons((u_short) port);

        char buf[128];
        // TODO IPV6: check if this also works under Windows
        //XmlRpcUtil::log(5, "found ipv6 host as %s\n", inet_ntop(AF_INET6, (void*)&(address->sin6_addr), buf, sizeof(buf)));
        found = true;
        break;
        }
    }

    if (!found)
    {
        printf("Couldn't find an %s address for [%s]\n", sb_use_ipv6_ ? "AF_INET6" : "AF_INET", host.c_str());
        freeaddrinfo(addr);
        return false;
    }

    // For asynch operation, this will return EWOULDBLOCK (windows) or
    // EINPROGRESS (linux) and we just need to wait for the socket to be writable...
    int result = ::connect(fd, (sockaddr*)&ss, ss_len);
    if (result != 0 ) {
        int error = getError();
        if ( (error != EINPROGRESS) && error != EWOULDBLOCK) { // actually, should probably do a platform check here, EWOULDBLOCK on WIN32 and EINPROGRESS otherwise
                printf("::connect error = %d\n", getError());
        }
    }

    freeaddrinfo(addr);

    return result == 0 || nonFatalError();
}

bool RobotRpcSocket::nbRead(int fd, std::string& s, bool *eof)
{
    //一次4M
    const int READ_SIZE = 4096;   // Number of bytes to attempt to read at a time
    char readBuf[READ_SIZE];

    bool wouldBlock = false;
    *eof = false;

    while ( ! wouldBlock && ! *eof) {
    #if defined(_WINDOWS)
        int n = recv(fd, readBuf, READ_SIZE-1, 0);
    #else
        int n = read(fd, readBuf, READ_SIZE-1);
    #endif
        //XmlRpcUtil::log(5, "XmlRpcSocket::nbRead: read/recv returned %d.", n);

        //当read 返回值 > 0时，表示实际从缓存中读入的字节数目
        if (n > 0) {
        readBuf[n] = 0;
        s.append(readBuf, n);
        //返回值 == 0时，表示对端已经关闭了socket
        } else if (n == 0) {
        *eof = true;
        } else if (nonFatalError()) {
        //
        wouldBlock = true;
        } else {
        return false;   // Error
        }
    }
    // Watch for integer overrun
    if (s.length() > size_t(__INT_MAX__)) {
        // XmlRpcUtil::error("XmlRpcSocket::nbRead: text size (%u) exceeds the maximum allowed size (%s).",
        //                 s.length(), __INT_MAX__);
        s.clear();
        return false;
    }
    return true;
}

bool RobotRpcSocket::nbWrite(int fd, std::string& s, int *bytesSoFar)
{
    // Watch for integer overrun
    if (s.length() > size_t(__INT_MAX__)) {
        // XmlRpcUtil::error("XmlRpcSocket::nbWrite: text size (%u) exceeds the maximum allowed size(%s)",
        //                 s.length(), __INT_MAX__);
        return false;
    }
    int nToWrite = int(s.length()) - *bytesSoFar;
    char *sp = const_cast<char*>(s.c_str()) + *bytesSoFar;
    bool wouldBlock = false;

    while ( nToWrite > 0 && ! wouldBlock ) {
    #if defined(_WINDOWS)
        int n = send(fd, sp, nToWrite, 0);
    #else
        int n = write(fd, sp, nToWrite);
    #endif
        //XmlRpcUtil::log(5, "XmlRpcSocket::nbWrite: send/write returned %d.", n);

        if (n > 0) {
        sp += n;
        *bytesSoFar += n;
        nToWrite -= n;
        } else if (nonFatalError()) {
        wouldBlock = true;
        } else {
        return false;   // Error
        }
    }
    return true;
}

// Returns last errno
int RobotRpcSocket::getError()
{
    #if defined(_WINDOWS)
    return WSAGetLastError();
    #else
    return errno;
    #endif
}


// Returns message corresponding to last errno
std::string RobotRpcSocket::getErrorMsg()
{
    return getErrorMsg(getError());
}

// Returns message corresponding to errno... well, it should anyway
std::string RobotRpcSocket::getErrorMsg(int error)
{
    char err[60];
    #ifdef _MSC_VER
    strerror_s(err,60,error);
    #else
    snprintf(err,sizeof(err),"%s",strerror(error));
    #endif
    return std::string(err);
}

int RobotRpcSocket::get_port(int socket)
{
    sockaddr_storage ss;
    socklen_t ss_len = sizeof(ss);
    getsockname(socket, (sockaddr *)&ss, &ss_len);

    sockaddr_in *sin = (sockaddr_in *)&ss;
    sockaddr_in6 *sin6 = (sockaddr_in6 *)&ss;
    
    switch (ss.ss_family)
    {
        case AF_INET:
        return ntohs(sin->sin_port);
        case AF_INET6:
        return ntohs(sin6->sin6_port);
    }  
    return 0;
}

void RobotRpcSocket::RpcTest()
{
    std::cout << "yes , you success" << std::endl;
}