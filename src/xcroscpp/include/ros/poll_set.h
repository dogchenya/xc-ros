#ifndef _XCROS_POLL_SET_H_
#define _XCROS_POLL_SET_H_

#include <vector>
#include <memory>
#include "ros/io.h"
#include "ros/xcroscpp_common.h"
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>
#include "ros/forwards.h"

namespace xcros
{
    // class Transport;
    // typedef boost::shared_ptr<Transport> TransportPtr;

    class ROSCPP_DECL PollSet
    {
    public:
    PollSet();
    ~PollSet();

    typedef boost::function<void(int)> SocketUpdateFunc;
    /**
     * \brief Add a socket.
     *
     * addSocket() may be called from any thread.
     * \param sock The socket to add
     * \param update_func The function to call when a socket has events
     * \param transport The (optional) transport associated with this socket. Mainly
     * used to prevent the transport from being deleted while we're calling the update function
     */
    bool addSocket(int sock, const SocketUpdateFunc& update_func, const TransportPtr& transport = TransportPtr());
    /**
     * \brief Delete a socket
     *
     * delSocket() may be called from any thread.
     * \param sock The socket to delete
     */
    bool delSocket(int sock);

    /**
     * \brief Add events to be polled on a socket
     *
     * addEvents() may be called from any thread.
     * \param sock The socket to add events to
     * \param events The events to add
     */
    bool addEvents(int sock, int events);
    /**
     * \brief Delete events to be polled on a socket
     *
     * delEvents() may be called from any thread.
     * \param sock The socket to delete events from
     * \param events The events to delete
     */
    bool delEvents(int sock, int events);

    /**
     * \brief Process all socket events
     *
     * This function will actually call poll() on the available sockets, and allow
     * them to do their processing.
     *
     * update() may only be called from one thread at a time
     *
     * \param poll_timeout The time, in milliseconds, for the poll() call to timeout after
     * if there are no events.  Note that this does not provide an upper bound for the entire
     * function, just the call to poll()
     */
    void update(int poll_timeout);

    /**
     * \brief Signal our poll() call to finish if it's blocked waiting (see the poll_timeout
     * option for update()).
     */
    void signal();

    private:
    /**
     * \brief Creates the native pollset for our sockets, if any have changed
     */
    void createNativePollset();

    /**
     * \brief Called when events have been triggered on our signal pipe
     */
    void onLocalPipeEvents(int events);

    struct SocketInfo
    {
        TransportPtr transport_;
        SocketUpdateFunc func_;
        int fd_;
        int events_;
    };
    typedef std::map<int, SocketInfo> M_SocketInfo;
    M_SocketInfo socket_info_;
    boost::mutex socket_info_mutex_;
    bool sockets_changed_;

    boost::mutex just_deleted_mutex_;
    typedef std::vector<int> V_int;
    V_int just_deleted_;

    std::vector<socket_pollfd> ufds_;

    boost::mutex signal_mutex_;
    signal_fd_t signal_pipe_[2];

    int epfd_;
    };
    
} // namespace xcros



#endif 