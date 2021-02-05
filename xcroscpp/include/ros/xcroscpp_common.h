#ifndef _XCROS_CPP_COMMON_H_
#define _XCROS_CPP_COMMON_H_

#include <stdint.h>
#include <assert.h>
#include <stddef.h>
#include <string>

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