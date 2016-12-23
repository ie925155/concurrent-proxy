/**
 * File: request-handler.cc
 * ------------------------
 * Provides the implementation for the HTTPRequestHandler class.
 */

#include "request-handler.h"
#include "response.h"

#include <iostream>              // for flush
#include <string>                // for string
#include "socket++/sockstream.h" // for sockbuf, iosockstream
using namespace std;

/**
 * Services a request by posting a placeholder payload.  The implementation
 * of this method will change dramatically as you work toward your final milestone.
 */

void HTTPRequestHandler::serviceRequest(const pair<int, string>& connection) throw() {
  HTTPResponse response;
  response.setResponseCode(200);
  response.setProtocol("HTTP/1.0");
  response.setPayload("Your IP address is " + connection.second + ".");
  sockbuf sb(connection.first);
  iosockstream ss(&sb);
  ss << response << flush;
}
