/**
 * File: scheduler.cc
 * ------------------
 * Presents the implementation of the HTTPProxyScheduler class.
 */

#include <utility>      // for make_pair
#include "scheduler.h"
using namespace std;

void HTTPProxyScheduler::scheduleRequest(int connectionfd, const string& clientIPAddress) {
  handler.serviceRequest(make_pair(connectionfd, clientIPAddress));
}
