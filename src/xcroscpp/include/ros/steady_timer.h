#ifndef _XCROSCPP_STEADY_TIMER_H_
#define _XCROSCPP_STEADY_TIMER_H_

#include "ros/xcroscpp_common.h"
#include "ros/forwards.h"
#include "ros/steady_timer_options.h"

namespace xcros
{

/**
 * \brief Manages a steady-clock timer callback
 *
 * A SteadyTimer should always be created through a call to NodeHandle::createSteadyTimer(), or copied from one
 * that was. Once all copies of a specific
 * SteadyTimer go out of scope, the callback associated with that handle will stop
 * being called.
 */
class ROSCPP_DECL SteadyTimer
{
public:
  SteadyTimer() {}
  SteadyTimer(const SteadyTimer& rhs);
  ~SteadyTimer();

  /**
   * \brief Start the timer.  Does nothing if the timer is already started.
   */
  void start();
  /**
   * \brief Stop the timer.  Once this call returns, no more callbacks will be called.  Does
   * nothing if the timer is already stopped.
   */
  void stop();

  /**
   * \brief Returns whether or not the timer has any pending events to call.
   */
  bool hasPending();

  /**
   * \brief Set the period of this timer
   */
  void setPeriod(const WallDuration& period, bool reset=true);

  bool isValid() { return impl_ && impl_->isValid(); }
  operator void*() { return isValid() ? (void *) 1 : (void *) 0; }

  bool operator<(const SteadyTimer& rhs)
  {
    return impl_ < rhs.impl_;
  }

  bool operator==(const SteadyTimer& rhs)
  {
    return impl_ == rhs.impl_;
  }

  bool operator!=(const SteadyTimer& rhs)
  {
    return impl_ != rhs.impl_;
  }

private:
  SteadyTimer(const SteadyTimerOptions& ops);

  class Impl
  {
  public:
    Impl();
    ~Impl();

    bool isValid();
    bool hasPending();
    void setPeriod(const WallDuration &period, bool reset=true);

    void start();
    void stop();

    bool started_;
    int32_t timer_handle_;

    WallDuration period_;
    SteadyTimerCallback callback_;
    CallbackQueueInterface *callback_queue_;
    VoidConstWPtr tracked_object_;
    bool has_tracked_object_;
    bool oneshot_;
  };
  typedef boost::shared_ptr<Impl> ImplPtr;
  typedef boost::weak_ptr<Impl> ImplWPtr;

  ImplPtr impl_;

  friend class NodeHandle;
};

}

#endif
