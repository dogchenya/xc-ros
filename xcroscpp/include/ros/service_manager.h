#ifndef _XCROSCPP_SERVICE_MANAGER_H_
#define _XCROSCPP_SERVICE_MANAGER_H_

#include "forwards.h"
#include "ros/xcroscpp_common.h"
#include "advertise_service_options.h"
#include "service_client_options.h"

#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>

namespace xcros
{

class ServiceManager;
typedef boost::shared_ptr<ServiceManager> ServiceManagerPtr;

class PollManager;
typedef boost::shared_ptr<PollManager> PollManagerPtr;

class XMLRPCManager;
typedef boost::shared_ptr<XMLRPCManager> XMLRPCManagerPtr;

class ConnectionManager;
typedef boost::shared_ptr<ConnectionManager> ConnectionManagerPtr;

class ROSCPP_DECL ServiceManager
{
public:
  static const ServiceManagerPtr& instance();

  ServiceManager();
  ~ServiceManager();

  /** @brief Lookup an advertised service.
   *
   * This method iterates over advertised_services, looking for one with name
   * matching the given topic name.  The advertised_services_mutex is locked
   * during this search.  This method is only used internally.
   *
   * @param service The service name to look for.
   *
   * @returns Pointer to the matching ServicePublication, NULL if none is found.
   */
  ServicePublicationPtr lookupServicePublication(const std::string& service);

  /** @brief Create a new client to the specified service.  If a client to that service already exists, returns the existing one.
   *
   * @param service The service to connect to
   * @param persistent Whether to keep this connection alive for more than one service call
   * @param request_md5sum The md5sum of the request message
   * @param response_md5sum The md5sum of the response message
   *
   * @returns Shared pointer to the ServiceServerLink, empty shared pointer if none is found.
   */
  ServiceServerLinkPtr createServiceServerLink(const std::string& service,
                                                bool persistent,
                                                const std::string& request_md5sum, const std::string& response_md5sum,
                                                const M_string& header_values);

  /** @brief Remove the specified service client from our list
   *
   * @param client The client to remove
   */
  void removeServiceServerLink(const ServiceServerLinkPtr& client);

  /** @brief Lookup the host/port of a service.
   *
   * @param name The name of the service
   * @param serv_host OUT -- The host of the service
   * @param serv_port OUT -- The port of the service
   */
  bool lookupService(const std::string& name, std::string& serv_host, uint32_t& serv_port);

  /** @brief Unadvertise a service.
   *
   * This call unadvertises a service, which must have been previously
   * advertised, using advertiseService().
   *
   * After this call completes, it is guaranteed that no further
   * callbacks will be invoked for this service.
   *
   * This method can be safely called from within a service callback.
   *
   * @param serv_name The service to be unadvertised.
   *
   * @return true on successful unadvertisement, false otherwise.
   */
  bool unadvertiseService(const std::string& serv_name);

  bool advertiseService(const AdvertiseServiceOptions& ops);

  void start();
  void shutdown();
private:

  bool isServiceAdvertised(const std::string& serv_name);
  bool unregisterService(const std::string& service);

  bool isShuttingDown() { return shutting_down_; }

  L_ServicePublication service_publications_;
  boost::mutex service_publications_mutex_;

  L_ServiceServerLink service_server_links_;
  boost::mutex service_server_links_mutex_;

  volatile bool shutting_down_;
  boost::recursive_mutex shutting_down_mutex_;

  PollManagerPtr poll_manager_;
  ConnectionManagerPtr connection_manager_;
  XMLRPCManagerPtr xmlrpc_manager_;
};

} // namespace ros

#endif