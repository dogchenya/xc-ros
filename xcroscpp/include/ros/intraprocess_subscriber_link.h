#ifndef _XCROSCPP_INTRAPROCESS_SUBSCRIBER_LINK_H_
#define _XCROSCPP_INTRAPROCESS_SUBSCRIBER_LINK_H_


#include "subscriber_link.h"
#include "ros/xcroscpp_common.h"

#include <boost/thread/recursive_mutex.hpp>

namespace xcros
{

class IntraProcessPublisherLink;
typedef boost::shared_ptr<IntraProcessPublisherLink> IntraProcessPublisherLinkPtr;

/**
 * \brief SubscriberLink handles broadcasting messages to a single subscriber on a single topic
 */
class ROSCPP_DECL IntraProcessSubscriberLink : public SubscriberLink
{
public:
  IntraProcessSubscriberLink(const PublicationPtr& parent);
  virtual ~IntraProcessSubscriberLink();

  void setSubscriber(const IntraProcessPublisherLinkPtr& subscriber);
  bool isLatching();

  virtual void enqueueMessage(const SerializedMessage& m, bool ser, bool nocopy);
  virtual void drop();
  virtual std::string getTransportType();
  virtual std::string getTransportInfo();
  virtual bool isIntraprocess() { return true; }
  virtual void getPublishTypes(bool& ser, bool& nocopy, const std::type_info& ti);

private:
  IntraProcessPublisherLinkPtr subscriber_;
  bool dropped_;
  boost::recursive_mutex drop_mutex_;
};
typedef boost::shared_ptr<IntraProcessSubscriberLink> IntraProcessSubscriberLinkPtr;

} // namespace xcros

#endif // _XCROSCPP_INTRAPROCESS_SUBSCRIBER_LINK_H_