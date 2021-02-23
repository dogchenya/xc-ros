#ifndef _XCROSCPP_TRANSPORT_H_
#define _XCROSCPP_TRANSPORT_H_

#include <ros/types.h>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <vector>

namespace xcros
{

class Transport;
typedef boost::shared_ptr<Transport> TransportPtr;

class Header;

class Transport : public boost::enable_shared_from_this<Transport>
{

public:
    Transport();
    virtual ~Transport() {}

    virtual int32_t read(uint8_t* buffer, uint32_t size) = 0;

    virtual int32_t write(uint8_t* buffer, uint32_t size) = 0;

    /**
     * \brief Enable writing on this transport.  Allows derived classes to, for example, enable write polling for asynchronous sockets
     */
    virtual void enableWrite() = 0;
    /**
     * \brief Disable writing on this transport.  Allows derived classes to, for example, disable write polling for asynchronous sockets
     */
    virtual void disableWrite() = 0;

    /**
     * \brief Enable reading on this transport.  Allows derived classes to, for example, enable read polling for asynchronous sockets
     */
    virtual void enableRead() = 0;
    /**
     * \brief Disable reading on this transport.  Allows derived classes to, for example, disable read polling for asynchronous sockets
     */
    virtual void disableRead() = 0;

    /**
     * \brief Close this transport.  Once this call has returned, writing on this transport should always return an error.
     */
    virtual void close() = 0;

    virtual const char* getType() = 0;

    typedef boost::function<void(const TransportPtr&)> Callback;
    /**
     * \brief Set the function to call when this transport has disconnected, either through a call to close(). Or a disconnect from the remote host.
     */
    void setDisconnectCallback(const Callback& cb) { disconnect_cb_ = cb; }
    /**
     * \brief Set the function to call when there is data available to be read by this transport
     */
    void setReadCallback(const Callback& cb) { read_cb_ = cb; }
    /**
     * \brief Set the function to call when there is space available to write on this transport
     */
    void setWriteCallback(const Callback& cb) { write_cb_ = cb; }

    /**
     * \brief Returns a string description of both the type of transport and who the transport is connected to
     */
    virtual std::string getTransportInfo() = 0;

    /**
     * \brief Returns a boolean to indicate if the transport mechanism is reliable or not
     */
    virtual bool requiresHeader() {return true;}

    /**
     * \brief Provides an opportunity for transport-specific options to come in through the header
     */
    virtual void parseHeader(const Header& header) { (void)header; }

protected:
    Callback disconnect_cb_;
    Callback read_cb_;
    Callback write_cb_;

    /**
     * \brief returns true if the transport is allowed to connect to the host passed to it.
     */
    bool isHostAllowed(const std::string &host) const;

    /**
     * \brief returns true if this transport is only allowed to talk to localhost
     */
    bool isOnlyLocalhostAllowed() const { return only_localhost_allowed_; }

    private:
    bool only_localhost_allowed_;
    std::vector<std::string> allowed_hosts_;
};
} // namespace xcros



#endif