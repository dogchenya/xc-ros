#ifndef ROSCPP_SINGLE_SUBSCRIBER_PUBLISHER_H
#define ROSCPP_SINGLE_SUBSCRIBER_PUBLISHER_H

#include "ros/forwards.h"
#include "ros/serialization.h"
#include "ros/xcroscpp_common.h"

#include <boost/utility.hpp>

namespace xcros
{

/**
 * \brief Allows publication of a message to a single subscriber. Only available inside subscriber connection callbacks
 */
class ROSCPP_DECL SingleSubscriberPublisher : public boost::noncopyable
{
public:
  SingleSubscriberPublisher(const SubscriberLinkPtr& link);
  ~SingleSubscriberPublisher();

  /**
   * \brief Publish a message on the topic associated with this Publisher.
   *
   * This version of publish will allow fast intra-process message-passing in the future,
   * so you may not mutate the message after it has been passed in here (since it will be
   * passed directly into a callback function)
   *
   */
  template<class M>
  void publish(const boost::shared_ptr<M const>& message) const
  {
    publish(*message);
  }

  /**
   * \brief Publish a message on the topic associated with this Publisher.
   *
   * This version of publish will allow fast intra-process message-passing in the future,
   * so you may not mutate the message after it has been passed in here (since it will be
   * passed directly into a callback function)
   *
   */
  template<class M>
  void publish(const boost::shared_ptr<M>& message) const
  {
    publish(*message);
  }

  /**
   * \brief Publish a message on the topic associated with this Publisher.
   */
  template<class M>
  void publish(const M& message) const
  {
    using namespace serialization;
    SerializedMessage m = serializeMessage(message);
    publish(m);
  }

  /**
   * \brief Returns the topic this publisher publishes on
   */
  std::string getTopic() const;

  /**
   * \brief Returns the name of the subscriber node
   */
  std::string getSubscriberName() const;

private:
  void publish(const SerializedMessage& m) const;

  SubscriberLinkPtr link_;
};

}

#endif // ROSCPP_PUBLISHER_HANDLE_H