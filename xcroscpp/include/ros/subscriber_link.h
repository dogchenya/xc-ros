#ifndef _XCROSCPP_SUBSCRIBER_LINK_H_
#define _XCROSCPP_SUBSCRIBER_LINK_H_

#include "ros/xcroscpp_common.h"

#include "boost/shared_ptr.hpp"
#include <boost/thread/mutex.hpp>
#include <boost/shared_array.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "ros/serialized_message.h"
#include "ros/forwards.h"

namespace xcros
{

class Header;
class Message;
class Publication;
typedef boost::shared_ptr<Publication> PublicationPtr;
typedef boost::weak_ptr<Publication> PublicationWPtr;
// class Connection;
// typedef boost::shared_ptr<Connection> ConnectionPtr;

class ROSCPP_DECL SubscriberLink : public boost::enable_shared_from_this<SubscriberLink>
{

public:
  class Stats
  {
  public:
    uint64_t bytes_sent_, message_data_sent_, messages_sent_;
    Stats()
    : bytes_sent_(0), message_data_sent_(0), messages_sent_(0) { }
  };

  SubscriberLink();
  virtual ~SubscriberLink();

  const std::string& getTopic() const { return topic_; }
  const Stats &getStats() { return stats_; }
  const std::string &getDestinationCallerID() const { return destination_caller_id_; }
  int getConnectionID() const { return connection_id_; }

  /**
   * \brief Queue up a message for publication.  Throws out old messages if we've reached our Publication's max queue size
   */
  virtual void enqueueMessage(const SerializedMessage& m, bool ser, bool nocopy) = 0;

  virtual void drop() = 0;

  virtual std::string getTransportType() = 0;
  virtual std::string getTransportInfo() = 0;

  virtual bool isIntraprocess() { return false; }
  virtual void getPublishTypes(bool& ser, bool& nocopy, const std::type_info& ti) { (void)ti; ser = true; nocopy = false; }

  const std::string& getMD5Sum();
  const std::string& getDataType();
  const std::string& getMessageDefinition();

protected:
  bool verifyDatatype(const std::string &datatype);

  PublicationWPtr parent_;
  unsigned int connection_id_;
  std::string destination_caller_id_;
  Stats stats_;
  std::string topic_;
};

}

#endif