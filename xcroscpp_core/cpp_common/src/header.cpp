
#include "ros/header.h"

#include "console_bridge/console.h"

#include <sstream>

#include <cstring>
#include <cerrno>

#define SROS_SERIALIZE_PRIMITIVE(ptr, data) { memcpy(ptr, &data, sizeof(data)); ptr += sizeof(data); }
#define SROS_SERIALIZE_BUFFER(ptr, data, data_size) { if (data_size > 0) { memcpy(ptr, data, data_size); ptr += data_size; } }
#define SROS_DESERIALIZE_PRIMITIVE(ptr, data) { memcpy(&data, ptr, sizeof(data)); ptr += sizeof(data); }
#define SROS_DESERIALIZE_BUFFER(ptr, data, data_size) { if (data_size > 0) { memcpy(data, ptr, data_size); ptr += data_size; } }

// Remove this when no longer supporting platforms with libconsole-bridge-dev < 0.3.0,
// in particular Debian Jessie: https://packages.debian.org/jessie/libconsole-bridge-dev
#ifndef CONSOLE_BRIDGE_logError
# define CONSOLE_BRIDGE_logError(fmt, ...)  \
  console_bridge::log(__FILE__, __LINE__, console_bridge::CONSOLE_BRIDGE_LOG_ERROR, fmt, ##__VA_ARGS__)
#endif

using namespace std;

namespace xcros
{

Header::Header()
: read_map_(new M_string())
{

}

Header::~Header()
{

}

bool Header::parse(const boost::shared_array<uint8_t>& buffer, uint32_t size, std::string& error_msg)
{
  return parse(buffer.get(), size, error_msg);
}

bool Header::parse(uint8_t* buffer, uint32_t size, std::string& error_msg)
{
  uint8_t* buf = buffer;
  while (buf < buffer + size)
  {
    uint32_t len;
    SROS_DESERIALIZE_PRIMITIVE(buf, len);

    if (len > 1000000)
    {
      error_msg = "Received an invalid TCPROS header.  Each element must be prepended by a 4-byte length.";
      CONSOLE_BRIDGE_logError("%s", error_msg.c_str());

      return false;
    }

    std::string line((char*)buf, len);

    buf += len;

    //printf(":%s:\n", line.c_str());
    size_t eqpos = line.find_first_of("=", 0);
    if (eqpos == string::npos)
    {
      error_msg = "Received an invalid TCPROS header.  Each line must have an equals sign.";
      CONSOLE_BRIDGE_logError("%s", error_msg.c_str());

      return false;
    }
    string key = line.substr(0, eqpos);
    string value = line.substr(eqpos+1);

    (*read_map_)[key] = value;
  }

  return true;
}

bool Header::getValue(const std::string& key, std::string& value) const
{
  M_string::const_iterator it = read_map_->find(key);
  if (it == read_map_->end())
  {
    return false;
  }

  value = it->second;

  return true;
}

void Header::write(const M_string& key_vals, boost::shared_array<uint8_t>& buffer, uint32_t& size)
{
  // Calculate the necessary size
  size = 0;
  {
    M_string::const_iterator it = key_vals.begin();
    M_string::const_iterator end = key_vals.end();
    for (; it != end; ++it)
    {
      const std::string& key = it->first;
      const std::string& value = it->second;

      size += key.length();
      size += value.length();
      size += 1; // = sign
      size += 4; // 4-byte length
    }
  }

  if (size == 0)
  {
    return;
  }

  buffer.reset(new uint8_t[size]);
  char* ptr = (char*)buffer.get();

  // Write the data
  {
    M_string::const_iterator it = key_vals.begin();
    M_string::const_iterator end = key_vals.end();
    for (; it != end; ++it)
    {
      const std::string& key = it->first;
      const std::string& value = it->second;

      uint32_t len = key.length() + value.length() + 1;
      SROS_SERIALIZE_PRIMITIVE(ptr, len);
      SROS_SERIALIZE_BUFFER(ptr, key.data(), key.length());
      static const char equals = '=';
      SROS_SERIALIZE_PRIMITIVE(ptr, equals);
      SROS_SERIALIZE_BUFFER(ptr, value.data(), value.length());
    }
  }

  assert(ptr == (char*)buffer.get() + size);
}

}
