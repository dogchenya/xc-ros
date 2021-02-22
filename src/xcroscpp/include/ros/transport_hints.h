#ifndef _XCROSCPP_TRANSPORT_HINTS_H_
#define _XCROSCPP_TRANSPORT_HINTS_H_

#include "xcroscpp_common.h"
#include "ros/forwards.h"

#include <boost/lexical_cast.hpp>

namespace xcros
{

class ROSCPP_DECL TransportHints
{
public:
  /**
   * \brief Specifies a reliable transport.  Currently this means TCP
   */
  TransportHints& reliable()
  {
    tcp();

    return *this;
  }

  /**
   * \brief Explicitly specifies the TCP transport
   */
  TransportHints& tcp()
  {
    transports_.push_back("TCP");
    return *this;
  }

  /**
   * \brief If a TCP transport is used, specifies whether or not to use TCP_NODELAY to provide
   * a potentially lower-latency connection.
   *
   * \param nodelay [optional] Whether or not to use TCP_NODELAY.  Defaults to true.
   */
  TransportHints& tcpNoDelay(bool nodelay = true)
  {
    options_["tcp_nodelay"] = nodelay ? "true" : "false";
    return *this;
  }

  /**
   * \brief Returns whether or not this TransportHints has specified TCP_NODELAY
   */
  bool getTCPNoDelay()
  {
    M_string::iterator it = options_.find("tcp_nodelay");
    if (it == options_.end())
    {
      return false;
    }

    const std::string& val = it->second;
    if (val == "true")
    {
      return true;
    }

    return false;
  }

  /**
   * \brief If a UDP transport is used, specifies the maximum datagram size.
   *
   * \param size The size, in bytes
   */
  TransportHints& maxDatagramSize(int size)
  {
    options_["max_datagram_size"] = boost::lexical_cast<std::string>(size);
    return *this;
  }

  /**
   * \brief Returns the maximum datagram size specified on this TransportHints, or 0 if
   * no size was specified.
   */
  int getMaxDatagramSize()
  {
    M_string::iterator it = options_.find("max_datagram_size");
    if (it == options_.end())
    {
      return 0;
    }

    return boost::lexical_cast<int>(it->second);
  }

  /**
   * \brief Specifies an unreliable transport.  Currently this means UDP.
   */
  TransportHints& unreliable()
  {
    udp();

    return *this;
  }

  /**
   * \brief Explicitly specifies a UDP transport.
   */
  TransportHints& udp()
  {
    transports_.push_back("UDP");
    return *this;
  }

  /**
   * \brief Returns a vector of transports, ordered by preference
   */
  const V_string& getTransports() { return transports_; }
  /**
   * \brief Returns the map of options created by other methods inside TransportHints
   */
  const M_string& getOptions() { return options_; }

private:
  V_string transports_;
  M_string options_;
};

} // namespace xcros



#endif