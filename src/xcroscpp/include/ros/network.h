#ifndef XCROSCPP_NETWORK_H
#define XCROSCPP_NETWORK_H

#include "ros/forwards.h"
#include "ros/xcroscpp_common.h"

namespace xcros
{

/**
 * \brief internal
 */
namespace network
{

bool splitURI(const std::string& uri, std::string& host, uint32_t& port);
const std::string& getHost(); 
uint16_t getTCPROSPort();

} // namespace network

} // namespace xcros

#endif