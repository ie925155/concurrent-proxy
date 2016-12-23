/**
 * File: http-proxy-scheduler.h
 * ----------------------------
 * This class defines the scheduler class, which takes all
 * proxied requests off of the main thread and schedules them to
 * be handled by a constant number of child threads.
 */

#ifndef _http_proxy_scheduler_
#define _http_proxy_scheduler_

#include <string>
#include "request-handler.h"

class HTTPProxyScheduler {

 public:
  void scheduleRequest(int connectionfd, const std::string& clientIPAddress);

 private:
  HTTPRequestHandler handler;
};

#endif
