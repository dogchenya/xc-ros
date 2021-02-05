#ifndef _XCROSCPP_SERVICE_HANDLE_H_
#define _XCROSCPP_SERVICE_HANDLE_H_

#include "ros/forwards.h"
#include "ros/xcroscpp_common.h"

namespace xcros
{

/**
 * \brief Manages an service advertisement.
 *
 * A ServiceServer should always be created through a call to NodeHandle::advertiseService(), or copied from
 * one that was.  Once all copies of a specific
 * ServiceServer go out of scope, the service associated with it will be unadvertised and the service callback
 * will stop being called.
 */
class ROSCPP_DECL ServiceServer
{
public:
  ServiceServer() {}
  ServiceServer(const ServiceServer& rhs);
  ~ServiceServer();

  /**
   * \brief Unadvertise the service associated with this ServiceServer
   *
   * This method usually does not need to be explicitly called, as automatic shutdown happens when
   * all copies of this ServiceServer go out of scope
   *
   * This method overrides the automatic reference counted unadvertise, and immediately
   * unadvertises the service associated with this ServiceServer
   */
  void shutdown();

  std::string getService() const;

  operator void*() const { return (impl_ && impl_->isValid()) ? (void*)1 : (void*)0; }

  bool operator<(const ServiceServer& rhs) const
  {
    return impl_ < rhs.impl_;
  }

  bool operator==(const ServiceServer& rhs) const
  {
    return impl_ == rhs.impl_;
  }

  bool operator!=(const ServiceServer& rhs) const
  {
    return impl_ != rhs.impl_;
  }

private:
  ServiceServer(const std::string& service, const NodeHandle& node_handle);

  class Impl
  {
  public:
    Impl();
    ~Impl();

    void unadvertise();
    bool isValid() const;

    std::string service_;
    NodeHandlePtr node_handle_;
    bool unadvertised_;
  };
  typedef boost::shared_ptr<Impl> ImplPtr;
  typedef boost::weak_ptr<Impl> ImplWPtr;

  ImplPtr impl_;

  friend class NodeHandle;
  friend class NodeHandleBackingCollection;
};
typedef std::vector<ServiceServer> V_ServiceServer;

}

#endif