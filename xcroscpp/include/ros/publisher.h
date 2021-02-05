#ifndef _XCROSCPP_PUBLISHER_H_
#define _XCROSCPP_PUBLISHER_H_

#include "ros/forwards.h"
#include "ros/xcroscpp_common.h"
#include "ros/message.h"
#include "ros/serialization.h"
#include <boost/bind.hpp>


namespace xcros
{

class ROSCPP_DECL Publisher
{
public:
    Publisher(){}
    Publisher(const Publisher& rhs);
    ~Publisher();

    template<typename M>
    void publish(const boost::shared_ptr<M>& message)
    {
        using namespace serialization;

        if(!impl_)
        {
            ROS_ASSERT_MSG(false, "Call to publish() on an invalid Publisher");
            return;
        }

        if(!impl_->isValid())
        {
            ROS_ASSERT_MSG(false, "Call to publish() on an invalid Publisher (topic [%s])", impl_->topic_.c_str());
            return;
        }

        ROS_ASSERT_MSG(impl_->md5sum_ == "*" || std::string(mt::md5sum<M>(*message)) == "*" || impl_->md5sum_ == mt::md5sum<M>(*message),
                     "Trying to publish message of type [%s/%s] on a publisher with type [%s/%s]",
                     mt::datatype<M>(*message), mt::md5sum<M>(*message),
                     impl_->datatype_.c_str(), impl_->md5sum_.c_str());

        SerializedMessage m;
        m.type_info = &typeid(M);
        m.message = message;

        publish(boost::bind(serializeMessage<M>, boost::ref(*message)), m);
    }

    template <typename M>
    void publish(const M& message) const
    {
      using namespace serialization;
      namespace mt = ros::message_traits;

      if (!impl_)
        {
          ROS_ASSERT_MSG(false, "Call to publish() on an invalid Publisher");
          return;
        }

      if (!impl_->isValid())
        {
          ROS_ASSERT_MSG(false, "Call to publish() on an invalid Publisher (topic [%s])", impl_->topic_.c_str());
          return;
        }

      ROS_ASSERT_MSG(impl_->md5sum_ == "*" || std::string(mt::md5sum<M>(message)) == "*" || impl_->md5sum_ == mt::md5sum<M>(message),
                     "Trying to publish message of type [%s/%s] on a publisher with type [%s/%s]",
                     mt::datatype<M>(message), mt::md5sum<M>(message),
                     impl_->datatype_.c_str(), impl_->md5sum_.c_str());

      SerializedMessage m;
      publish(boost::bind(serializeMessage<M>, boost::ref(message)), m);
    }

    void shutdown();

    /**
     * \brief Returns the topic that this Publisher will publish on.
     */
    std::string getTopic() const;

    /**
     * \brief Returns the number of subscribers that are currently connected to this Publisher
     */
    uint32_t getNumSubscribers() const;

    /**
     * \brief Returns whether or not this topic is latched
     */
    bool isLatched() const;

    operator void*() const { return (impl_ && impl_->isValid()) ? (void*)1 : (void*)0; }

    bool operator<(const Publisher& rhs) const
    {
      return impl_ < rhs.impl_;
    }

    bool operator==(const Publisher& rhs) const
    {
      return impl_ == rhs.impl_;
    }

    bool operator!=(const Publisher& rhs) const
    {
      return impl_ != rhs.impl_;
    }

private:
    Publisher(const std::string& topic, const std::string& md5sum, 
              const std::string& datatype, const NodeHandle& node_handle, 
              const SubscriberCallbacksPtr& callbacks);

    
    void publish(const boost::function<SerializedMessage(void)>& serfunc, SerializedMessage& m) const;
    void incrementSequence() const;

    class ROSCPP_DECL Impl
    {
    public:
      Impl();
      ~Impl();

      void unadvertise();
      bool isValid() const;

      std::string topic_;
      std::string md5sum_;
      std::string datatype_;
      NodeHandlePtr node_handle_;
      SubscriberCallbacksPtr callbacks_;
      bool unadvertised_;
    };

    typedef boost::shared_ptr<Impl> ImplPtr;
    typedef boost::weak_ptr<Impl> ImplWPtr;

    ImplPtr impl_;

    friend class NodeHandle;
    friend class NodeHandleBackingCollection;
};

    typedef std::vector<Publisher> V_Publisher;
} // namespace xcros


#endif