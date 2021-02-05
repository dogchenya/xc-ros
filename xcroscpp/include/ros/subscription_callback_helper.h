#ifndef _XCROSCPP_SUBSCRIPTION_CALLBACK_HELPER_H_
#define _XCROSCPP_SUBSCRIPTION_CALLBACK_HELPER_H_

#include <typeinfo>

#include "ros/xcroscpp_common.h"
#include "ros/forwards.h"
#include "ros/parameter_adapter.h"
#include "ros/message_traits.h"
#include "ros/builtin_message_traits.h"
#include "ros/serialization.h"
#include "ros/message_event.h"
#include <ros/static_assert.h>

#include <boost/type_traits/add_const.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/make_shared.hpp>

namespace xcros
{

struct SubscriptionCallbackHelperDeserializeParams
{
  uint8_t* buffer;
  uint32_t length;
  boost::shared_ptr<M_string> connection_header;
};

struct ROSCPP_DECL SubscriptionCallbackHelperCallParams
{
  MessageEvent<void const> event;
};

/**
 * \brief Abstract base class used by subscriptions to deal with concrete message types through a common
 * interface.  This is one part of the roscpp API that is \b not fully stable, so overloading this class
 * is not recommended.
 */
class ROSCPP_DECL SubscriptionCallbackHelper
{
public:
  virtual ~SubscriptionCallbackHelper() {}
  virtual VoidConstPtr deserialize(const SubscriptionCallbackHelperDeserializeParams&) = 0;
  virtual void call(SubscriptionCallbackHelperCallParams& params) = 0;
  virtual const std::type_info& getTypeInfo() = 0;
  virtual bool isConst() = 0;
  virtual bool hasHeader() = 0;
};
typedef boost::shared_ptr<SubscriptionCallbackHelper> SubscriptionCallbackHelperPtr;

/**
 * \brief Concrete generic implementation of
 * SubscriptionCallbackHelper for any normal message type.  Use
 * directly with care, this is mostly for internal use.
 */
template<typename P, typename Enabled = void>
class SubscriptionCallbackHelperT : public SubscriptionCallbackHelper
{
public:
  typedef ParameterAdapter<P> Adapter;
  typedef typename ParameterAdapter<P>::Message NonConstType;
  typedef typename ParameterAdapter<P>::Event Event;
  typedef typename boost::add_const<NonConstType>::type ConstType;
  typedef boost::shared_ptr<NonConstType> NonConstTypePtr;
  typedef boost::shared_ptr<ConstType> ConstTypePtr;

  static const bool is_const = ParameterAdapter<P>::is_const;

  typedef boost::function<void(typename Adapter::Parameter)> Callback;
  typedef boost::function<NonConstTypePtr()> CreateFunction;

  SubscriptionCallbackHelperT(const Callback& callback, 
			      const CreateFunction& create = DefaultMessageCreator<NonConstType>())
    : callback_(callback)
    , create_(create)
  { }

  void setCreateFunction(const CreateFunction& create)
  {
    create_ = create;
  }

  virtual bool hasHeader()
  {
     return message_traits::hasHeader<typename ParameterAdapter<P>::Message>();
  }

  virtual VoidConstPtr deserialize(const SubscriptionCallbackHelperDeserializeParams& params)
  {
    namespace ser = serialization;

    NonConstTypePtr msg = create_();

    if (!msg)
    {
      ROS_DEBUG("Allocation failed for message of type [%s]", getTypeInfo().name());
      return VoidConstPtr();
    }

    ser::PreDeserializeParams<NonConstType> predes_params;
    predes_params.message = msg;
    predes_params.connection_header = params.connection_header;
    ser::PreDeserialize<NonConstType>::notify(predes_params);

    ser::IStream stream(params.buffer, params.length);
    ser::deserialize(stream, *msg);

    return VoidConstPtr(msg);
  }

  virtual void call(SubscriptionCallbackHelperCallParams& params)
  {
    Event event(params.event, create_);
    callback_(ParameterAdapter<P>::getParameter(event));
  }

  virtual const std::type_info& getTypeInfo()
  {
    return typeid(NonConstType);
  }

  virtual bool isConst()
  {
    return ParameterAdapter<P>::is_const;
  }

private:
  Callback callback_;
  CreateFunction create_;
};
    
} // namespace xcros



#endif