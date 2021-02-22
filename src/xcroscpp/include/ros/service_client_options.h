#ifndef _XCROSCPP_SERVICE_CLIENT_OPTIONS_H_
#define _XCROSCPP_SERVICE_CLIENT_OPTIONS_H_

#include "ros/forwards.h"
#include "ros/xcroscpp_common.h"
#include "ros/service_traits.h"

namespace xcros
{

struct ROSCPP_DECL ServiceClientOptions
{
  ServiceClientOptions()
  : persistent(false)
  {
  }

  /*
   * \brief Constructor
   * \param _service Name of the service to connect to
   * \param _md5sum md5sum of the service
   * \param _persistent Whether or not to keep the connection open to the service for future calls
   * \param _header Any extra values to be passed along in the connection header
   */
  ServiceClientOptions(const std::string& _service, const std::string& _md5sum, bool _persistent, const M_string& _header)
  : service(_service)
  , md5sum(_md5sum)
  , persistent(_persistent)
  , header(_header)
  {
  }

  /*
   * \brief Templated helper method, preventing you from needing to manually get the service md5sum
   * \param MReq [template] Request message type
   * \param MRes [template] Response message type
   * \param _service Name of the service to connect to
   * \param _persistent Whether or not to keep the connection open to the service for future calls
   * \param _header Any extra values to be passed along in the connection header
   */
  template <class MReq, class MRes>
  void init(const std::string& _service, bool _persistent, const M_string& _header)
  {
    namespace st = service_traits;

    service = _service;
    md5sum = st::md5sum<MReq>();
    persistent = _persistent;
    header = _header;
  }

  /*
   * \brief Templated helper method, preventing you from needing to manually get the service md5sum
   * \param Service [template] Service type
   * \param _service Name of the service to connect to
   * \param _persistent Whether or not to keep the connection open to the service for future calls
   * \param _header Any extra values to be passed along in the connection header
   */
  template <class Service>
  void init(const std::string& _service, bool _persistent, const M_string& _header)
  {
    namespace st = service_traits;

    service = _service;
    md5sum = st::md5sum<Service>();
    persistent = _persistent;
    header = _header;
  }

  std::string service;                                                      ///< Service to connect to
  std::string md5sum;                                                       ///< Service md5sum
  bool persistent;                                                          ///< Whether or not the connection should persist
  M_string header;                                                          ///< Extra key/value pairs to add to the connection header
};
    
} // namespace xcros



#endif