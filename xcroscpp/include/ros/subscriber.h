#ifndef _XCROSCPP_SUBSCRIBER_HANDLE_H_
#define _XCROSCPP_SUBSCRIBER_HANDLE_H_

#include "ros/xcroscpp_common.h"
#include "ros/forwards.h"
#include "ros/subscription_callback_helper.h"

namespace xcros
{

/**
 * \brief Manages an subscription callback on a specific topic.
 *
 * A Subscriber should always be created through a call to NodeHandle::subscribe(), or copied from one
 * that was. Once all copies of a specific
 * Subscriber go out of scope, the subscription callback associated with that handle will stop
 * being called.  Once all Subscriber for a given topic go out of scope the topic will be unsubscribed.
 */
class ROSCPP_DECL Subscriber
{
public:
  Subscriber() {}
  Subscriber(const Subscriber& rhs);
  ~Subscriber();

  /**
   * \brief Unsubscribe the callback associated with this Subscriber
   *
   * This method usually does not need to be explicitly called, as automatic shutdown happens when
   * all copies of this Subscriber go out of scope
   *
   * This method overrides the automatic reference counted unsubscribe, and immediately
   * unsubscribes the callback associated with this Subscriber
   */
  void shutdown();

  std::string getTopic() const;

  /**
   * \brief Returns the number of publishers this subscriber is connected to
   */
  uint32_t getNumPublishers() const;

  operator void*() const { return (impl_ && impl_->isValid()) ? (void*)1 : (void*)0; }

  bool operator<(const Subscriber& rhs) const
  {
    return impl_ < rhs.impl_;
  }

  bool operator==(const Subscriber& rhs) const
  {
    return impl_ == rhs.impl_;
  }

  bool operator!=(const Subscriber& rhs) const
  {
    return impl_ != rhs.impl_;
  }

private:

  Subscriber(const std::string& topic, const NodeHandle& node_handle, 
	     const SubscriptionCallbackHelperPtr& helper);

  class Impl
  {
  public:
    Impl();
    ~Impl();

    void unsubscribe();
    bool isValid() const;

    std::string topic_;
    NodeHandlePtr node_handle_;
    SubscriptionCallbackHelperPtr helper_;
    bool unsubscribed_;
  };
  typedef boost::shared_ptr<Impl> ImplPtr;
  typedef boost::weak_ptr<Impl> ImplWPtr;

  ImplPtr impl_;

  friend class NodeHandle;
  friend class NodeHandleBackingCollection;
};
typedef std::vector<Subscriber> V_Subscriber;

}

#endif // ROSCPP_PUBLISHER_HANDLE_H