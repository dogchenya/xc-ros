#ifndef _XCROSCPP_TOPIC_H_
#define _XCROSCPP_TOPIC_H_

#include "ros/xcroscpp_common.h"
#include "ros/node_handle.h"
#include "ros/duration.h"
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

namespace xcros
{
namespace topic
{

ROSCPP_DECL void waitForMessageImpl(SubscribeOptions& ops, const boost::function<bool(void)>& ready_pred, NodeHandle& nh, xcros::Duration timeout);

} // namespace topic
} // namespace xcros


#endif