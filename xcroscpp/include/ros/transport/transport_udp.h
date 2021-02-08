#ifndef _XCROSCPP_TRANSPORT_UDP_H_
#define _XCROSCPP_TRANSPORT_UDP_H_

#include <ros/types.h>
#include <ros/transport/transport.h>

#include <boost/thread/mutex.hpp>
#include "ros/io.h"
#include <ros/xcroscpp_common.h>

namespace xcros
{

class TransportUDP;
typedef boost::shared_ptr<TransportUDP> TransportUDPPtr;

class PollSet;

#define ROS_UDP_DATA0 0
#define ROS_UDP_DATAN 1
#define ROS_UDP_PING 2
#define ROS_UDP_ERR 3
typedef struct TransportUDPHeader {
  uint32_t connection_id_;
  uint8_t op_;
  uint8_t message_id_;
  uint16_t block_;
} TransportUDPHeader;

/**
 * \brief UDPROS transport
 */
class ROSCPP_DECL TransportUDP : public Transport
{
public:
  enum Flags
  {
    SYNCHRONOUS = 1<<0,
  };

  TransportUDP(PollSet* poll_set, int flags = 0, int max_datagram_size = 0);
  virtual ~TransportUDP();

  /**
   * \brief Connect to a remote host.
   * \param host The hostname/IP to connect to
   * \param port The port to connect to
   * \return Whether or not the connection was successful
   */
  bool connect(const std::string& host, int port, int conn_id);

  /**
   * \brief Returns the URI of the remote host
   */
  std::string getClientURI();

  /**
   * \brief Start a server socket and listen on a port
   * \param port The port to listen on
   */
  bool createIncoming(int port, bool is_server);
  /**
   * \brief Create a connection to a server socket.
   */
  TransportUDPPtr createOutgoing(std::string host, int port, int conn_id, int max_datagram_size);
  /**
   * \brief Returns the port this transport is listening on
   */
  int getServerPort() const {return server_port_;}

  // overrides from Transport
  virtual int32_t read(uint8_t* buffer, uint32_t size);
  virtual int32_t write(uint8_t* buffer, uint32_t size);

  virtual void enableWrite();
  virtual void disableWrite();
  virtual void enableRead();
  virtual void disableRead();

  virtual void close();

  virtual std::string getTransportInfo();

  virtual bool requiresHeader() {return false;}

  virtual const char* getType() {return "UDPROS";}

  int getMaxDatagramSize() const {return max_datagram_size_;}

private:
  /**
   * \brief Initializes the assigned socket -- sets it to non-blocking and enables reading
   */
  bool initializeSocket();

  /**
   * \brief Set the socket to be used by this transport
   * \param sock A valid UDP socket
   * \return Whether setting the socket was successful
   */
  bool setSocket(int sock);

  void socketUpdate(int events);

  socket_fd_t sock_;
  bool closed_;
  boost::mutex close_mutex_;

  bool expecting_read_;
  bool expecting_write_;

  bool is_server_;
  sockaddr_in server_address_;
  sockaddr_in local_address_;
  int server_port_;
  int local_port_;

  std::string cached_remote_host_;

  PollSet* poll_set_;
  int flags_;

  uint32_t connection_id_;
  uint8_t current_message_id_;
  uint16_t total_blocks_;
  uint16_t last_block_;

  uint32_t max_datagram_size_;

  uint8_t* data_buffer_;
  uint8_t* data_start_;
  uint32_t data_filled_;

  uint8_t* reorder_buffer_;
  uint8_t* reorder_start_;
  TransportUDPHeader reorder_header_;
  uint32_t reorder_bytes_;
};

}

#endif // _XCROSCPP_TRANSPORT_UDP_H_