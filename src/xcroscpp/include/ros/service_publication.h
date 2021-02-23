#ifndef _XCROSCPP_SERVICE_PUBLICATION_H_
#define _XCROSCPP_SERVICE_PUBLICATION_H_

#include "ros/service_callback_helper.h"
#include "ros/xcroscpp_common.h"
#include "rpcpp/RobotRpc.h"

#include <boost/thread/mutex.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/thread.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <vector>
#include <queue>

namespace xcros
{

class ServiceClientLink;
typedef boost::shared_ptr<ServiceClientLink> ServiceClientLinkPtr;
typedef std::vector<ServiceClientLinkPtr> V_ServiceClientLink;
class CallbackQueueInterface;

class Message;

/**
 * \brief Manages an advertised service.
 *
 * ServicePublication manages all incoming service requests.  If its thread pool size is not 0, it will queue the requests
 * into a number of threads, calling the callback from within those threads.  Otherwise it immediately calls the callback
 */
class ROSCPP_DECL ServicePublication : public boost::enable_shared_from_this<ServicePublication>
{
public:
  ServicePublication(const std::string& name, const std::string &md5sum, const std::string& data_type, const std::string& request_data_type,
                const std::string& response_data_type, const ServiceCallbackHelperPtr& helper, CallbackQueueInterface* queue,
                const VoidConstPtr& tracked_object);
  ~ServicePublication();

  /**
   * \brief Adds a request to the queue if our thread pool size is not 0, otherwise immediately calls the callback
   */
  void processRequest(boost::shared_array<uint8_t> buf, size_t num_bytes, const ServiceClientLinkPtr& link);

  /**
   * \brief Adds a service link for us to manage
   */
  void addServiceClientLink(const ServiceClientLinkPtr& link);
  /**
   * \brief Removes a service link from our list
   */
  void removeServiceClientLink(const ServiceClientLinkPtr& link);

  /**
   * \brief Terminate this service server
   */
  void drop();
  /**
   * \brief Returns whether or not this service server is valid
   */
  bool isDropped() { return dropped_; }

  const std::string& getMD5Sum() { return md5sum_; }
  const std::string& getRequestDataType() { return request_data_type_; }
  const std::string& getResponseDataType() { return response_data_type_; }
  const std::string& getDataType() { return data_type_; }
  const std::string& getName() { return name_; }

private:
  void dropAllConnections();

  std::string name_;
  std::string md5sum_;
  std::string data_type_;
  std::string request_data_type_;
  std::string response_data_type_;
  ServiceCallbackHelperPtr helper_;

  V_ServiceClientLink client_links_;
  boost::mutex client_links_mutex_;

  bool dropped_;

  CallbackQueueInterface* callback_queue_;
  bool has_tracked_object_;
  VoidConstWPtr tracked_object_;
};
typedef boost::shared_ptr<ServicePublication> ServicePublicationPtr;

}

#endif