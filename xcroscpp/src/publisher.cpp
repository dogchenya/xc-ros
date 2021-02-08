#include "ros/publisher.h"
#include "ros/publication.h"
#include "ros/node_handle.h"
#include "ros/topic_manager.h"


namespace xcros
{

Publisher::Impl::Impl() : unadvertised_(false) { }

Publisher::Impl::~Impl()
{
  ROS_DEBUG("Publisher on '%s' deregistering callbacks.", topic_.c_str());
  unadvertise();
}

bool Publisher::Impl::isValid() const
{
  return !unadvertised_;
}

void Publisher::Impl::unadvertise()
{
  if (!unadvertised_)
  {
    unadvertised_ = true;
    TopicManager::instance()->unadvertise(topic_, callbacks_);
    node_handle_.reset();
  }
}

Publisher::Publisher(const std::string& topic, const std::string& md5sum, const std::string& datatype, const NodeHandle& node_handle, const SubscriberCallbacksPtr& callbacks)
: impl_(boost::make_shared<Impl>())
{
  impl_->topic_ = topic;
  impl_->md5sum_ = md5sum;
  impl_->datatype_ = datatype;
  impl_->node_handle_ = boost::make_shared<NodeHandle>(node_handle);
  impl_->callbacks_ = callbacks;
}

Publisher::Publisher(const Publisher& rhs)
{
  impl_ = rhs.impl_;
}

Publisher::~Publisher()
{
}

void Publisher::publish(const boost::function<SerializedMessage(void)>& serfunc, SerializedMessage& m) const
{
  if (!impl_)
  {
    ROS_ASSERT_MSG(false, "Call to publish() on an invalid Publisher (topic [%s])", impl_->topic_.c_str());
    return;
  }

  if (!impl_->isValid())
  {
    ROS_ASSERT_MSG(false, "Call to publish() on an invalid Publisher (topic [%s])", impl_->topic_.c_str());
    return;
  }

  TopicManager::instance()->publish(impl_->topic_, serfunc, m);
}

void Publisher::incrementSequence() const
{
  if (impl_ && impl_->isValid())
  {
    TopicManager::instance()->incrementSequence(impl_->topic_);
  }
}

void Publisher::shutdown()
{
  if (impl_)
  {
    impl_->unadvertise();
    impl_.reset();
  }
}

std::string Publisher::getTopic() const
{
  if (impl_)
  {
    return impl_->topic_;
  }

  return std::string();
}

uint32_t Publisher::getNumSubscribers() const
{
  if (impl_ && impl_->isValid())
  {
    return TopicManager::instance()->getNumSubscribers(impl_->topic_);
  }

  return 0;
}

bool Publisher::isLatched() const {
  PublicationPtr publication_ptr;
  if (impl_ && impl_->isValid()) {
    publication_ptr =
      TopicManager::instance()->lookupPublication(impl_->topic_);
  } else {
    ROS_ASSERT_MSG(false, "Call to isLatched() on an invalid Publisher");
    throw xcros::Exception("Call to isLatched() on an invalid Publisher");
  }
  if (publication_ptr) {
    return publication_ptr->isLatched();
  } else {
    ROS_ASSERT_MSG(false, "Call to isLatched() on an invalid Publisher");
    throw xcros::Exception("Call to isLatched() on an invalid Publisher");
  }
}

} // namespace ros