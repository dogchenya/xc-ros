#ifndef _XCROSCONSOLE_STATIC_ASSERT_H_
#define _XCROSCONSOLE_STATIC_ASSERT_H_

#include <boost/static_assert.hpp>

/**
 * \def ROS_COMPILE_ASSERT(cond)
 * \brief Compile-time assert.
 *
 * Only works with compile time statements, ie:
 @verbatim
   struct A
   {
     uint32_t a;
   };
   ROS_COMPILE_ASSERT(sizeof(A) == 4);
 @endverbatim
 */
#define ROS_COMPILE_ASSERT(cond) BOOST_STATIC_ASSERT(cond)

/**
 * \def ROS_STATIC_ASSERT(cond)
 * \brief Compile-time assert.
 *
 * Only works with compile time statements, ie:
 @verbatim
   struct A
   {
     uint32_t a;
   };
   ROS_STATIC_ASSERT(sizeof(A) == 4);
 @endverbatim
 */
#define ROS_STATIC_ASSERT(cond) BOOST_STATIC_ASSERT(cond)


#endif // ROSCONSOLE_STATIC_ASSERT_H