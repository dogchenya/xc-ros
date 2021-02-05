#ifndef _XCROSCPP_TIMER_H_
#define _XCROSCPP_TIMER_H_

#include "ros/xcroscpp_common.h"
#include "ros/forwards.h"
#include "ros/timer_options.h"

namespace xcros
{
    
class ROSCPP_DECL Timer
{
public:
  Timer() {}
  Timer(const Timer& rhs);
  ~Timer();

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
   * \param reset Whether to reset the timer. If true, timer ignores elapsed time and next cb occurs at now()+period
   */
  void setPeriod(const Duration& period, bool reset=true);

  bool hasStarted() const { return impl_->hasStarted(); }
  bool isValid() { return impl_ && impl_->isValid(); }
  operator void*() { return isValid() ? (void*)1 : (void*)0; }

  bool operator<(const Timer& rhs)
  {
    return impl_ < rhs.impl_;
  }

  bool operator==(const Timer& rhs)
  {
    return impl_ == rhs.impl_;
  }

  bool operator!=(const Timer& rhs)
  {
    return impl_ != rhs.impl_;
  }

private:
  Timer(const TimerOptions& ops);

  class Impl
  {
  public:
    Impl();
    ~Impl();

    bool hasStarted() const;
    bool isValid();
    bool hasPending();
    void setPeriod(const Duration& period, bool reset=true);

    void start();
    void stop();

    bool started_;
    int32_t timer_handle_;

    Duration period_;
    TimerCallback callback_;
    CallbackQueueInterface* callback_queue_;
    VoidConstWPtr tracked_object_;
    bool has_tracked_object_;
    bool oneshot_;
  };
  typedef boost::shared_ptr<Impl> ImplPtr;
  typedef boost::weak_ptr<Impl> ImplWPtr;

  ImplPtr impl_;

  friend class NodeHandle;
};    


} // namespace xcros


#endif