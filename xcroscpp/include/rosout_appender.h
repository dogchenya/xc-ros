#ifndef _XCROSCPP_ROSOUT_APPENDER_H_
#define _XCROSCPP_ROSOUT_APPENDER_H_

#include <memory>
#include <thread>
#include <mutex>
#include <vector>
#include <condition_variable>

#include <ros/message_forward.h>
#include "xcroscpp_common.h"
#include "ros/console.h"

namespace rosgraph_msgs
{
ROS_DECLARE_MESSAGE(Log);
}

namespace xcros
{

class Publication;
typedef std::shared_ptr<Publication> PublicationPtr;
typedef std::weak_ptr<Publication> PublicationWPtr;

class ROSCPP_DECL ROSOutAppender : public xcros::console::LogAppender
{
public:
  ROSOutAppender();
  ~ROSOutAppender();

  const std::string& getLastError() const;

  virtual void log(::xcros::console::Level level, const char* str, const char* file, const char* function, int line);

protected:
  void logThread();

  std::string last_error_;

  typedef std::vector<rosgraph_msgs::LogPtr> V_Log;
  V_Log log_queue_;
  std::mutex queue_mutex_;
  std::condition_variable queue_condition_;
  bool shutting_down_;

  std::thread publish_thread_;
};

} // namespace ros

#endif