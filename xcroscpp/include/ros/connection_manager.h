#include "forwards.h"
#include "connection.h"
#include "ros/xcroscpp_common.h"

#include <boost/thread/mutex.hpp>
#include <boost/signals2/connection.hpp>


namespace xcros
{

class PollManager;
typedef boost::shared_ptr<PollManager> PollManagerPtr;

class ConnectionManager;
typedef boost::shared_ptr<ConnectionManager> ConnectionManagerPtr;

class ROSCPP_DECL ConnectionManager
{
public:
  static const ConnectionManagerPtr& instance();

  ConnectionManager();
  ~ConnectionManager();

  /** @brief Get a new connection ID
   */
  uint32_t getNewConnectionID();

  /** @brief Add a connection to be tracked by the node.  Will automatically remove them if they've been dropped, but from inside the ros thread
   *
   * @param The connection to add
   */
  void addConnection(const ConnectionPtr& connection);

  void clear(Connection::DropReason reason);

  uint32_t getTCPPort();
  uint32_t getUDPPort();

  const TransportTCPPtr& getTCPServerTransport() { return tcpserver_transport_; }
  const TransportUDPPtr& getUDPServerTransport() { return udpserver_transport_; }

  void udprosIncomingConnection(const TransportUDPPtr& transport, Header& header);

  void start();
  void shutdown();

private:
  void onConnectionDropped(const ConnectionPtr& conn);
  // Remove any dropped connections from our list, causing them to be destroyed
  // They can't just be removed immediately when they're dropped because the ros
  // thread may still be using them (or more likely their transport)
  void removeDroppedConnections();

  bool onConnectionHeaderReceived(const ConnectionPtr& conn, const Header& header);
  void tcprosAcceptConnection(const TransportTCPPtr& transport);

  PollManagerPtr poll_manager_;

  S_Connection connections_;
  V_Connection dropped_connections_;
  boost::mutex connections_mutex_;
  boost::mutex dropped_connections_mutex_;

  // The connection ID counter, used to assign unique ID to each inbound or
  // outbound connection.  Access via getNewConnectionID()
  uint32_t connection_id_counter_;
  boost::mutex connection_id_counter_mutex_;

  boost::signals2::connection poll_conn_;

  TransportTCPPtr tcpserver_transport_;
  TransportUDPPtr udpserver_transport_;

  const static int MAX_TCPROS_CONN_QUEUE = 100; // magic
};

}