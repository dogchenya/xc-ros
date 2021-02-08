#ifndef ROSCPP_SERVICE_H
#define ROSCPP_SERVICE_H

#include <string>
#include "ros/xcroscpp_common.h"
#include "ros/message.h"
#include "ros/forwards.h"
#include "ros/node_handle.h"
#include "ros/service_traits.h"
#include "ros/names.h"

#include <boost/shared_ptr.hpp>

namespace xcros
{

class ServiceServerLink;
typedef boost::shared_ptr<ServiceServerLink> ServiceServerLinkPtr;

/**
 * \brief Contains functions for querying information about and calling a service
 */
namespace service
{

/** @brief Invoke an RPC service.
 *
 * This method invokes an RPC service on a remote server, looking up the
 * service location first via the master.
 *
 * @param service_name The name of the service.
 * @param req The request message.
 * @param[out] res Storage for the response message.
 *
 * @return true on success, false otherwise.
 */
template<class MReq, class MRes>
bool call(const std::string& service_name, MReq& req, MRes& res)
{
  namespace st = service_traits;
  NodeHandle nh;
  ServiceClientOptions ops(xcros::names::resolve(service_name), st::md5sum(req), false, M_string());
  ServiceClient client = nh.serviceClient(ops);
  return client.call(req, res);
}

/** @brief Invoke an RPC service.
 *
 * This method invokes an RPC service on a remote server, looking up the
 * service location first via the master.
 *
 * @param service_name The name of the service.
 * @param service The service class that contains the request and response messages
 *
 * @return true on success, false otherwise.
 */
template<class Service>
bool call(const std::string& service_name, Service& service)
{
  namespace st = service_traits;

  NodeHandle nh;
  ServiceClientOptions ops(xcros::names::resolve(service_name), st::md5sum(service), false, M_string());
  ServiceClient client = nh.serviceClient(ops);
  return client.call(service.request, service.response);
}

/**
 * \brief Wait for a service to be advertised and available.  Blocks until it is.
 * \param service_name Name of the service to wait for.
 * \param timeout The amount of time to wait for, in milliseconds.  If timeout is -1,
 * waits until the node is shutdown
 * \return true on success, false otherwise
 */
ROSCPP_DECL bool waitForService(const std::string& service_name, int32_t timeout);

/**
 * \brief Wait for a service to be advertised and available.  Blocks until it is.
 * \param service_name Name of the service to wait for.
 * \param timeout The amount of time to wait for before timing out.  If timeout is -1 (default),
 * waits until the node is shutdown
 * \return true on success, false otherwise
 */
ROSCPP_DECL bool waitForService(const std::string& service_name, xcros::Duration timeout = xcros::Duration(-1));

/**
 * \brief Checks if a service is both advertised and available.
 * \param service_name Name of the service to check for
 * \param print_failure_reason Whether to print the reason for failure to the console (service not advertised vs.
 * could not connect to the advertised host)
 * \return true if the service is up and available, false otherwise
 */
ROSCPP_DECL bool exists(const std::string& service_name, bool print_failure_reason);

/** @brief Create a client for a service.
 *
 * When the last handle reference of a persistent connection is cleared, the connection will automatically close.
 *
 * @param service_name The name of the service to connect to
 * @param persistent Whether this connection should persist.  Persistent services keep the connection to the remote host active
 *        so that subsequent calls will happen faster.  In general persistent services are discouraged, as they are not as
 *        robust to node failure as non-persistent services.
 * @param header_values Key/value pairs you'd like to send along in the connection handshake
 */
template<class MReq, class MRes>
ServiceClient createClient(const std::string& service_name, bool persistent = false, const M_string& header_values = M_string())
{
  NodeHandle nh;
  ServiceClient client = nh.template serviceClient<MReq, MRes>(xcros::names::resolve(service_name), persistent, header_values);
  return client;
}

/** @brief Create a client for a service.
 *
 * When the last handle reference of a persistent connection is cleared, the connection will automatically close.
 *
 * @param service_name The name of the service to connect to
 * @param persistent Whether this connection should persist.  Persistent services keep the connection to the remote host active
 *        so that subsequent calls will happen faster.  In general persistent services are discouraged, as they are not as
 *        robust to node failure as non-persistent services.
 * @param header_values Key/value pairs you'd like to send along in the connection handshake
 */
template<class Service>
ServiceClient createClient(const std::string& service_name, bool persistent = false, const M_string& header_values = M_string())
{
  NodeHandle nh;
  ServiceClient client = nh.template serviceClient<Service>(xcros::names::resolve(service_name), persistent, header_values);
  return client;
}

}

}

#endif // ROSCPP_SERVICE_H