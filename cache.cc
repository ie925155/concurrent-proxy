/**
 * File: cache.cc
 * --------------
 * Presents the implementation of the HTTPCache class as
 * presented in http-response-cache.h.
 */

#include <unistd.h>
#include <fstream>
#include <sstream>
#include <string>
#include <functional>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/dir.h>
//#include <dirent.h>

#include "cache.h"
#include "request.h"
#include "response.h"
#include "proxy-exception.h"
#include "ostreamlock.h"

using namespace std;

static const string kCacheSubdirectory = ".http-proxy-cache";
HTTPCache::HTTPCache() {
  string homeDirectoryEnv = getenv("HOME");
  cacheDirectory = homeDirectoryEnv + "/" + kCacheSubdirectory;
  ensureDirectoryExists(cacheDirectory);
}

bool HTTPCache::shouldCache(const HTTPRequest& request, const HTTPResponse& response) const {
  static const int kMinTTLForCache = 100; // in seconds (don't bother if it can't live for very long)
  return
    request.getMethod() == "GET" &&
    response.getResponseCode() == 200 &&
    response.permitsCaching() &&
    response.getTTL() >= kMinTTLForCache;
}

bool HTTPCache::containsCacheEntry(const HTTPRequest& request, HTTPResponse& response) const {
  if (request.getMethod() != "GET") return false;
  string requestHash = hashRequest(request);
  bool exists = cacheEntryExists(requestHash);
  if (!exists) return false;
  string cachedFileName = getRequestHashCacheEntryName(requestHash);
  if (cachedFileName.empty()) return false;
  string fullCacheEntryName = cacheDirectory + "/" + requestHash + "/" + cachedFileName;
  if (!cachedEntryIsValid(cachedFileName)) {
    remove(fullCacheEntryName.c_str());
    string fullCacheDirectoryName = cacheDirectory + "/" + requestHash;
    unlink(fullCacheDirectoryName.c_str());
    return false;
  }

  ifstream instream(fullCacheEntryName.c_str(), ios::in | ios::binary);
  try {
    response.ingestResponseHeader(instream);
    response.ingestPayload(instream);
    cout << oslock << "     [Using cached copy of previous request for "
	 << request.getURL() << ".]" << endl << osunlock;
    return true;
  } catch (HTTPProxyException& hpe) {
    cerr << oslock << "     [Problem rehydrating previously cached response for "
	 << request.getURL() << ".]" << endl;
    cerr << "     [Operating as if cached entry doesn't exist and forwarding request to origin server.]"
	 << endl << osunlock;
    return false;
  }
}

void HTTPCache::cacheEntry(const HTTPRequest& request, const HTTPResponse& response) {
  string requestHash = hashRequest(request);
  cout << oslock << "     [Okay to cache response, so caching response under hash of "
       << requestHash << " for next " << response.getTTL() << " seconds.]" << endl << osunlock;
  ensureDirectoryExists(cacheDirectory + "/" + requestHash, /* empty = */ true);
  string cacheEntryName = cacheDirectory + "/" + requestHash + "/" + getExpirationTime(response.getTTL());
  ofstream outfile(cacheEntryName.c_str(), ios::out | ios::binary);
  outfile << response;
  outfile.flush();
}

string HTTPCache::hashRequest(const HTTPRequest& request) const {
  hash<string> hasher;
  ostringstream oss;
  size_t hashValue = hasher(serializeRequest(request));
  oss << hashValue;
  return oss.str();
}

string HTTPCache::serializeRequest(const HTTPRequest& request) const {
  ostringstream oss;
  oss << request;
  return oss.str();
}

bool HTTPCache::cacheEntryExists(const string& requestHash) const {
  string path = cacheDirectory + "/" + requestHash;
  struct stat st;
  if (lstat(path.c_str(), &st) != 0) return false;
  if (!S_ISDIR(st.st_mode)) return false;

  DIR *dir = opendir(path.c_str());
  bool exists = false;
  struct dirent entry, *result;
  while (!exists) {
    int ret = readdir_r(dir, &entry, &result);
    if(ret > 0 && result == NULL){
        cerr << "readdir_r failed ret=" << ret << endl;
        break;
    }
    string dirEntry = entry.d_name;
    exists = dirEntry != "." && dirEntry != "..";
  }

  closedir(dir);
  return exists;
}

string HTTPCache::getRequestHashCacheEntryName(const string& requestHash) const {
  string requestHashDirectory = cacheDirectory + "/" + requestHash;
  DIR *dir = opendir(requestHashDirectory.c_str());
  if (dir == NULL) return "";
  string cachedEntryName;
  struct dirent entry, *result;

  while (true) {
    int ret = readdir_r(dir, &entry, &result);
    if(ret > 0 && result == NULL){
        cerr << "readdir_r failed ret=" << ret << endl;
        break;
    }
    cachedEntryName = entry.d_name;
    if (cachedEntryName != "." && cachedEntryName != "..") break;
  }

  closedir(dir);
  if (cachedEntryName == ".." || cachedEntryName == ".") return "";
  return cachedEntryName;
}

static const int kDefaultPermissions = 0755;
void HTTPCache::ensureDirectoryExists(const string& directory, bool empty) const {
  struct stat st;
  if (lstat(directory.c_str(), &st) != 0){
    mkdir(directory.c_str(), kDefaultPermissions);
}

  if (!empty) return;
  DIR *dir = opendir(directory.c_str());
  struct dirent entry, *result;
  while (true) {
    int ret = readdir_r(dir, &entry, &result);
    if(ret > 0 && result == NULL){
        cerr << "readdir_r failed ret=" << ret << endl;
        break;
    }
    string dirEntry = entry.d_name;
    if (dirEntry != "." && dirEntry != "..")
      remove(dirEntry.c_str());
  }

  closedir(dir);
}

string HTTPCache::getExpirationTime(int ttl) const {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  if (ttl > 0) tv.tv_sec += ttl;
  ostringstream oss;
  oss << tv.tv_sec;
  return oss.str();
}

bool HTTPCache::cachedEntryIsValid(const string& cachedFileName) const {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  time_t expirationTime = strtol(cachedFileName.c_str(), NULL, 0);
  return tv.tv_sec <= expirationTime;
}
