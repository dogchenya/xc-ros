#ifndef _XCROSCPP_FILE_LOG_H_
#define _XCROSCPP_FILE_LOG_H_

#include "ros/forwards.h"
#include <ros/console.h>
#include "ros/xcroscpp_common.h"
#include "rosconsole/macros_generated.h"

#define ROSCPP_LOG_DEBUG(...) ROS_DEBUG_NAMED("roscpp_internal", __VA_ARGS__)
#define ROSCPP_CONN_LOG_DEBUG(...) ROS_DEBUG_NAMED("roscpp_internal.connections", __VA_ARGS__)

namespace xcros
{

/**
 * \brief internal
 */
namespace file_log
{
  // 20110418 TDS: this appears to be used only by rosout.
  ROSCPP_DECL const std::string& getLogDirectory();
}

}

#endif // ROSCPP_FILE_LOG_H