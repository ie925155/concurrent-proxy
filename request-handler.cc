/**
 * File: request-handler.cc
 * ------------------------
 * Provides the implementation for the HTTPRequestHandler class.
 */

#include "request-handler.h"
#include "request.h"
#include "response.h"

#include <iostream>              // for flush
#include <string>                // for string
#include <netdb.h>                // for gethostbyname
#include <sys/socket.h>           // for socket, AF_INET
#include <sys/types.h>            // for SOCK_STREAM
#include <unistd.h>               // for close
#include "socket++/sockstream.h" // for sockbuf, iosockstream

using namespace std;

/**
 * Services a request by posting a placeholder payload.  The implementation
 * of this method will change dramatically as you work toward your final milestone.
 */

const int kClientSocketError = -1;
const int kBadRequest = 400;
const int kForbiddenRequest = 403;

HTTPRequestHandler::HTTPRequestHandler(void)
 : blacklist("blocked-domains.txt") {}

static int createClientSocket(const string& host, unsigned short port) {

  struct hostent *he = gethostbyname(host.c_str());
  if (he == NULL) return kClientSocketError;

  int s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) return kClientSocketError;

  struct sockaddr_in serverAddress;
  memset(&serverAddress, 0, sizeof(serverAddress));
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(port);
  serverAddress.sin_addr.s_addr =
    ((struct in_addr *)he->h_addr)->s_addr;

  if (connect(s, (struct sockaddr *) &serverAddress,
	      sizeof(serverAddress)) != 0) {
    close(s);
    return kClientSocketError;
  }

  return s;
}


void HTTPRequestHandler::serviceRequest(const pair<int, string>& connection)
	throw() {
  sockbuf sb(connection.first);
  iosockstream client_stream(&sb);
  HTTPRequest request;
  HTTPResponse response;
  try {
      request.ingestRequestLine(client_stream);
      /*cout << request.getMethod() << " " << request.getURL() <<  " "
           << request.getServer() << " " << request.getProtocol() << " "
           << request.getPort() << " " << request.getPath() << endl;*/
  } catch(HTTPBadRequestException exception){
      response.setPayload(exception.what());
 	  response.setResponseCode(kBadRequest);
 	  client_stream << response << flush;
      return;
  }
  if(!blacklist.serverIsAllowed(request.getServer())){
      response.setPayload("Forbidden Content");
 	  response.setResponseCode(kForbiddenRequest);
 	  client_stream << response << flush;
	  return;
  };
  request.ingestHeader(client_stream, connection.second);
  request.ingestPayload(client_stream);
  int client_fd = createClientSocket(request.getServer(), request.getPort());
  if(client_fd == kClientSocketError) {
      cerr << "can not open a client socket" << endl;
      return;
  }
  sockbuf sb2(client_fd);
  iosockstream server_stream(&sb2);
  //send request to original server
  server_stream << request << flush;
  response.ingestResponseHeader(server_stream);
  response.ingestPayload(server_stream);
  client_stream << response << flush;
}
