#include "ros/statistics.h"
#include "ros/node_handle.h"
#include <rosgraph_msgs/TopicStatistics.h>
#include "ros/this_node.h"
#include "ros/message_traits.h"
#include "std_msgs/Header.h"
#include "ros/param.h"


namespace xcros
{

StatisticsLogger::StatisticsLogger()
: pub_frequency_(1.0)
{
}

void StatisticsLogger::init(const SubscriptionCallbackHelperPtr& helper) {
  hasHeader_ = helper->hasHeader();
  param::param("/enable_statistics", enable_statistics, false);
  param::param("/statistics_window_min_elements", min_elements, 10);
  param::param("/statistics_window_max_elements", max_elements, 100);
  param::param("/statistics_window_min_size", min_window, 4);
  param::param("/statistics_window_max_size", max_window, 64);
}

void StatisticsLogger::callback(const boost::shared_ptr<M_string>& connection_header,
                                const std::string& topic, const std::string& callerid, const SerializedMessage& m, const uint64_t& bytes_sent,
                                const xcros::Time& received_time, bool dropped)
{
  (void)connection_header;
  struct StatData stats;

  if (!enable_statistics)
  {
    return;
  }

  // ignore /clock for safety and /statistics to reduce noise
  if (topic == "/statistics" || topic == "/clock")
  {
    return;
  }

  // callerid identifies the connection
  std::map<std::string, struct StatData>::iterator stats_it = map_.find(callerid);
  if (stats_it == map_.end())
  {
    // this is the first time, we received something on this connection
    stats.stat_bytes_last = 0;
    stats.dropped_msgs = 0;
    stats.last_seq = 0;
    stats.last_publish = xcros::Time::now();
    map_[callerid] = stats;
  }
  else
  {
    stats = map_[callerid];
  }

  stats.arrival_time_list.push_back(received_time);

  if (dropped)
  {
    stats.dropped_msgs++;
  }

  // try to extract header, if the message has one. this fails sometimes,
  // therefore the try-catch
  if (hasHeader_)
  {
    try
    {
      std_msgs::Header header;
      xcros::serialization::IStream stream(m.message_start, m.num_bytes - (m.message_start - m.buf.get()));
      xcros::serialization::deserialize(stream, header);
      if (!header.stamp.isZero())
      {
        stats.age_list.push_back(received_time - header.stamp);
      }
    }
    catch (xcros::serialization::StreamOverrunException& e)
    {
      ROS_DEBUG("Error during header extraction for statistics (topic=%s, message_length=%li)", topic.c_str(), m.num_bytes - (m.message_start - m.buf.get()));
      hasHeader_ = false;
    }
  }

  // should publish new statistics?
  if (stats.last_publish + xcros::Duration(pub_frequency_) < received_time)
  {
    xcros::Time window_start = stats.last_publish;
    stats.last_publish = received_time;

    // fill the message with the aggregated data
    rosgraph_msgs::TopicStatistics msg;
    msg.topic = topic;
    msg.node_pub = callerid;
    msg.node_sub = xcros::this_node::getName();
    msg.window_start = window_start;
    msg.window_stop = received_time;
    msg.delivered_msgs = stats.arrival_time_list.size();
    msg.dropped_msgs = stats.dropped_msgs;
    msg.traffic = bytes_sent - stats.stat_bytes_last;

    // not all message types have this
    if (stats.age_list.size() > 0)
    {
      msg.stamp_age_mean = xcros::Duration(0);
      msg.stamp_age_max = xcros::Duration(0);

      for(std::list<xcros::Duration>::iterator it = stats.age_list.begin(); it != stats.age_list.end(); it++)
      {
        xcros::Duration age = *it;
        msg.stamp_age_mean += age;

        if (age > msg.stamp_age_max)
        {
            msg.stamp_age_max = age;
        }
      }

      msg.stamp_age_mean *= 1.0 / stats.age_list.size();

      double stamp_age_variance = 0.0;
      for(std::list<xcros::Duration>::iterator it = stats.age_list.begin(); it != stats.age_list.end(); it++)
      {
        xcros::Duration t = msg.stamp_age_mean - *it;
        stamp_age_variance += t.toSec() * t.toSec();
      }
      double stamp_age_stddev = sqrt(stamp_age_variance / stats.age_list.size());
      try
      {
        msg.stamp_age_stddev = xcros::Duration(stamp_age_stddev);
      }
      catch(std::runtime_error& e)
      {
        msg.stamp_age_stddev = xcros::Duration(0);
        ROS_WARN_STREAM("Error updating stamp_age_stddev for topic [" << topic << "]"
          << " from node [" << callerid << "],"
          << " likely due to the time between the mean stamp age and this message being exceptionally large."
          << " Exception was: " << e.what());
        ROS_DEBUG_STREAM("Mean stamp age was: " << msg.stamp_age_mean << " - std_dev of: " << stamp_age_stddev);
      }

    }
    else
    {
      // in that case, set to NaN
      msg.stamp_age_mean = xcros::Duration(0);
      msg.stamp_age_stddev = xcros::Duration(0);
      msg.stamp_age_max = xcros::Duration(0);
    }

    // first, calculate the mean period between messages in this connection
    // we need at least two messages in the window for this.
    if (stats.arrival_time_list.size() > 1)
    {
      msg.period_mean = xcros::Duration(0);
      msg.period_max = xcros::Duration(0);

      xcros::Time prev;
      for(std::list<xcros::Time>::iterator it = stats.arrival_time_list.begin(); it != stats.arrival_time_list.end(); it++) {

        if (it == stats.arrival_time_list.begin()) {
          prev = *it;
          continue;
        }

        xcros::Duration period = *it - prev;
        msg.period_mean += period;
        if (period > msg.period_max)
            msg.period_max = period;
        prev = *it;
      }
      msg.period_mean *= 1.0 / (stats.arrival_time_list.size() - 1);

      // then, calc the stddev
      double period_variance = 0.0;
      for(std::list<xcros::Time>::iterator it = stats.arrival_time_list.begin(); it != stats.arrival_time_list.end(); it++)
      {
        if (it == stats.arrival_time_list.begin())
        {
          prev = *it;
          continue;
        }
        xcros::Duration period = *it - prev;
        xcros::Duration t = msg.period_mean - period;
        period_variance += t.toSec() * t.toSec();
        prev = *it;
      }
      double period_stddev = sqrt(period_variance / (stats.arrival_time_list.size() - 1));
      msg.period_stddev = xcros::Duration(period_stddev);
    }
    else
    {
      // in that case, set to NaN
      msg.period_mean = xcros::Duration(0);
      msg.period_stddev = xcros::Duration(0);
      msg.period_max = xcros::Duration(0);
    }

    if (!pub_.getTopic().length())
    {
      xcros::NodeHandle n("~");
      // creating the publisher in the constructor results in a deadlock. so do it here.
      pub_ = n.advertise<rosgraph_msgs::TopicStatistics>("/statistics", 1);
    }

    pub_.publish(msg);

    // dynamic window resizing
    if (stats.arrival_time_list.size() > static_cast<size_t>(max_elements) && pub_frequency_ * 2 <= max_window)
    {
      pub_frequency_ *= 2;
    }
    if (stats.arrival_time_list.size() < static_cast<size_t>(min_elements) && pub_frequency_ / 2 >= min_window)
    {
      pub_frequency_ /= 2;
    }

    // clear the window
    stats.age_list.clear();
    stats.arrival_time_list.clear();
    stats.dropped_msgs = 0;
    stats.stat_bytes_last = bytes_sent;

  }
  // store the stats for this connection
  map_[callerid] = stats;
}


} // namespace xcros