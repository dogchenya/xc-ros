

#ifndef __APPLE__
#define BOOST_THREAD_HAS_CONDATTR_SET_CLOCK_MONOTONIC
#endif

#include "ros/timer_manager.h"
#include "ros/internal_timer_manager.h"

// check if we have really included the backported boost condition variable
// just in case someone messes with the include order...
#if BOOST_VERSION < 106100
#ifndef USING_BACKPORTED_BOOST_CONDITION_VARIABLE
#error "steady timer needs boost version >= 1.61 or the backported headers!"
#endif
#endif

namespace xcros
{

static InternalTimerManagerPtr g_timer_manager;

InternalTimerManagerPtr getInternalTimerManager()
{
  return g_timer_manager;
}

void initInternalTimerManager()
{
  if (!g_timer_manager)
  {
    g_timer_manager.reset(new InternalTimerManager);
  }
}

} // namespace ros