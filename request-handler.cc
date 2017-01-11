/**
 * File: request-handler.cc
 * ------------------------
 * Provides the implementation for the HTTPRequestHandler class.
 */

#include <iostream>              // for flush
#include <string>                // for string
#include <netdb.h>                // for gethostbyname
#include <sys/socket.h>           // for socket, AF_INET
#include <sys/types.h>            // for SOCK_STREAM
#include <unistd.h>               // for close

#include "request-handler.h"
#include "request.h"
#include "response.h"
#include "ostreamlock.h"

using namespace std;

/**
 * Services a request by posting a placeholder payload.  The implementation
 * of this method will change dramatically as you work toward your final milestone.
 */

const int kClientSocketError = -1;
const int kBadRequest = 400;
const int kForbiddenRequest = 403;
#define TMPLEN 8192

HTTPRequestHandler::HTTPRequestHandler(void)
 : blacklist("blocked-domains.txt") {}

static int createClientSocket(const string& host, unsigned short port) {
  struct hostent hbuf, *hp; /* output DNS host entry */
  char tmp[TMPLEN];         /* temporary scratch buffer */
  int my_h_errno, rc;       /* DNS error code and return code */
  rc = gethostbyname_r(host.c_str(), &hbuf, tmp, TMPLEN, &hp, &my_h_errno);
  if (rc != 0) {
    cout << "gethostbyname_r error: " << hstrerror(my_h_errno) << endl;
  }
  //cout << "official hostname: " <<  hp->h_name << endl;
  int s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) return kClientSocketError;

  struct sockaddr_in serverAddress;
  memset(&serverAddress, 0, sizeof(serverAddress));
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(port);
  serverAddress.sin_addr.s_addr =
    ((struct in_addr *)hbuf.h_addr)->s_addr;

  if (connect(s, (struct sockaddr *) &serverAddress,
	      sizeof(serverAddress)) != 0) {
    close(s);
    return kClientSocketError;
  }

  return s;
}

bool HTTPRequestHandler::ingestRequest(const string& clientIPAddress, 
iosockstream &client_stream, HTTPRequest &request, HTTPResponse &response){
  try {
    request.ingestRequestLine(client_stream);
    /*cout << oslock << request.getMethod() << " " << request.getURL() <<  " "
       << request.getServer() << " " << request.getProtocol() << " "
       << request.getPort() << " " << request.getPath() << endl << osunlock;*/
  } catch(HTTPBadRequestException exception){
    response.setPayload(exception.what());
 	  response.setResponseCode(kBadRequest);
    return false;
  }
  if(!blacklist.serverIsAllowed(request.getServer())){
    response.setPayload("Forbidden Content");
 	  response.setResponseCode(kForbiddenRequest);
	  return false;
  };
  request.ingestHeader(client_stream, clientIPAddress);
  request.ingestPayload(client_stream);
  if(cache.containsCacheEntry_r(request, response)){
	  cout << oslock << "contain cache entry" << endl << osunlock;
	  return false;
  }
  return true;
}

void HTTPRequestHandler::ingestResponse(iosockstream &server_stream, 
  HTTPRequest &request, HTTPResponse &response)
{
  response.ingestResponseHeader(server_stream);
  response.ingestPayload(server_stream);
  if(cache.shouldCache(request, response)){
	  cout << oslock << "cache entry" << endl << osunlock;
	  cache.cacheEntry_r(request, response);
  }
}

void HTTPRequestHandler::serviceRequest(const pair<int, string>& connection)
	throw() {
  sockbuf sb(connection.first);
  iosockstream client_stream(&sb);
  HTTPRequest request;
  HTTPResponse response;
  int client_fd;
  if(!ingestRequest(connection.second, client_stream, request, response)){
 	  client_stream << response << flush;
    return;
  }
  client_fd = createClientSocket(request.getServer(), request.getPort());
  if(client_fd == kClientSocketError) {
      cerr << oslock << "can not open a client socket" << endl << osunlock;
      return;
  }
  sockbuf sb2(client_fd);
  iosockstream server_stream(&sb2);
  //send request to original server
  server_stream << request << flush;
  ingestResponse(server_stream, request, response);
  client_stream << response << flush;
}
