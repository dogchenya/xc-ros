#ifndef ROBOTRPCPP_DECL_H_INCLUDED
#define ROBOTRPCPP_DECL_H_INCLUDED

//#include <ros/macros.h>

#ifdef ROS_BUILD_SHARED_LIBS // ros is being built around shared libraries
  #ifdef xmlrpcpp_EXPORTS // we are building a shared lib/dll
    #define XMLRPCPP_DECL ROS_HELPER_EXPORT
  #else // we are using shared lib/dll
    #define XMLRPCPP_DECL ROS_HELPER_IMPORT
  #endif
#else // ros is being built around static libraries
  #define ROBOTRPCPP_DECL
#endif

#endif /* ROBOTRPCPP_DECL_H_INCLUDED */