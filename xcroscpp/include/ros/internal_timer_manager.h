#ifndef _XCROSCPP_INTERNAL_TIMER_MANAGER_H_
#define _XCROSCPP_INTERNAL_TIMER_MANAGER_H_

#include "ros/forwards.h"
#include <ros/time.h>
#include "ros/xcroscpp_common.h"

namespace xcros
{

template<typename T, typename D, typename E> class TimerManager;
typedef TimerManager<SteadyTime, WallDuration, SteadyTimerEvent> InternalTimerManager;
typedef boost::shared_ptr<InternalTimerManager> InternalTimerManagerPtr;

ROSCPP_DECL void initInternalTimerManager();
ROSCPP_DECL InternalTimerManagerPtr getInternalTimerManager();

} // namespace xcros

#endif // _XCROSCPP_INTERNAL_TIMER_MANAGER_H