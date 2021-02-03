#ifndef _XCROS_CORE_HEADER_H_
#define _XCROS_CORE_HEADER_H_

#include <stdint.h>

#include <boost/shared_array.hpp>

#include "ros/datatypes.h"
#include "ros/cpp_common_decl.h"

namespace xcros
{

class CPP_COMMON_DECL Header
{
public:
  Header();
  ~Header();

  /**
   * \brief Get a value from a parsed header
   * \param key Key value
   * \param value OUT -- value corresponding to the key if there is one
   * \return Returns true if the header had the specified key in it
   */
  bool getValue(const std::string& key, std::string& value) const;
  /**
   * \brief Returns a shared pointer to the internal map used
   */
  M_stringPtr getValues() { return read_map_; }

  /**
   * \brief Parse a header out of a buffer of data
   */
  bool parse(const boost::shared_array<uint8_t>& buffer, uint32_t size, std::string& error_msg);

  /**
   * \brief Parse a header out of a buffer of data
   */
  bool parse(uint8_t* buffer, uint32_t size, std::string& error_msg);

  static void write(const M_string& key_vals, boost::shared_array<uint8_t>& buffer, uint32_t& size);

private:

  M_stringPtr read_map_;
};

}

#endif