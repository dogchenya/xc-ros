#ifndef _XCROSCPP_SPINNER_H_
#define _XCROSCPP_SPINNER_H_

#include "ros/types.h"
#include "ros/xcroscpp_common.h"

#include <boost/shared_ptr.hpp>

namespace xcros
{
class NodeHandle;
class CallbackQueue;

/**
 * \brief Abstract interface for classes which spin on a callback queue.
 */
class ROSCPP_DECL Spinner
{
public:
  virtual ~Spinner() {}

  /**
   * \brief Spin on a callback queue (defaults to the global one).  Blocks until roscpp has been shutdown.
   */
  virtual void spin(CallbackQueue* queue = 0) = 0;
};

/**
 * \brief Spinner which runs in a single thread.
 */
class SingleThreadedSpinner : public Spinner
{
public:
  virtual void spin(CallbackQueue* queue = 0);
};

/**
 * \brief Spinner which spins in multiple threads.
 */
class ROSCPP_DECL MultiThreadedSpinner : public Spinner
{
public:
  /**
   * \param thread_count Number of threads to use for calling callbacks.   0 will
   * automatically use however many hardware threads exist on your system.
   */
  MultiThreadedSpinner(uint32_t thread_count = 0);

  virtual void spin(CallbackQueue* queue = 0);

private:
  uint32_t thread_count_;
};

class AsyncSpinnerImpl;
typedef boost::shared_ptr<AsyncSpinnerImpl> AsyncSpinnerImplPtr;

/**
 * \brief AsyncSpinner is a spinner that does not conform to the abstract Spinner interface.  Instead,
 * it spins asynchronously when you call start(), and stops when either you call stop(), ros::shutdown()
 * is called, or its destructor is called
 *
 * AsyncSpinner is reference counted internally, so if you copy one it will continue spinning until all
 * copies have destructed (or stop() has been called on one of them)
 */
class ROSCPP_DECL AsyncSpinner
{
public:
  /**
   * \brief Simple constructor.  Uses the global callback queue
   * \param thread_count The number of threads to use.  A value of 0 means to use the number of processor cores
   */
  AsyncSpinner(uint32_t thread_count);

  /**
   * \brief Constructor with custom callback queue
   * \param thread_count The number of threads to use.  A value of 0 means to use the number of processor cores
   * \param queue The callback queue to operate on.  A null value means to use the global queue
   */
  AsyncSpinner(uint32_t thread_count, CallbackQueue* queue);



  /**
   * \brief Check if the spinner can be started. The spinner shouldn't be started if
   * another single-threaded spinner is already operating on the callback queue.
   *
   * This function is not necessary anymore. start() will always try to start spinning
   * and throw a std::runtime_error if it failed.
   */
  // TODO: deprecate in L-turtle
  bool canStart();
  /**
   * \brief Start this spinner spinning asynchronously
   */
  void start();
  /**
   * \brief Stop this spinner from running
   */
  void stop();

private:
  AsyncSpinnerImplPtr impl_;
};

}


#endif