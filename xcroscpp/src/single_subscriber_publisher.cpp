#include "ros/single_subscriber_publisher.h"
#include "ros/subscriber_link.h"

namespace xcros
{

SingleSubscriberPublisher::SingleSubscriberPublisher(const SubscriberLinkPtr& link)
: link_(link)
{
}

SingleSubscriberPublisher::~SingleSubscriberPublisher()
{
}

void SingleSubscriberPublisher::publish(const SerializedMessage& m) const
{
  link_->enqueueMessage(m, true, true);
}

std::string SingleSubscriberPublisher::getTopic() const
{
  return link_->getTopic();
}

std::string SingleSubscriberPublisher::getSubscriberName() const
{
  return link_->getDestinationCallerID();
}

} // namespace xcros
