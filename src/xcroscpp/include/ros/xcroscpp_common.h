#ifndef _XCROS_CPP_COMMON_H_
#define _XCROS_CPP_COMMON_H_

#include <stdint.h>
#include <assert.h>
#include <stddef.h>
#include <string>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>


#define ROS_VERSION_MAJOR @roscpp_VERSION_MAJOR@
#define ROS_VERSION_MINOR @roscpp_VERSION_MINOR@
#define ROS_VERSION_PATCH @roscpp_VERSION_PATCH@
#define ROS_VERSION_COMBINED(major, minor, patch) (((major) << 20) | ((minor) << 10) | (patch))
#define ROS_VERSION ROS_VERSION_COMBINED(ROS_VERSION_MAJOR, ROS_VERSION_MINOR, ROS_VERSION_PATCH)

#define ROS_VERSION_GE(major1, minor1, patch1, major2, minor2, patch2) (ROS_VERSION_COMBINED(major1, minor1, patch1) >= ROS_VERSION_COMBINED(major2, minor2, patch2))
#define ROS_VERSION_MINIMUM(major, minor, patch) ROS_VERSION_GE(ROS_VERSION_MAJOR, ROS_VERSION_MINOR, ROS_VERSION_PATCH, major, minor, patch)

#ifdef ROS_BUILD_SHARED_LIBS // ros is being built around shared libraries
  #ifdef roscpp_EXPORTS // we are building a shared lib/dll
    #define ROSCPP_DECL ROS_HELPER_EXPORT
  #else // we are using shared lib/dll
    #define ROSCPP_DECL ROS_HELPER_IMPORT
  #endif
#else // ros is being built around static libraries
  #define ROSCPP_DECL
#endif

namespace xcros
{
void disableAllSignalsInThisThread();
}

#endif