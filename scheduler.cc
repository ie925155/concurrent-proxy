/**
 * File: scheduler.cc
 * ------------------
 * Presents the implementation of the HTTPProxyScheduler class.
 */

#include <utility>      // for make_pair
#include <iostream>
#include "scheduler.h"
using namespace std;

HTTPProxyScheduler::HTTPProxyScheduler()
  : thread_pool(20)
{
  cout << "HTTPProxyScheduler constructor called" << endl;
}
void HTTPProxyScheduler::scheduleRequest(int connectionfd,
  const string& clientIPAddress) {
  thread_pool.schedule([this, connectionfd, clientIPAddress] () -> void {
      handler.serviceRequest(make_pair(connectionfd, clientIPAddress));
    });
}
