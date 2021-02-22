
#include "ros/transport/transport.h"

#include "ros/console.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

#if !defined(__ANDROID__)
#include <ifaddrs.h>
#endif

#ifndef NI_MAXHOST
  #define NI_MAXHOST 1025
#endif

namespace xcros
{

Transport::Transport()
: only_localhost_allowed_(false)
{
    char *ros_ip_env = NULL, *ros_hostname_env = NULL;
    #ifdef _MSC_VER
        _dupenv_s(&ros_ip_env, NULL, "ROS_IP");
        _dupenv_s(&ros_hostname_env, NULL, "ROS_HOSTNAME");
    #else
        ros_ip_env = getenv("ROS_IP");
        ros_hostname_env = getenv("ROS_HOSTNAME");
    #endif

    if(ros_hostname_env && !strcmp(ros_hostname_env, "localhost"))
        only_localhost_allowed_ = true;
    else if(ros_ip_env && !strncmp(ros_ip_env, "127.", 4))
        only_localhost_allowed_ = true;
    else if(ros_ip_env && !strcmp(ros_ip_env, "::1"))
    {
        only_localhost_allowed_ = true;
    }
    
    char our_hostname[256] = {0};
    gethostname(our_hostname, sizeof(our_hostname)-1);
    allowed_hosts_.push_back(std::string(our_hostname));
    allowed_hosts_.push_back("localhost");

    #if !defined(__ANDROID__)

    ifaddrs *ifaddr;
    //获取本地网络接口的信息 链表
    if (-1 == getifaddrs(&ifaddr))
    {
        ROS_ERROR("getifaddr() failed");
        return;
    }

    for(ifaddrs *ifa = ifaddr; ifa; ifa = ifaddr->ifa_next)
    {
        if(NULL == ifa->ifa_addr)
        {
            continue; // ifa_addr can be NULL
        }
        int family = ifa->ifa_addr->sa_family;
        if (family != AF_INET && family != AF_INET6)
            continue; // we're only looking for IP addresses
        char addr[NI_MAXHOST] = {0};
        if (getnameinfo(ifa->ifa_addr,
                        (family == AF_INET) ? sizeof(sockaddr_in)
                                            : sizeof(sockaddr_in6),
                        addr, NI_MAXHOST,
                        NULL, 0, NI_NUMERICHOST))
        {
            ROS_ERROR("getnameinfo() failed");
            continue;
        }
        allowed_hosts_.push_back(std::string(addr));
    }
    freeifaddrs(ifaddr);
    #endif
}

bool Transport::isHostAllowed(const std::string &host) const
{
  if (!only_localhost_allowed_)
    return true; // doesn't matter; we'll connect to anybody

  if (host.length() >= 4 && host.substr(0, 4) == std::string("127."))
    return true; // ipv4 localhost
  // now, loop through the list of valid hostnames and see if we find it
  for (std::vector<std::string>::const_iterator it = allowed_hosts_.begin(); 
       it != allowed_hosts_.end(); ++it)
  {
    if (host == *it)
      return true; // hooray
  }
  ROS_WARN("ROS_HOSTNAME / ROS_IP is set to only allow local connections, so "
           "a requested connection to '%s' is being rejected.", host.c_str());
  return false; // sadness
}
    
} // namespace xcros