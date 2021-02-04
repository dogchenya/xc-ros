#ifndef _XCROSCPP_FORWARDS_H_
#define _XCROSCPP_FORWARDS_H_

#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>

#include <memory>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/function.hpp>

#include <ros/time.h>
#include <ros/macros.h>
#include "exceptions.h"
#include "ros/datatypes.h"

namespace xcros
{

typedef std::shared_ptr<void> VoidPtr;
typedef std::weak_ptr<void> VoidWPtr;
typedef std::shared_ptr<void const> VoidConstPtr;
typedef std::weak_ptr<void const> VoidConstWPtr;

class Header;
class Transport;
typedef std::shared_ptr<Transport> TransportPtr;
class TransportTCP;
typedef std::shared_ptr<TransportTCP> TransportTCPPtr;
class TransportUDP;
typedef std::shared_ptr<TransportUDP> TransportUDPPtr;
class Connection;
typedef std::shared_ptr<Connection> ConnectionPtr;
typedef std::set<ConnectionPtr> S_Connection;
typedef std::vector<ConnectionPtr> V_Connection;
class Publication;
typedef std::shared_ptr<Publication> PublicationPtr;
typedef std::vector<PublicationPtr> V_Publication;
class SubscriberLink;
typedef std::shared_ptr<SubscriberLink> SubscriberLinkPtr;
typedef std::vector<SubscriberLinkPtr> V_SubscriberLink;
class Subscription;
typedef std::shared_ptr<Subscription> SubscriptionPtr;
typedef std::weak_ptr<Subscription> SubscriptionWPtr;
typedef std::list<SubscriptionPtr> L_Subscription;
typedef std::vector<SubscriptionPtr> V_Subscription;
typedef std::set<SubscriptionPtr> S_Subscription;
class PublisherLink;
typedef std::shared_ptr<PublisherLink> PublisherLinkPtr;
typedef std::vector<PublisherLinkPtr> V_PublisherLink;
class ServicePublication;
typedef std::shared_ptr<ServicePublication> ServicePublicationPtr;
typedef std::list<ServicePublicationPtr> L_ServicePublication;
typedef std::vector<ServicePublicationPtr> V_ServicePublication;
class ServiceServerLink;
typedef std::shared_ptr<ServiceServerLink> ServiceServerLinkPtr;
typedef std::list<ServiceServerLinkPtr> L_ServiceServerLink;
class Transport;
typedef std::shared_ptr<Transport> TransportPtr;
class NodeHandle;
typedef std::shared_ptr<NodeHandle> NodeHandlePtr;


class SingleSubscriberPublisher;
typedef boost::function<void(const SingleSubscriberPublisher&)> SubscriberStatusCallback;

class CallbackQueue;
class CallbackQueueInterface;
class CallbackInterface;
typedef std::shared_ptr<CallbackInterface> CallbackInterfacePtr;

struct SubscriberCallbacks
{
  SubscriberCallbacks(const SubscriberStatusCallback& connect = SubscriberStatusCallback(),
                      const SubscriberStatusCallback& disconnect = SubscriberStatusCallback(),
                      const VoidConstPtr& tracked_object = VoidConstPtr(),
                      CallbackQueueInterface* callback_queue = 0)
  : connect_(connect)
  , disconnect_(disconnect)
  , callback_queue_(callback_queue)
  {
    has_tracked_object_ = false;
    if (tracked_object)
    {
      has_tracked_object_ = true;
      tracked_object_ = tracked_object;
    }
  }
  SubscriberStatusCallback connect_;
  SubscriberStatusCallback disconnect_;

  bool has_tracked_object_;
  VoidConstWPtr tracked_object_;
  CallbackQueueInterface* callback_queue_;
};
typedef std::shared_ptr<SubscriberCallbacks> SubscriberCallbacksPtr;

/**
 * \brief Structure passed as a parameter to the callback invoked by a ros::Timer
 */
struct TimerEvent
{
  Time last_expected;                     ///< In a perfect world, this is when the last callback should have happened
  Time last_real;                         ///< When the last callback actually happened

  Time current_expected;                  ///< In a perfect world, this is when the current callback should be happening
  Time current_real;                      ///< This is when the current callback was actually called (Time::now() as of the beginning of the callback)

  struct
  {
    WallDuration last_duration;           ///< How long the last callback ran for
  } profile;
};
typedef boost::function<void(const TimerEvent&)> TimerCallback;

/**
 * \brief Structure passed as a parameter to the callback invoked by a ros::WallTimer
 */
struct WallTimerEvent
{
  WallTime last_expected;                 ///< In a perfect world, this is when the last callback should have happened
  WallTime last_real;                     ///< When the last callback actually happened

  WallTime current_expected;              ///< In a perfect world, this is when the current callback should be happening
  WallTime current_real;                  ///< This is when the current callback was actually called (Time::now() as of the beginning of the callback)

  struct
  {
    WallDuration last_duration;           ///< How long the last callback ran for
  } profile;
};
typedef boost::function<void(const WallTimerEvent&)> WallTimerCallback;

/**
 * \brief Structure passed as a parameter to the callback invoked by a ros::SteadyTimer
 */
struct SteadyTimerEvent
{
  SteadyTime last_expected;            ///< In a perfect world, this is when the last callback should have happened
  SteadyTime last_real;                ///< When the last callback actually happened

  SteadyTime current_expected;         ///< In a perfect world, this is when the current callback should be happening
  SteadyTime current_real;             ///< This is when the current callback was actually called (SteadyTime::now() as of the beginning of the callback)

  struct
  {
    WallDuration last_duration;           ///< How long the last callback ran for
  } profile;
};
typedef boost::function<void(const SteadyTimerEvent&)> SteadyTimerCallback;

class ServiceManager;
typedef std::shared_ptr<ServiceManager> ServiceManagerPtr;
class TopicManager;
typedef std::shared_ptr<TopicManager> TopicManagerPtr;
class ConnectionManager;
typedef std::shared_ptr<ConnectionManager> ConnectionManagerPtr;
class XMLRPCManager;
typedef std::shared_ptr<XMLRPCManager> XMLRPCManagerPtr;
class PollManager;
typedef std::shared_ptr<PollManager> PollManagerPtr;

}

#endif