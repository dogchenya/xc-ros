
#include "RobotRpcSocketSource.h"
#include "RobotRpcSocket.h"
#include "RobotRpcUtil.h"

namespace RobotRpc {


  RobotRpcSocketSource::RobotRpcSocketSource(int fd /*= -1*/, bool deleteOnClose /*= false*/) 
    : _fd(fd), _deleteOnClose(deleteOnClose), _keepOpen(false)
  {
  }

  RobotRpcSocketSource::~RobotRpcSocketSource()
  {
  }


  void
  RobotRpcSocketSource::close()
  {
    if (_fd != -1) {
      RobotRpcUtil::log(2,"RobotRpcSocketSource::close: closing socket %d.", _fd);
      RobotRpcSocket::close(_fd);
      RobotRpcUtil::log(2,"RobotRpcSocketSource::close: done closing socket %d.", _fd);
      _fd = -1;
    }
    if (_deleteOnClose) {
      RobotRpcUtil::log(2,"RobotRpcSocketSource::close: deleting this");
      _deleteOnClose = false;
      delete this;
    }
  }

} // namespace RobotRpc
