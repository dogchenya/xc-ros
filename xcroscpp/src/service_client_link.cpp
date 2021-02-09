
#include "ros/service_client_link.h"
#include "ros/service_publication.h"
#include "ros/header.h"
#include "ros/connection.h"
#include "ros/service_manager.h"
#include "ros/transport/transport.h"
#include "ros/this_node.h"
#include "ros/file_log.h"

#include <boost/bind.hpp>

namespace xcros
{

ServiceClientLink::ServiceClientLink()
: persistent_(false)
{
}

ServiceClientLink::~ServiceClientLink()
{
  if (connection_)
  {
    if (connection_->isSendingHeaderError())
    {
      connection_->removeDropListener(dropped_conn_);
    }
    else
    {
      connection_->drop(Connection::Destructing);
    }
  }
}

bool ServiceClientLink::initialize(const ConnectionPtr& connection)
{
  connection_ = connection;
  dropped_conn_ = connection_->addDropListener(boost::bind(&ServiceClientLink::onConnectionDropped, this, _1));

  return true;
}

bool ServiceClientLink::handleHeader(const Header& header)
{
  std::string md5sum, service, client_callerid;
  if (!header.getValue("md5sum", md5sum)
   || !header.getValue("service", service)
   || !header.getValue("callerid", client_callerid))
  {
    std::string msg("bogus tcpros header. did not have the "
                          "required elements: md5sum, service, callerid");

    ROS_ERROR("%s", msg.c_str());
    connection_->sendHeaderError(msg);

    return false;
  }

  std::string persistent;
  if (header.getValue("persistent", persistent))
  {
    if (persistent == "1" || persistent == "true")
    {
      persistent_ = true;
    }
  }

  ROSCPP_LOG_DEBUG("Service client [%s] wants service [%s] with md5sum [%s]", client_callerid.c_str(), service.c_str(), md5sum.c_str());
  ServicePublicationPtr ss = ServiceManager::Instance()->lookupServicePublication(service);
  if (!ss)
  {
    std::string msg = std::string("received a tcpros connection for a "
                             "nonexistent service [") +
            service + std::string("].");

    ROS_ERROR("%s", msg.c_str());
    connection_->sendHeaderError(msg);

    return false;
  }
  if (ss->getMD5Sum() != md5sum &&
      (md5sum != std::string("*") && ss->getMD5Sum() != std::string("*")))
  {
    std::string msg = std::string("client wants service ") + service +
            std::string(" to have md5sum ") + md5sum +
            std::string(", but it has ") + ss->getMD5Sum() +
            std::string(". Dropping connection.");

    ROS_ERROR("%s", msg.c_str());
    connection_->sendHeaderError(msg);

    return false;
  }

  // Check whether the service (ss here) has been deleted from
  // advertised_topics through a call to unadvertise(), which could
  // have happened while we were waiting for the subscriber to
  // provide the md5sum.
  if(ss->isDropped())
  {
    std::string msg = std::string("received a tcpros connection for a "
                             "nonexistent service [") +
            service + std::string("].");

    ROS_ERROR("%s", msg.c_str());
    connection_->sendHeaderError(msg);

    return false;
  }
  else
  {
    parent_ = ServicePublicationWPtr(ss);

    // Send back a success, with info
    M_string m;
    m["request_type"] = ss->getRequestDataType();
    m["response_type"] = ss->getResponseDataType();
    m["type"] = ss->getDataType();
    m["md5sum"] = ss->getMD5Sum();
    m["callerid"] = this_node::getName();
    connection_->writeHeader(m, boost::bind(&ServiceClientLink::onHeaderWritten, this, _1));

    ss->addServiceClientLink(shared_from_this());
  }

  return true;
}

void ServiceClientLink::onConnectionDropped(const ConnectionPtr& conn)
{
  (void)conn;
  ROS_ASSERT(conn == connection_);

  if (ServicePublicationPtr parent = parent_.lock())
  {
    parent->removeServiceClientLink(shared_from_this());
  }
}

void ServiceClientLink::onHeaderWritten(const ConnectionPtr& conn)
{
  (void)conn;
  connection_->read(4, boost::bind(&ServiceClientLink::onRequestLength, this, _1, _2, _3, _4));
}

void ServiceClientLink::onRequestLength(const ConnectionPtr& conn, const boost::shared_array<uint8_t>& buffer, uint32_t size, bool success)
{
  (void)size;
  if (!success)
    return;

  ROS_ASSERT(conn == connection_);
  ROS_ASSERT(size == 4);

  uint32_t len = *((uint32_t*)buffer.get());

  if (len > 1000000000)
  {
    ROS_ERROR("a message of over a gigabyte was " \
                "predicted in tcpros. that seems highly " \
                "unlikely, so I'll assume protocol " \
                "synchronization is lost.");
    conn->drop(Connection::Destructing);
    return;
  }

  connection_->read(len, boost::bind(&ServiceClientLink::onRequest, this, _1, _2, _3, _4));
}

void ServiceClientLink::onRequest(const ConnectionPtr& conn, const boost::shared_array<uint8_t>& buffer, uint32_t size, bool success)
{
  (void)conn;
  if (!success)
    return;

  ROS_ASSERT(conn == connection_);

  if (ServicePublicationPtr parent = parent_.lock())
  {
    parent->processRequest(buffer, size, shared_from_this());
  }
  else
  {
    ROS_BREAK();
  }
}

void ServiceClientLink::onResponseWritten(const ConnectionPtr& conn)
{
  (void)conn;
  ROS_ASSERT(conn == connection_);

  if (persistent_)
  {
    connection_->read(4, boost::bind(&ServiceClientLink::onRequestLength, this, _1, _2, _3, _4));
  }
  else
  {
    connection_->drop(Connection::Destructing);
  }
}

void ServiceClientLink::processResponse(bool ok, const SerializedMessage& res)
{
  (void)ok;
  connection_->write(res.buf, res.num_bytes, boost::bind(&ServiceClientLink::onResponseWritten, this, _1));
}


} // namespace xcros