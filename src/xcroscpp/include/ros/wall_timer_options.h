#ifndef _XCROSCPP_WALL_TIMER_OPTIONS_H_
#define _XCROSCPP_WALL_TIMER_OPTIONS_H_

#include "ros/xcroscpp_common.h"
#include "ros/forwards.h"

namespace xcros
{
struct ROSCPP_DECL WallTimerOptions
{
  WallTimerOptions()
  : period(0.1)
  , callback_queue(0)
  , oneshot(false)
  , autostart(true)
  {
  }

  /*
   * \brief Constructor
   * \param
   */
  WallTimerOptions(WallDuration _period, const WallTimerCallback& _callback, CallbackQueueInterface* _queue, 
                   bool oneshot = false, bool autostart = true)
  : period(_period)
  , callback(_callback)
  , callback_queue(_queue)
  , oneshot(oneshot)
  , autostart(autostart)
  {}

  WallDuration period;                                              ///< The period to call the callback at
  WallTimerCallback callback;                                       ///< The callback to call

  CallbackQueueInterface* callback_queue;                           ///< Queue to add callbacks to.  If NULL, the global callback queue will be used

  /**
   * A shared pointer to an object to track for these callbacks.  If set, the a weak_ptr will be created to this object,
   * and if the reference count goes to 0 the subscriber callbacks will not get called.
   *
   * \note Note that setting this will cause a new reference to be added to the object before the
   * callback, and for it to go out of scope (and potentially be deleted) in the code path (and therefore
   * thread) that the callback is invoked from.
   */
  VoidConstPtr tracked_object;

  bool oneshot;
  bool autostart;
};  
    
} // namespace xcros



#endif