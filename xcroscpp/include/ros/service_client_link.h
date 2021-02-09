#ifndef _XCROSCPP_SERVICE_CLIENT_LINK_H_
#define _XCROSCPP_SERVICE_CLIENT_LINK_H_

#include "ros/xcroscpp_common.h"

#include <boost/thread/mutex.hpp>
#include <boost/shared_array.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/signals2/connection.hpp>

#include <queue>

#include "ros/serialized_message.h"

namespace xcros
{
class Header;
class ServicePublication;
typedef boost::weak_ptr<ServicePublication> ServicePublicationWPtr;
typedef boost::shared_ptr<ServicePublication> ServicePublicationPtr;
class Connection;
typedef boost::shared_ptr<Connection> ConnectionPtr;

/**
 * \brief Handles a connection to a single incoming service client.
 */
class ROSCPP_DECL ServiceClientLink : public boost::enable_shared_from_this<ServiceClientLink>
{
public:
  ServiceClientLink();
  virtual ~ServiceClientLink();

  //
  bool initialize(const ConnectionPtr& connection);
  bool handleHeader(const Header& header);

  /**
   * \brief Writes a response to the current request.
   * \param ok Whether the callback was successful or not
   * \param resp The message response.  ServiceClientLink will delete this
   */
  void processResponse(bool ok, const SerializedMessage& res);

  const ConnectionPtr& getConnection() { return connection_; }

private:
  void onConnectionDropped(const ConnectionPtr& conn);

  void onHeaderWritten(const ConnectionPtr& conn);
  void onRequestLength(const ConnectionPtr& conn, const boost::shared_array<uint8_t>& buffer, uint32_t size, bool success);
  void onRequest(const ConnectionPtr& conn, const boost::shared_array<uint8_t>& buffer, uint32_t size, bool success);
  void onResponseWritten(const ConnectionPtr& conn);

  ConnectionPtr connection_;
  ServicePublicationWPtr parent_;
  bool persistent_;
  boost::signals2::connection dropped_conn_;
};
typedef boost::shared_ptr<ServiceClientLink> ServiceClientLinkPtr;

} // namespace xcros


#endif