#ifndef _XCROSCPP_SERVICE_MESSAGE_HELPER_H_
#define _XCROSCPP_SERVICE_MESSAGE_HELPER_H_

#include "ros/forwards.h"
#include "ros/xcroscpp_common.h"
#include "ros/message.h"
#include "ros/message_traits.h"
#include "ros/service_traits.h"
#include "ros/serialization.h"

#include <boost/type_traits/is_base_of.hpp>
#include <boost/utility/enable_if.hpp>

namespace xcros
{
struct ROSCPP_DECL ServiceCallbackHelperCallParams
{
  SerializedMessage request;
  SerializedMessage response;
  boost::shared_ptr<M_string> connection_header;
};

template<typename M>
inline boost::shared_ptr<M> defaultServiceCreateFunction()
{
  return boost::make_shared<M>();
}

template<typename MReq, typename MRes>
struct ServiceSpecCallParams
{
  boost::shared_ptr<MReq> request;
  boost::shared_ptr<MRes> response;
  boost::shared_ptr<M_string> connection_header;
};

/**
 * \brief Event type for services, ros::ServiceEvent<MReq, MRes>& can be used in your callback instead of MReq&, MRes&
 *
 * Useful if you need to retrieve meta-data about the call, such as the full connection header, or the caller's node name
 */
template<typename MReq, typename MRes>
class ServiceEvent
{
public:
  typedef MReq RequestType;
  typedef MRes ResponseType;
  typedef boost::shared_ptr<RequestType> RequestPtr;
  typedef boost::shared_ptr<ResponseType> ResponsePtr;
  typedef boost::function<bool(ServiceEvent<RequestType, ResponseType>&)> CallbackType;

  static bool call(const CallbackType& cb, ServiceSpecCallParams<RequestType, ResponseType>& params)
  {
    ServiceEvent<RequestType, ResponseType> event(params.request, params.response, params.connection_header);
    return cb(event);
  }

  ServiceEvent(const boost::shared_ptr<MReq const>& req, const boost::shared_ptr<MRes>& res, const boost::shared_ptr<M_string>& connection_header)
  : request_(req)
  , response_(res)
  , connection_header_(connection_header)
  {}

  /**
   * \brief Returns a const-reference to the request
   */
  const RequestType& getRequest() const { return *request_; }
  /**
   * \brief Returns a non-const reference to the response
   */
  ResponseType& getResponse() const { return *response_; }
  /**
   * \brief Returns a reference to the connection header.
   */
  M_string& getConnectionHeader() const { return *connection_header_; }

  /**
   * \brief Returns the name of the node which called this service
   */
  const std::string& getCallerName() const { return (*connection_header_)["callerid"]; }
private:
  boost::shared_ptr<RequestType const> request_;
  boost::shared_ptr<ResponseType> response_;
  boost::shared_ptr<M_string> connection_header_;
};

template<typename MReq, typename MRes>
struct ServiceSpec
{
  typedef MReq RequestType;
  typedef MRes ResponseType;
  typedef boost::shared_ptr<RequestType> RequestPtr;
  typedef boost::shared_ptr<ResponseType> ResponsePtr;
  typedef boost::function<bool(RequestType&, ResponseType&)> CallbackType;

  static bool call(const CallbackType& cb, ServiceSpecCallParams<RequestType, ResponseType>& params)
  {
    return cb(*params.request, *params.response);
  }
};

/**
 * \brief Abstract base class used by service servers to deal with concrete message types through a common
 * interface.  This is one part of the roscpp API that is \b not fully stable, so overloading this class
 * is not recommended
 */
class ROSCPP_DECL ServiceCallbackHelper
{
public:
  virtual ~ServiceCallbackHelper() {}
  virtual bool call(ServiceCallbackHelperCallParams& params) = 0;
};
typedef boost::shared_ptr<ServiceCallbackHelper> ServiceCallbackHelperPtr;

/**
 * \brief Concrete generic implementation of ServiceCallbackHelper for any normal service type
 */
template<typename Spec>
class ServiceCallbackHelperT : public ServiceCallbackHelper
{
public:
  typedef typename Spec::RequestType RequestType;
  typedef typename Spec::ResponseType ResponseType;
  typedef typename Spec::RequestPtr RequestPtr;
  typedef typename Spec::ResponsePtr ResponsePtr;
  typedef typename Spec::CallbackType Callback;
  typedef boost::function<RequestPtr()> ReqCreateFunction;
  typedef boost::function<ResponsePtr()> ResCreateFunction;

  ServiceCallbackHelperT(const Callback& callback, 
                         const ReqCreateFunction& create_req = 
                         // these static casts are legally unnecessary, but
                         // here to keep clang 2.8 from getting confused
                         static_cast<RequestPtr(*)()>(defaultServiceCreateFunction<RequestType>), 
                         const ResCreateFunction& create_res = 
                         static_cast<ResponsePtr(*)()>(defaultServiceCreateFunction<ResponseType>))
  : callback_(callback)
  , create_req_(create_req)
  , create_res_(create_res)
  {
  }

  virtual bool call(ServiceCallbackHelperCallParams& params)
  {
    namespace ser = serialization;
    RequestPtr req(create_req_());
    ResponsePtr res(create_res_());

    ser::deserializeMessage(params.request, *req);

    ServiceSpecCallParams<RequestType, ResponseType> call_params;
    call_params.request = req;
    call_params.response = res;
    call_params.connection_header = params.connection_header;
    bool ok = Spec::call(callback_, call_params);
    params.response = ser::serializeServiceResponse(ok, *res);
    return ok;
  }

private:
  Callback callback_;
  ReqCreateFunction create_req_;
  ResCreateFunction create_res_;
};

}

#endif