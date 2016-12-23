/**
 * File: http-blacklist.h
 * ----------------------
 * Defines a class that helper identify hosts
 * that should be blocked out by our http-proxy.
 */

#ifndef _http_blacklist_
#define _http_blacklist_

#include <vector>
#include <string>
#include <regex>

#include "proxy-exception.h"

class HTTPBlacklist {
 public:
/**
 * Constructor: HTTPBlacklist
 * --------------------------
 * Constructs an HTTPBlacklist object based on information
 * in the identified file.  The file contents should be
 * a list of regular expressions--one per line--describing
 * a class of server strings that should be blocked out.
 *
 * The file might, for instance, look like this:
 *
 *   (.*)\.berkeley.edu
 *   (.*)\.bing.com
 *   (.*)\.microsoft.com
 *   (.*)\.org
 *
 */

  HTTPBlacklist(const std::string& filename) throw (HTTPProxyException);

/**
 * Method: serverIsAllowed
 * -----------------------
 * Returns true if and only if access to the the
 * identified server (i.e www.facebook.com) is permitted.
 */

  bool serverIsAllowed(const std::string& server) const;

 private:
  std::vector<std::regex> blockedDomains;
};

#endif
