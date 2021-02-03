#ifndef _XCROS_CORE_SERIALIZATION_MACROS_H_
#define _XCROS_CORE_SERIALIZATION_MACROS_H_

#include <ros/macros.h>

#ifdef ROS_BUILD_SHARED_LIBS // ros is being built around shared libraries
  #ifdef roscpp_serialization_EXPORTS // we are building a shared lib/dll
    #define ROSCPP_SERIALIZATION_DECL ROS_HELPER_EXPORT
  #else // we are using shared lib/dll
    #define ROSCPP_SERIALIZATION_DECL ROS_HELPER_IMPORT
  #endif
#else // ros is being built around static libraries
  #define ROSCPP_SERIALIZATION_DECL
#endif

#endif /* ROSCPP_SERIALIZATION_MACROS_HPP_ */