
#include "RobotRpcServerConnection.h"

#include "RobotRpcSocket.h"
#include "RobotRpc.h"
#include "RobotRpcUtil.h"
#include "RobotRpcDispatch.h"
#include "RobotRpcException.h"
#ifndef MAKEDEPEND
# include <stdio.h>
# include <stdlib.h>
#ifndef _WINDOWS
	# include <strings.h>
#endif
# include <string.h>
#endif

using namespace RobotRpc;

// Static data
const char RobotRpcServerConnection::METHODNAME_TAG[] = "<methodName>";
const char RobotRpcServerConnection::PARAMS_TAG[] = "<params>";
const char RobotRpcServerConnection::PARAMS_ETAG[] = "</params>";
const char RobotRpcServerConnection::PARAM_TAG[] = "<param>";
const char RobotRpcServerConnection::PARAM_ETAG[] = "</param>";

const std::string RobotRpcServerConnection::SYSTEM_MULTICALL = "system.multicall";
const std::string RobotRpcServerConnection::METHODNAME = "methodName";
const std::string RobotRpcServerConnection::PARAMS = "params";

const std::string RobotRpcServerConnection::FAULTCODE = "faultCode";
const std::string RobotRpcServerConnection::FAULTSTRING = "faultString";



// The server delegates handling client requests to a serverConnection object.
RobotRpcServerConnection::RobotRpcServerConnection(int fd, RobotRpcServer* server, bool deleteOnClose /*= false*/) :
  RobotRpcSocketSource(fd, deleteOnClose)
{
  RobotRpcUtil::log(2,"RobotRpcServerConnection: new socket %d.", fd);
  _server = server;
  _connectionState = READ_HEADER;
  _contentLength = 0;
  _bytesWritten = 0;
  _keepAlive = true;
}


RobotRpcServerConnection::~RobotRpcServerConnection()
{
  RobotRpcUtil::log(4,"RobotRpcServerConnection dtor.");
  _server->removeConnection(this);
}


// Handle input on the server socket by accepting the connection
// and reading the rpc request. Return true to continue to monitor
// the socket for events, false to remove it from the dispatcher.
unsigned
RobotRpcServerConnection::handleEvent(unsigned /*eventType*/)
{
  if (_connectionState == READ_HEADER)
    if ( ! readHeader()) return 0;

  if (_connectionState == READ_REQUEST)
    if ( ! readRequest()) return 0;

  if (_connectionState == WRITE_RESPONSE)
    if ( ! writeResponse()) return 0;

  return (_connectionState == WRITE_RESPONSE) 
        ? RobotRpcDispatch::WritableEvent : RobotRpcDispatch::ReadableEvent;
}


bool
RobotRpcServerConnection::readHeader()
{
  // Read available data
  bool eof;
  if ( ! RobotRpcSocket::nbRead(this->getfd(), _header, &eof)) {
    // Its only an error if we already have read some data
    if (_header.length() > 0)
      RobotRpcUtil::error("RobotRpcServerConnection::readHeader: error while reading header (%s).",RobotRpcSocket::getErrorMsg().c_str());
    return false;
  }

  RobotRpcUtil::log(4, "RobotRpcServerConnection::readHeader: read %d bytes.", _header.length());
  char *hp = (char*)_header.c_str();  // Start of header
  char *ep = hp + _header.length();   // End of string
  char *bp = 0;                       // Start of body
  char *lp = 0;                       // Start of content-length value
  char *kp = 0;                       // Start of connection value

  for (char *cp = hp; (bp == 0) && (cp < ep); ++cp) {
	if ((ep - cp > 16) && (strncasecmp(cp, "Content-length: ", 16) == 0))
	  lp = cp + 16;
	else if ((ep - cp > 12) && (strncasecmp(cp, "Connection: ", 12) == 0))
	  kp = cp + 12;
	else if ((ep - cp > 4) && (strncmp(cp, "\r\n\r\n", 4) == 0))
	  bp = cp + 4;
	else if ((ep - cp > 2) && (strncmp(cp, "\n\n", 2) == 0))
	  bp = cp + 2;
  }

  // If we haven't gotten the entire header yet, return (keep reading)
  if (bp == 0) {
    // EOF in the middle of a request is an error, otherwise its ok
    if (eof) {
      RobotRpcUtil::log(4, "RobotRpcServerConnection::readHeader: EOF");
      if (_header.length() > 0)
        RobotRpcUtil::error("RobotRpcServerConnection::readHeader: EOF while reading header");
      return false;   // Either way we close the connection
    }
    
    return true;  // Keep reading
  }

  // Decode content length
  if (lp == 0) {
    RobotRpcUtil::error("RobotRpcServerConnection::readHeader: No Content-length specified");
    return false;   // We could try to figure it out by parsing as we read, but for now...
  }

  // avoid overly large or improperly formatted content-length
  long int clength = 0;
  clength = strtol(lp, NULL, 10);
  if ((clength < 0) || (clength > __INT_MAX__)) {
    RobotRpcUtil::error("RobotRpcServerConnection::readHeader: Invalid Content-length specified.");
    return false;
  }
  _contentLength = int(clength);
  	
  RobotRpcUtil::log(3, "RobotRpcServerConnection::readHeader: specified content length is %d.", _contentLength);

  // Otherwise copy non-header data to request buffer and set state to read request.
  _request = bp;

  // Parse out any interesting bits from the header (HTTP version, connection)
  _keepAlive = true;
  if (_header.find("HTTP/1.0") != std::string::npos) {
    if (kp == 0 || strncasecmp(kp, "keep-alive", 10) != 0)
      _keepAlive = false;           // Default for HTTP 1.0 is to close the connection
  } else {
    if (kp != 0 && strncasecmp(kp, "close", 5) == 0)
      _keepAlive = false;
  }
  RobotRpcUtil::log(3, "KeepAlive: %d", _keepAlive);


  _header = ""; 
  _connectionState = READ_REQUEST;
  return true;    // Continue monitoring this source
}

bool
RobotRpcServerConnection::readRequest()
{
  // If we dont have the entire request yet, read available data
  if (int(_request.length()) < _contentLength) {
    bool eof;
    if ( ! RobotRpcSocket::nbRead(this->getfd(), _request, &eof)) {
      RobotRpcUtil::error("RobotRpcServerConnection::readRequest: read error (%s).",RobotRpcSocket::getErrorMsg().c_str());
      return false;
    }
    // Avoid an overly large request
    if (_request.length() > size_t(__INT_MAX__)) {
      RobotRpcUtil::error("RobotRpcServerConnection::readRequest: request length (%u) exceeds the maximum allowed size (%u)",
                        _request.length(), __INT_MAX__);
      _request.resize(__INT_MAX__);
      return false;
    }

    // If we haven't gotten the entire request yet, return (keep reading)
    if (int(_request.length()) < _contentLength) {
      if (eof) {
        RobotRpcUtil::error("RobotRpcServerConnection::readRequest: EOF while reading request");
        return false;   // Either way we close the connection
      }
      return true;
    }
  }

  // Otherwise, parse and dispatch the request
  RobotRpcUtil::log(3, "RobotRpcServerConnection::readRequest read %d bytes.", _request.length());
  //RobotRpcUtil::log(5, "RobotRpcServerConnection::readRequest:\n%s\n", _request.c_str());

  _connectionState = WRITE_RESPONSE;

  return true;    // Continue monitoring this source
}


bool
RobotRpcServerConnection::writeResponse()
{
  if (_response.length() == 0) {
    executeRequest();
    _bytesWritten = 0;
    if (_response.length() == 0) {
      RobotRpcUtil::error("RobotRpcServerConnection::writeResponse: empty response.");
      return false;
    }
  }

  // Try to write the response
  if ( ! RobotRpcSocket::nbWrite(this->getfd(), _response, &_bytesWritten)) {
    RobotRpcUtil::error("RobotRpcServerConnection::writeResponse: write error (%s).",RobotRpcSocket::getErrorMsg().c_str());
    return false;
  }
  RobotRpcUtil::log(3, "RobotRpcServerConnection::writeResponse: wrote %d of %d bytes.", _bytesWritten, _response.length());

  // Prepare to read the next request
  if (_bytesWritten == int(_response.length())) {
    _header = "";
    _request = "";
    _response = "";
    _connectionState = READ_HEADER;
  }

  return _keepAlive;    // Continue monitoring this source if true
}

// Run the method, generate _response string
void
RobotRpcServerConnection::executeRequest()
{
  XmlRpcValue params, resultValue;
  std::string methodName = parseRequest(params);
  RobotRpcUtil::log(2, "RobotRpcServerConnection::executeRequest: server calling method '%s'", 
                    methodName.c_str());

  try {

    if ( ! executeMethod(methodName, params, resultValue) &&
         ! executeMulticall(methodName, params, resultValue))
      generateFaultResponse(methodName + ": unknown method name");
    else
      generateResponse(resultValue.toXml());

  } catch (const RobotRpcException& fault) {
    RobotRpcUtil::log(2, "RobotRpcServerConnection::executeRequest: fault %s.",
                    fault.getMessage().c_str()); 
    generateFaultResponse(fault.getMessage(), fault.getCode());
  }
}

// Parse the method name and the argument values from the request.
std::string
RobotRpcServerConnection::parseRequest(XmlRpcValue& params)
{
  int offset = 0;   // Number of chars parsed from the request

  std::string methodName = RobotRpcUtil::parseTag(METHODNAME_TAG, _request, &offset);

  if (methodName.size() > 0 && RobotRpcUtil::findTag(PARAMS_TAG, _request, &offset))
  {
    int nArgs = 0;
    while (RobotRpcUtil::nextTagIs(PARAM_TAG, _request, &offset)) {
      params[nArgs++] = XmlRpcValue(_request, &offset);
      (void) RobotRpcUtil::nextTagIs(PARAM_ETAG, _request, &offset);
    }

    (void) RobotRpcUtil::nextTagIs(PARAMS_ETAG, _request, &offset);
  }

  return methodName;
}

// Execute a named method with the specified params.
bool
RobotRpcServerConnection::executeMethod(const std::string& methodName, 
                                      XmlRpcValue& params, XmlRpcValue& result)
{
  RobotRpcServerMethod* method = _server->findMethod(methodName);

  if ( ! method) return false;

  method->execute(params, result);

  // Ensure a valid result value
  if ( ! result.valid())
      result = std::string();

  return true;
}

// Execute multiple calls and return the results in an array.
bool
RobotRpcServerConnection::executeMulticall(const std::string& methodName, 
                                         XmlRpcValue& params, XmlRpcValue& result)
{
  if (methodName != SYSTEM_MULTICALL) return false;

  // There ought to be 1 parameter, an array of structs
  if (params.size() != 1 || params[0].getType() != XmlRpcValue::TypeArray)
    throw RobotRpcException(SYSTEM_MULTICALL + ": Invalid argument (expected an array)");

  int nc = params[0].size();
  result.setSize(nc);

  for (int i=0; i<nc; ++i) {

    if ( ! params[0][i].hasMember(METHODNAME) ||
         ! params[0][i].hasMember(PARAMS)) {
      result[i][FAULTCODE] = -1;
      result[i][FAULTSTRING] = SYSTEM_MULTICALL +
              ": Invalid argument (expected a struct with members methodName and params)";
      continue;
    }

    const std::string& methodName = params[0][i][METHODNAME];
    XmlRpcValue& methodParams = params[0][i][PARAMS];

    XmlRpcValue resultValue;
    resultValue.setSize(1);
    try {
      if ( ! executeMethod(methodName, methodParams, resultValue[0]) &&
           ! executeMulticall(methodName, params, resultValue[0]))
      {
        result[i][FAULTCODE] = -1;
        result[i][FAULTSTRING] = methodName + ": unknown method name";
      }
      else
        result[i] = resultValue;

    } catch (const RobotRpcException& fault) {
        result[i][FAULTCODE] = fault.getCode();
        result[i][FAULTSTRING] = fault.getMessage();
    }
  }

  return true;
}


// Create a response from results xml
void
RobotRpcServerConnection::generateResponse(std::string const& resultXml)
{
  const char RESPONSE_1[] = 
    "<?xml version=\"1.0\"?>\r\n"
    "<methodResponse><params><param>\r\n\t";
  const char RESPONSE_2[] =
    "\r\n</param></params></methodResponse>\r\n";

  std::string body = RESPONSE_1 + resultXml + RESPONSE_2;
  std::string header = generateHeader(body);

  // Avoid an overly large response
  if ((header.length() + body.length()) > size_t(__INT_MAX__)) {
    RobotRpcUtil::error("RobotRpcServerConnection::generateResponse: response length (%u) exceeds the maximum allowed size (%u).",
                      _response.length(), __INT_MAX__);
    _response = "";
  }
  else {
    _response = header + body;
    RobotRpcUtil::log(5, "RobotRpcServerConnection::generateResponse:\n%s\n", _response.c_str());
  }
}

// Prepend http headers
std::string
RobotRpcServerConnection::generateHeader(std::string const& body)
{
  std::string header = 
    "HTTP/1.1 200 OK\r\n"
    "Server: ";
  header += ROBOTRPC_VERSION;
  header += "\r\n"
    "Content-Type: text/xml\r\n"
    "Content-length: ";

  char buffLen[40];
#ifdef _MSC_VER
  sprintf_s(buffLen,40,"%d\r\n\r\n", (int)body.size());
#else
  sprintf(buffLen,"%d\r\n\r\n", (int)body.size());
#endif

  return header + buffLen;
}


void
RobotRpcServerConnection::generateFaultResponse(std::string const& errorMsg, int errorCode)
{
  const char RESPONSE_1[] = 
    "<?xml version=\"1.0\"?>\r\n"
    "<methodResponse><fault>\r\n\t";
  const char RESPONSE_2[] =
    "\r\n</fault></methodResponse>\r\n";

  XmlRpcValue faultStruct;
  faultStruct[FAULTCODE] = errorCode;
  faultStruct[FAULTSTRING] = errorMsg;
  std::string body = RESPONSE_1 + faultStruct.toXml() + RESPONSE_2;
  std::string header = generateHeader(body);

  _response = header + body;
}

