#ifndef _XCROSCPP_STATISTICS_H_
#define _XCROSCPP_STATISTICS_H_

#include "forwards.h"
#include "poll_set.h"
#include "ros/xcroscpp_common.h"
#include "publisher.h"
#include <ros/time.h>
#include "ros/subscription_callback_helper.h"
#include <map>

namespace xcros
{

/**
 * \brief This class logs statistics data about a ROS connection and
 * publishs them periodically on a common topic.
 *
 * It provides a callback() function that has to be called everytime
 * a new message arrives on a topic.
 */
class ROSCPP_DECL StatisticsLogger
{
public:

  /**
   * Constructior
   */
  StatisticsLogger();

  /**
   * Actual initialization. Must be called before the first call to callback()
   */
  void init(const SubscriptionCallbackHelperPtr& helper);

  /**
   * Callback function. Must be called for every message received.
   */
  void callback(const boost::shared_ptr<M_string>& connection_header, const std::string& topic, const std::string& callerid, const SerializedMessage& m, const uint64_t& bytes_sent, const xcros::Time& received_time, bool dropped);

private:

  // these are hard constrains
  int max_window;
  int min_window;

  // these are soft constrains
  int max_elements;
  int min_elements;

  bool enable_statistics;

  // remember, if this message type has a header
  bool hasHeader_;

  // frequency to publish statistics
  double pub_frequency_;

  // publisher for statistics data
  xcros::Publisher pub_;

  struct StatData {
    // last time, we published /statistics data
    xcros::Time last_publish;
    // arrival times of all messages within the current window
    std::list<xcros::Time> arrival_time_list;
    // age of all messages within the current window (if available)
    std::list<xcros::Duration> age_list;
    // number of dropped messages
    uint64_t dropped_msgs;
    // latest sequence number observered (if available)
    uint64_t last_seq;
    // latest total traffic volume observed
    uint64_t stat_bytes_last;
  };

  // storage for statistics data
  std::map<std::string, struct StatData> map_;
};

}

#endif