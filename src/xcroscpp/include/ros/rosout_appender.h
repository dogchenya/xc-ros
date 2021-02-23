#ifndef _XCROSCPP_ROSOUT_APPENDER_H_
#define _XCROSCPP_ROSOUT_APPENDER_H_

#include <memory>
#include <thread>
#include <mutex>
#include <vector>
#include <condition_variable>

#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

#include <ros/message_forward.h>
#include "ros/xcroscpp_common.h"
#include "ros/console.h"
#include "ros/forwards.h"

namespace rosgraph_msgs
{
ROS_DECLARE_MESSAGE(Log);
}

namespace xcros
{

//class Publication;
typedef boost::shared_ptr<Publication> PublicationPtr;
typedef boost::weak_ptr<Publication> PublicationWPtr;

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
  boost::mutex queue_mutex_;
  boost::condition_variable queue_condition_;
  bool shutting_down_;

  boost::thread publish_thread_;
};

} // namespace xcros

#endif