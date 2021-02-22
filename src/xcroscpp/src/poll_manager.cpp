#include "ros/poll_manager.h"

#include <signal.h>

namespace xcros
{

PollManager::PollManager()
  : shutting_down_(false)
{
}

PollManager::~PollManager()
{
  shutdown();
}

void PollManager::start()
{
  shutting_down_ = false;
  thread_ = std::thread(&PollManager::threadFunc, this);
}

void PollManager::shutdown()
{
  if (shutting_down_) return;

  shutting_down_ = true;
  if (thread_.get_id() != std::this_thread::get_id())
  {
    thread_.join();
  }

  boost::recursive_mutex::scoped_lock lock(signal_mutex_);
  poll_signal_.disconnect_all_slots();
}

void PollManager::threadFunc()
{
  xcros::disableAllSignalsInThisThread();

  while (!shutting_down_)
  {
    {
      boost::recursive_mutex::scoped_lock lock(signal_mutex_);
      poll_signal_();
    }

    if (shutting_down_)
    {
      return;
    }

    poll_set_.update(100);
  }
}

boost::signals2::connection PollManager::addPollThreadListener(const VoidFunc& func)
{
  boost::recursive_mutex::scoped_lock lock(signal_mutex_);
  return poll_signal_.connect(func);
}

void PollManager::removePollThreadListener(boost::signals2::connection c)
{
  boost::recursive_mutex::scoped_lock lock(signal_mutex_);
  c.disconnect();
}
}