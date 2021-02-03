#ifndef _XCROS_CORE_MACROS_H_
#define _XCROS_CORE_MACROS_H_

#if defined(__GNUC__)
#define ROS_DEPRECATED __attribute__((deprecated))
#define ROS_FORCE_INLINE __attribute__((always_inline))
#elif defined(_MSC_VER)
#define ROS_DEPRECATED
#define ROS_FORCE_INLINE __forceinline
#else
#define ROS_DEPRECATED
#define ROS_FORCE_INLINE inline
#endif

/*
  Windows import/export and gnu http://gcc.gnu.org/wiki/Visibility
  macros.
 */
#if defined(_MSC_VER)
    #define ROS_HELPER_IMPORT __declspec(dllimport)
    #define ROS_HELPER_EXPORT __declspec(dllexport)
    #define ROS_HELPER_LOCAL
#elif __GNUC__ >= 4
    #define ROS_HELPER_IMPORT __attribute__ ((visibility("default")))
    #define ROS_HELPER_EXPORT __attribute__ ((visibility("default")))
    #define ROS_HELPER_LOCAL  __attribute__ ((visibility("hidden")))
#else
    #define ROS_HELPER_IMPORT
    #define ROS_HELPER_EXPORT
    #define ROS_HELPER_LOCAL
#endif

// Ignore warnings about import/exports when deriving from std classes.
#ifdef _MSC_VER
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4275)
#endif

#endif