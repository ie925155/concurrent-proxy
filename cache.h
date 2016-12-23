/**
 * File: cache.h
 * -------------
 * Defines a class to help manage an
 * HTTP response cache.
 */

#ifndef _http_cache_
#define _http_cache_

#include <string>
#include <memory>
#include "request.h"
#include "response.h"

class HTTPCache {
 public:

/**
 * Constructs the HTTPCache object.
 */

  HTTPCache();

/**
 * The following three functions do what you'd expect, except that they
 * aren't thread safe.  In a MT environment, you should acquire the lock
 * on the relevant response before calling these.
 */
  bool containsCacheEntry(const HTTPRequest& request, HTTPResponse& response) const;
  bool shouldCache(const HTTPRequest& request, const HTTPResponse& response) const;
  void cacheEntry(const HTTPRequest& request, const HTTPResponse& response);

 private:
  std::string hashRequest(const HTTPRequest& request) const;
  std::string serializeRequest(const HTTPRequest& request) const;
  bool cacheEntryExists(const std::string& filename) const;
  std::string getRequestHashCacheEntryName(const std::string& requestHash) const;
  void ensureDirectoryExists(const std::string& directory, bool empty = false) const;
  std::string getExpirationTime(int ttl) const;
  bool cachedEntryIsValid(const std::string& cachedFileName) const;

  std::string cacheDirectory;
};

#endif
