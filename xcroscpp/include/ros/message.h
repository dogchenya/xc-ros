#ifndef _XCROSCPP_MESSAGE_H_
#define _XCROSCPP_MESSAGE_H_

#include <map>

#include "ros/macros.h"
#include "ros/assert.h"
#include <string>
#include <string.h>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>
#include <stdint.h>

#define ROSCPP_MESSAGE_HAS_DEFINITION

namespace xcros
{
    typedef std::map<std::string, std::string> M_string;

/**
 * \deprecated This base-class is deprecated in favor of a template-based serialization and traits system
 */
#if 0
class Message
{
public:
  typedef boost::shared_ptr<Message> Ptr;
  typedef boost::shared_ptr<Message const> ConstPtr;
  Message()
  {
  }
  virtual ~Message()
  {
  }
  virtual const std::string __getDataType() const = 0;
  virtual const std::string __getMD5Sum() const = 0;
  virtual const std::string __getMessageDefinition() const = 0;
  inline static std::string __s_getDataType() { ROS_BREAK(); return std::string(""); }
  inline static std::string __s_getMD5Sum() { ROS_BREAK(); return std::string(""); }
  inline static std::string __s_getMessageDefinition() { ROS_BREAK(); return std::string(""); }
  virtual uint32_t serializationLength() const = 0;
  virtual uint8_t *serialize(uint8_t *write_ptr, uint32_t seq) const = 0;
  virtual uint8_t *deserialize(uint8_t *read_ptr) = 0;
  uint32_t __serialized_length;
};

typedef boost::shared_ptr<Message> MessagePtr;
typedef boost::shared_ptr<Message const> MessageConstPtr;
#endif

#define SROS_SERIALIZE_PRIMITIVE(ptr, data) { memcpy(ptr, &data, sizeof(data)); ptr += sizeof(data); }
#define SROS_SERIALIZE_BUFFER(ptr, data, data_size) { if (data_size > 0) { memcpy(ptr, data, data_size); ptr += data_size; } }
#define SROS_DESERIALIZE_PRIMITIVE(ptr, data) { memcpy(&data, ptr, sizeof(data)); ptr += sizeof(data); }
#define SROS_DESERIALIZE_BUFFER(ptr, data, data_size) { if (data_size > 0) { memcpy(data, ptr, data_size); ptr += data_size; } }
    
} // namespace xcros



#endif