#ifndef _XCROSCPP_PARAMETER_ADAPTER_H_
#define _XCROSCPP_PARAMETER_ADAPTER_H_

#include "ros/forwards.h"
#include "ros/message_event.h"
#include <ros/static_assert.h>

#include <boost/type_traits/add_const.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/type_traits/remove_reference.hpp>

namespace xcros
{
/**
 * \brief Generally not for outside use.  Adapts a function parameter type into the message type, event type and parameter.  Allows you to
 * retrieve a parameter type from an event type.
 *
 * ParameterAdapter is generally only useful for outside use when implementing things that require message callbacks
 * (such as the message_filters package)and you would like to support all the roscpp message parameter types
 *
 * The ParameterAdapter is templated on the callback parameter type (\b not the bare message type), and provides 3 things:
 *  - Message typedef, which provides the bare message type, no const or reference qualifiers
 *  - Event typedef, which provides the ros::MessageEvent type
 *  - Parameter typedef, which provides the actual parameter type (may be slightly different from M)
 *  - static getParameter(event) function, which returns a parameter type given the event
 *  - static bool is_const informs you whether or not the parameter type is a const message
 *
 *  ParameterAdapter is specialized to allow callbacks of any of the forms:
\verbatim
void callback(const boost::shared_ptr<M const>&);
void callback(const boost::shared_ptr<M>&);
void callback(boost::shared_ptr<M const>);
void callback(boost::shared_ptr<M>);
void callback(const M&);
void callback(M);
void callback(const MessageEvent<M const>&);
void callback(const MessageEvent<M>&);
\endverbatim
 */
template<typename M>
struct ParameterAdapter
{
  typedef typename boost::remove_reference<typename boost::remove_const<M>::type>::type Message;
  typedef ros::MessageEvent<Message const> Event;
  typedef M Parameter;
  static const bool is_const = true;

  static Parameter getParameter(const Event& event)
  {
    return *event.getMessage();
  }
};

template<typename M>
struct ParameterAdapter<const boost::shared_ptr<M const>& >
{
  typedef typename boost::remove_reference<typename boost::remove_const<M>::type>::type Message;
  typedef ros::MessageEvent<Message const> Event;
  typedef const boost::shared_ptr<Message const> Parameter;
  static const bool is_const = true;

  static Parameter getParameter(const Event& event)
  {
    return event.getMessage();
  }
};

template<typename M>
struct ParameterAdapter<const boost::shared_ptr<M>& >
{
  typedef typename boost::remove_reference<typename boost::remove_const<M>::type>::type Message;
  typedef ros::MessageEvent<Message const> Event;
  typedef boost::shared_ptr<Message> Parameter;
  static const bool is_const = false;

  static Parameter getParameter(const Event& event)
  {
    return ros::MessageEvent<Message>(event).getMessage();
  }
};

template<typename M>
struct ParameterAdapter<const M&>
{
  typedef typename boost::remove_reference<typename boost::remove_const<M>::type>::type Message;
  typedef ros::MessageEvent<Message const> Event;
  typedef const M& Parameter;
  static const bool is_const = true;

  static Parameter getParameter(const Event& event)
  {
    return *event.getMessage();
  }
};

template<typename M>
struct ParameterAdapter<boost::shared_ptr<M const> >
{
  typedef typename boost::remove_reference<typename boost::remove_const<M>::type>::type Message;
  typedef ros::MessageEvent<Message const> Event;
  typedef boost::shared_ptr<Message const> Parameter;
  static const bool is_const = true;

  static Parameter getParameter(const Event& event)
  {
    return event.getMessage();
  }
};

template<typename M>
struct ParameterAdapter<boost::shared_ptr<M> >
{
  typedef typename boost::remove_reference<typename boost::remove_const<M>::type>::type Message;
  typedef ros::MessageEvent<Message const> Event;
  typedef boost::shared_ptr<Message> Parameter;
  static const bool is_const = false;

  static Parameter getParameter(const Event& event)
  {
    return ros::MessageEvent<Message>(event).getMessage();
  }
};

template<typename M>
struct ParameterAdapter<const xcros::MessageEvent<M const>& >
{
  typedef typename boost::remove_reference<typename boost::remove_const<M>::type>::type Message;
  typedef ros::MessageEvent<Message const> Event;
  typedef const ros::MessageEvent<Message const>& Parameter;
  static const bool is_const = true;

  static Parameter getParameter(const Event& event)
  {
    return event;
  }
};

template<typename M>
struct ParameterAdapter<const xcros::MessageEvent<M>& >
{
  typedef typename boost::remove_reference<typename boost::remove_const<M>::type>::type Message;
  typedef ros::MessageEvent<Message const> Event;
  typedef ros::MessageEvent<Message> Parameter;
  static const bool is_const = false;

  static Parameter getParameter(const Event& event)
  {
    return ros::MessageEvent<Message>(event);
  }
};

} // namespace xcros



#endif