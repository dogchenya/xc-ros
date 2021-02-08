#include "ros/subscriber_link.h"
#include "ros/publication.h"

#include "rosconsole/macros_generated.h"
#include "ros/assert.h"

#include <boost/bind.hpp>

namespace xcros
{

SubscriberLink::SubscriberLink()
  : connection_id_(0)
{

}

SubscriberLink::~SubscriberLink()
{

}

bool SubscriberLink::verifyDatatype(const std::string &datatype)
{
  PublicationPtr parent = parent_.lock();
  if (!parent)
  {
    ROS_ERROR("Trying to verify the datatype on a publisher without a parent");
    ROS_BREAK();

    return false;
  }

  if (datatype != parent->getDataType())
  {
    ROS_ERROR( "tried to send a message with type %s on a " \
                       "TransportSubscriberLink that has datatype %s",
                datatype.c_str(), parent->getDataType().c_str());
    return false; // todo: figure out a way to log this error
  }

  return true;
}

const std::string& SubscriberLink::getMD5Sum()
{
  PublicationPtr parent = parent_.lock();
  return parent->getMD5Sum();
}

const std::string& SubscriberLink::getDataType()
{
  PublicationPtr parent = parent_.lock();
  return parent->getDataType();
}

const std::string& SubscriberLink::getMessageDefinition()
{
  PublicationPtr parent = parent_.lock();
  return parent->getMessageDefinition();
}

} // namespace xcros