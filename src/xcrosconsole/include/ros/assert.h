#ifndef _XCROSCONSOLE_ASSERT_H_
#define _XCROSCONSOLE_ASSERT_H_

#include <ros/platform.h>
#include "rosconsole/macros_generated.h"

#ifdef WIN32
# if defined (__MINGW32__)
#  define ROS_ISSUE_BREAK() DebugBreak();
# else // MSVC
#  define ROS_ISSUE_BREAK() __debugbreak();
# endif
#elif defined(__powerpc64__)
# define ROS_ISSUE_BREAK() asm volatile ("tw 31,1,1");
#elif defined(__i386__) || defined(__ia64__) || defined(__x86_64__)
# define ROS_ISSUE_BREAK() asm("int $3");
#else
# include <stdlib.h>
# define ROS_ISSUE_BREAK() abort();
#endif

#ifndef NDEBUG
#ifndef ROS_ASSERT_ENABLED
#define ROS_ASSERT_ENABLED
#endif
#endif

#ifdef ROS_ASSERT_ENABLED
#define ROS_BREAK() \
  do { \
    ROS_FATAL("BREAKPOINT HIT\n\tfile = %s\n\tline=%d\n", __FILE__, __LINE__); \
    ROS_ISSUE_BREAK() \
  } while (false)

#define ROS_ASSERT(cond) \
  do { \
    if (!(cond)) { \
      ROS_FATAL("ASSERTION FAILED\n\tfile = %s\n\tline = %d\n\tcond = %s\n", __FILE__, __LINE__, #cond); \
      ROS_ISSUE_BREAK() \
    } \
  } while (false)

#define ROS_ASSERT_MSG(cond, ...) \
  do { \
    if (!(cond)) { \
      ROS_FATAL("ASSERTION FAILED\n\tfile = %s\n\tline = %d\n\tcond = %s\n\tmessage = ", __FILE__, __LINE__, #cond); \
      ROS_FATAL(__VA_ARGS__); \
      ROS_FATAL("\n"); \
      ROS_ISSUE_BREAK(); \
    } \
  } while (false)

#define ROS_ASSERT_CMD(cond, cmd) \
  do { \
    if (!(cond)) { \
      cmd; \
    } \
  } while (false)


#else
#define ROS_BREAK()
#define ROS_ASSERT(cond)
#define ROS_ASSERT_MSG(cond, ...)
#define ROS_ASSERT_CMD(cond, cmd)
#endif

#endif