
#ifndef _ROBOTRPCDISPATCH_H_
#define _ROBOTRPCDISPATCH_H_


#if defined(_MSC_VER)
# pragma warning(disable:4786)    // identifier was truncated in debug info
#endif

#include "RobotRpcDecl.h"

#ifndef MAKEDEPEND
# include <list>
#endif

namespace RobotRpc {

  // An RPC source represents a file descriptor to monitor
  class RobotRpcSocketSource;

  // 监控 文件描述符 集合的触发的事件
  //! An object which monitors file descriptors for events and performs
  //! callbacks when interesting events happen.
  class ROBOTRPCPP_DECL RobotRpcDispatch {
  public:
    //! Constructor
    RobotRpcDispatch();
    ~RobotRpcDispatch();

    //! Values indicating the type of events a source is interested in
    enum EventType {
      ReadableEvent = 1,    //!< data available to read
      WritableEvent = 2,    //!< connected/data can be written without blocking
      Exception     = 4     //!< out-of-band data has arrived
    };
    
    //! Monitor this source for the event types specified by the event mask
    //! and call its event handler when any of the events occur.
    //!  @param source The source to monitor
    //!  @param eventMask Which event types to watch for. \see EventType
    void addSource(RobotRpcSocketSource* source, unsigned eventMask);

    //! Stop monitoring this source.
    //!  @param source The source to stop monitoring
    void removeSource(RobotRpcSocketSource* source);

    //! Modify the types of events to watch for on this source
    void setSourceEvents(RobotRpcSocketSource* source, unsigned eventMask);


    //! Watch current set of sources and process events for the specified
    //! duration (in ms, -1 implies wait forever, or until exit is called)
    void work(double msTime);

    //! Exit from work routine
    void exit();

    //! Clear all sources from the monitored sources list. Sources are closed.
    void clear();

    // helper returning current steady/monotonic time
    double getTime();

    // A source to monitor and what to monitor it for
    struct MonitoredSource {
      MonitoredSource(RobotRpcSocketSource* src, unsigned mask) : _src(src), _mask(mask) {}
      RobotRpcSocketSource* getSource() const { return _src; }
      unsigned& getMask() { return _mask; }
      RobotRpcSocketSource* _src;
      unsigned _mask;
    };

    // A list of sources to monitor
    typedef std::list< MonitoredSource > SourceList; 

    // Sources being monitored
    SourceList _sources;
  protected:

    // When work should stop (-1 implies wait forever, or until exit is called)
    double _endTime;

    bool _doClear;
    bool _inWork;

  };
} // namespace RobotRpc

#endif  // _ROBOTRPCDISPATCH_H_
