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
#include <map>
#include <mutex>
#include "request.h"
#include "response.h"

class HTTPCache {
 public:

/**
 * Constructs the HTTPCache object.
 */

  HTTPCache();

  bool containsCacheEntry_r(const HTTPRequest& request, HTTPResponse& response);
  bool shouldCache(const HTTPRequest& request, const HTTPResponse& response) const;
  void cacheEntry_r(const HTTPRequest& request, const HTTPResponse& response);

 private:
  std::string hashRequest(const HTTPRequest& request) const;
  std::string serializeRequest(const HTTPRequest& request) const;
  bool cacheEntryExists(const std::string& filename) const;
  std::string getRequestHashCacheEntryName(const std::string& requestHash) const;
  void ensureDirectoryExists(const std::string& directory, bool empty = false) const;
  std::string getExpirationTime(int ttl) const;
  bool cachedEntryIsValid(const std::string& cachedFileName) const;
  bool containsCacheEntry(const HTTPRequest& request, HTTPResponse& response) const;
  void cacheEntry(const HTTPRequest& request, const HTTPResponse& response);

  std::string cacheDirectory;
  std::map<uint32_t, std::unique_ptr<std::mutex> > requestLocks;
};

#endif
