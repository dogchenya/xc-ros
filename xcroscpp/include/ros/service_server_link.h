#ifndef _XCROSCPP_SERVICE_SERVER_LINK_H_
#define _XCROSCPP_SERVICE_SERVER_LINK_H_

#include "ros/xcroscpp_common.h"

#include <boost/thread/mutex.hpp>
#include <boost/shared_array.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>

#include <queue>

namespace xcros
{
class Header;
class Message;
class Connection;
typedef boost::shared_ptr<Connection> ConnectionPtr;

/**
 * \brief Handles a connection to a service.  If it's a non-persistent client, automatically disconnects
 * when its first service call has finished.
 */
class ROSCPP_DECL ServiceServerLink : public boost::enable_shared_from_this<ServiceServerLink>
{
private:
  struct CallInfo
  {
    SerializedMessage req_;
    SerializedMessage* resp_;

    bool finished_;
    boost::condition_variable finished_condition_;
    boost::mutex finished_mutex_;
    boost::thread::id caller_thread_id_;

    bool success_;
    bool call_finished_;

    std::string exception_string_;
  };
  typedef boost::shared_ptr<CallInfo> CallInfoPtr;
  typedef std::queue<CallInfoPtr> Q_CallInfo;

public:
  typedef std::map<std::string, std::string> M_string;
  ServiceServerLink(const std::string& service_name, bool persistent, const std::string& request_md5sum, const std::string& response_md5sum, const M_string& header_values);
  virtual ~ServiceServerLink();

  //
  bool initialize(const ConnectionPtr& connection);

  /**
   * \brief Returns whether this client is still valid, ie. its connection has not been dropped
   */
  bool isValid() const;
  /**
   * \brief Returns whether this is a persistent connection
   */
  bool isPersistent() const { return persistent_; }

  const ConnectionPtr& getConnection() const { return connection_; }

  const std::string& getServiceName() const { return service_name_; }
  const std::string& getRequestMD5Sum() const { return request_md5sum_; }
  const std::string& getResponseMD5Sum() const { return response_md5sum_; }

  /**
   * \brief Blocking call the service this client is connected to
   *
   * If there is already a call happening in another thread, this will queue up the call and still block until
   * it has finished.
   */
  bool call(const SerializedMessage& req, SerializedMessage& resp);

private:
  void onConnectionDropped(const ConnectionPtr& conn);
  bool onHeaderReceived(const ConnectionPtr& conn, const Header& header);

  /**
   * \brief Called when the currently queued call has finished.  Clears out the current call, notifying it that it
   * has finished, then calls processNextCall()
   */
  void callFinished();
  /**
   * \brief Pops the next call off the queue if one is available.  If this is a non-persistent connection and the queue is empty
   * it will also drop the connection.
   */
  void processNextCall();
  /**
   * \brief Clear all calls, notifying them that they've failed
   */
  void clearCalls();
  /**
   * \brief Cancel a queued call, notifying it that it has failed
   */
  void cancelCall(const CallInfoPtr& info);

  void onHeaderWritten(const ConnectionPtr& conn);
  void onRequestWritten(const ConnectionPtr& conn);
  void onResponseOkAndLength(const ConnectionPtr& conn, const boost::shared_array<uint8_t>& buffer, uint32_t size, bool success);
  void onResponse(const ConnectionPtr& conn, const boost::shared_array<uint8_t>& buffer, uint32_t size, bool success);

  ConnectionPtr connection_;
  std::string service_name_;
  bool persistent_;
  std::string request_md5sum_;
  std::string response_md5sum_;

  M_string extra_outgoing_header_values_;
  bool header_written_;
  bool header_read_;

  Q_CallInfo call_queue_;
  boost::mutex call_queue_mutex_;

  CallInfoPtr current_call_;

  bool dropped_;
};
typedef boost::shared_ptr<ServiceServerLink> ServiceServerLinkPtr;

} // namespace ros

#endif // _XCROSCPP_SERVICE_SERVER_LINK_H_