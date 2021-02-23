#include "ros/service.h"
#include "ros/connection.h"
#include "ros/service_server_link.h"
#include "ros/service_manager.h"
#include "ros/transport/transport_tcp.h"
#include "ros/poll_manager.h"
#include "ros/init.h"
#include "ros/names.h"
#include "ros/this_node.h"
#include "ros/header.h"

using namespace xcros;

bool service::exists(const std::string& service_name, bool print_failure_reason)
{
  std::string mapped_name = names::resolve(service_name);

  std::string host;
  uint32_t port;

  if (ServiceManager::Instance()->lookupService(mapped_name, host, port))
  {
    TransportTCPPtr transport(boost::make_shared<TransportTCP>(static_cast<xcros::PollSet*>(NULL), TransportTCP::SYNCHRONOUS));

    if (transport->connect(host, port))
    {
      M_string m;
      m["probe"] = "1";
      m["md5sum"] = "*";
      m["callerid"] = this_node::getName();
      m["service"] = mapped_name;
      boost::shared_array<uint8_t> buffer;
      uint32_t size = 0;;
      Header::write(m, buffer, size);
      transport->write((uint8_t*)&size, sizeof(size));
      transport->write(buffer.get(), size);
      transport->close();

      return true;
    }
    else
    {
      if (print_failure_reason)
      {
        ROS_INFO("waitForService: Service [%s] could not connect to host [%s:%d], waiting...", mapped_name.c_str(), host.c_str(), port);
      }
    }
  }
  else
  {
    if (print_failure_reason)
    {
      ROS_INFO("waitForService: Service [%s] has not been advertised, waiting...", mapped_name.c_str());
    }
  }

  return false;
}

bool service::waitForService(const std::string& service_name, xcros::Duration timeout)
{
  std::string mapped_name = names::resolve(service_name);

  Time start_time = Time::now();

  bool printed = false;
  bool result = false;
  while (xcros::ok())
  {
    if (exists(service_name, !printed))
    {
      result = true;
      break;
    }
    else
    {
      printed = true;

      if (timeout >= Duration(0))
      {
        Time current_time = Time::now();

        if ((current_time - start_time) >= timeout)
        {
          return false;
        }
      }

      Duration(0.02).sleep();
    }
  }

  if (printed && xcros::ok())
  {
    ROS_INFO("waitForService: Service [%s] is now available.", mapped_name.c_str());
  }

  return result;
}

bool service::waitForService(const std::string& service_name, int32_t timeout)
{
  return waitForService(service_name, xcros::Duration(timeout / 1000.0));
}
