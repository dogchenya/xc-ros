// #include "rosout_appender.h"
// #include "this_node.h"
// #include "node_handle.h"
// #include "topic_manager.h"
// #include "advertise_options.h"
// #include "names.h"

// //#include <rosgraph_msgs/Log.h>

// namespace xcros
// {

// ROSOutAppender::ROSOutAppender()
// : shutting_down_(false)
// , publish_thread_(boost::bind(&ROSOutAppender::logThread, this))
// {
//   AdvertiseOptions ops;
//   ops.init<rosgraph_msgs::Log>(names::resolve("/rosout"), 0);
//   ops.latch = true;
//   SubscriberCallbacksPtr cbs(boost::make_shared<SubscriberCallbacks>());
//   TopicManager::instance()->advertise(ops, cbs);
// }

// ROSOutAppender::~ROSOutAppender()
// {
//   shutting_down_ = true;

//   {
//     boost::mutex::scoped_lock lock(queue_mutex_);
//     queue_condition_.notify_all();
//   }

//   publish_thread_.join();
// }

// const std::string&  ROSOutAppender::getLastError() const
// {
//   return last_error_;
// }

// void ROSOutAppender::log(::ros::console::Level level, const char* str, const char* file, const char* function, int line)
// {
//   rosgraph_msgs::LogPtr msg(boost::make_shared<rosgraph_msgs::Log>());

//   msg->header.stamp = ros::Time::now();
//   if (level == ros::console::levels::Debug)
//   {
//     msg->level = rosgraph_msgs::Log::DEBUG;
//   }
//   else if (level == ros::console::levels::Info)
//   {
//     msg->level = rosgraph_msgs::Log::INFO;
//   }
//   else if (level == ros::console::levels::Warn)
//   {
//     msg->level = rosgraph_msgs::Log::WARN;
//   }
//   else if (level == ros::console::levels::Error)
//   {
//     msg->level = rosgraph_msgs::Log::ERROR;
//   }
//   else if (level == ros::console::levels::Fatal)
//   {
//     msg->level = rosgraph_msgs::Log::FATAL;
//   }
//   msg->name = this_node::getName();
//   msg->msg = str;
//   msg->file = file;
//   msg->function = function;
//   msg->line = line;
//   this_node::getAdvertisedTopics(msg->topics);

//   if (level == ::ros::console::levels::Fatal || level == ::ros::console::levels::Error)
//   {
//     last_error_ = str;
//   }

//   boost::mutex::scoped_lock lock(queue_mutex_);
//   log_queue_.push_back(msg);
//   queue_condition_.notify_all();
// }

// void ROSOutAppender::logThread()
// {
//   while (!shutting_down_)
//   {
//     V_Log local_queue;

//     {
//       boost::mutex::scoped_lock lock(queue_mutex_);

//       if (shutting_down_)
//       {
//         return;
//       }

//       queue_condition_.wait(lock);

//       if (shutting_down_)
//       {
//         return;
//       }

//       local_queue.swap(log_queue_);
//     }

//     V_Log::iterator it = local_queue.begin();
//     V_Log::iterator end = local_queue.end();
//     for (; it != end; ++it)
//     {
//       TopicManager::instance()->publish(names::resolve("/rosout"), *(*it));
//     }
//   }
// }

// } // namespace xcros
