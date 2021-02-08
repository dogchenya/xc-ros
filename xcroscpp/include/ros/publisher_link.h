#ifndef _XCROSCPP_PUBLISHER_LINK_H_
#define _XCROSCPP_PUBLISHER_LINK_H_

#include "ros/xcroscpp_common.h"
#include "ros/transport_hints.h"
#include "ros/header.h"
#include "ros/serialized_message.h"

#include <boost/thread/mutex.hpp>
#include <boost/shared_array.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <queue>

namespace xcros
{
class Header;
class Message;
class Subscription;
typedef boost::shared_ptr<Subscription> SubscriptionPtr;
typedef boost::weak_ptr<Subscription> SubscriptionWPtr;
class Connection;
typedef boost::shared_ptr<Connection> ConnectionPtr;

/**
 * \brief Handles a connection to a single publisher on a given topic.  Receives messages from a publisher
 * and hands them off to its parent Subscription
 */
class ROSCPP_DECL PublisherLink : public boost::enable_shared_from_this<PublisherLink>
{
public:
  class Stats
  {
  public:
    uint64_t bytes_received_, messages_received_, drops_;
    Stats()
    : bytes_received_(0), messages_received_(0), drops_(0) { }
  };


  PublisherLink(const SubscriptionPtr& parent, const std::string& xmlrpc_uri, const TransportHints& transport_hints);
  virtual ~PublisherLink();

  const Stats &getStats() { return stats_; }
  const std::string& getPublisherXMLRPCURI();
  int getConnectionID() const { return connection_id_; }
  const std::string& getCallerID() { return caller_id_; }
  bool isLatched() { return latched_; }

  bool setHeader(const Header& header);

  /**
   * \brief Handles handing off a received message to the subscription, where it will be deserialized and called back
   */
  virtual void handleMessage(const SerializedMessage& m, bool ser, bool nocopy) = 0;
  virtual std::string getTransportType() = 0;
  virtual std::string getTransportInfo() = 0;
  virtual void drop() = 0;

  const std::string& getMD5Sum();

protected:
  SubscriptionWPtr parent_;
  unsigned int connection_id_;
  std::string publisher_xmlrpc_uri_;

  Stats stats_;

  TransportHints transport_hints_;

  bool latched_;
  std::string caller_id_;
  Header header_;
  std::string md5sum_;
};

} // namespace ros

#endif // _XCROSCPP_PUBLISHER_LINK_H_