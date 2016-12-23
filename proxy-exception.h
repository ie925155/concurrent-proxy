/**
 * File: proxy-exception.h
 * -----------------------
 * Defines the hierarchy of exceptions
 * used through http-proxy system.
 */

#ifndef _http_proxy_exception_
#define _http_proxy_exception_

#include <exception>
#include <string>

class HTTPProxyException: public std::exception {
 public:
  HTTPProxyException() throw() {}
  HTTPProxyException(const std::string& message) throw() : message(message) {}
  const char *what() const throw() { return message.c_str(); }
  ~HTTPProxyException() throw() {}

 protected:
  std::string message;
};

class HTTPBadRequestException: public HTTPProxyException {
 public:
  HTTPBadRequestException() throw() {}
  HTTPBadRequestException(const std::string& message) throw() : HTTPProxyException(message) {}
  ~HTTPBadRequestException() throw() {}
};

class HTTPRequestException: public HTTPProxyException {
 public:
  HTTPRequestException() throw() {}
  HTTPRequestException(const std::string& message) throw() : HTTPProxyException(message) {}
  ~HTTPRequestException() throw() {}
};

class HTTPResponseException: public HTTPProxyException {
 public:
  HTTPResponseException() throw() {}
  HTTPResponseException(const std::string& message) throw() : HTTPProxyException(message) {}
  ~HTTPResponseException() throw() {}
};

#endif
