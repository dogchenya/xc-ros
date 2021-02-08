#ifndef _XCROSCPP_INTRAPROCESS_PUBLISHER_LINK_H_
#define _XCROSCPP_INTRAPROCESS_PUBLISHER_LINK_H_

#include "ros/publisher_link.h"
#include "ros/xcroscpp_common.h"

#include <boost/thread/recursive_mutex.hpp>

namespace xcros
{
class Subscription;
typedef boost::shared_ptr<Subscription> SubscriptionPtr;
typedef boost::weak_ptr<Subscription> SubscriptionWPtr;

class IntraProcessSubscriberLink;
typedef boost::shared_ptr<IntraProcessSubscriberLink> IntraProcessSubscriberLinkPtr;

/**
 * \brief Handles a connection to a single publisher on a given topic.  Receives messages from a publisher
 * and hands them off to its parent Subscription
 */
class ROSCPP_DECL IntraProcessPublisherLink : public PublisherLink
{
public:
  IntraProcessPublisherLink(const SubscriptionPtr& parent, const std::string& xmlrpc_uri, const TransportHints& transport_hints);
  virtual ~IntraProcessPublisherLink();

  void setPublisher(const IntraProcessSubscriberLinkPtr& publisher);

  virtual std::string getTransportType();
  virtual std::string getTransportInfo();
  virtual void drop();

  /**
   * \brief Handles handing off a received message to the subscription, where it will be deserialized and called back
   */
  virtual void handleMessage(const SerializedMessage& m, bool ser, bool nocopy);

  void getPublishTypes(bool& ser, bool& nocopy, const std::type_info& ti);

private:
  IntraProcessSubscriberLinkPtr publisher_;
  bool dropped_;
  boost::recursive_mutex drop_mutex_;
};
typedef boost::shared_ptr<IntraProcessPublisherLink> IntraProcessPublisherLinkPtr;

} // namespace ros

#endif // _XCROSCPP_INTRAPROCESS_PUBLISHER_LINK_H_