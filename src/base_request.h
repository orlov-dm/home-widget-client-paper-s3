#ifndef BASE_REQUEST_H
#define BASE_REQUEST_H

#include <Arduino.h>

class BaseRequest {
public:
  void setMethod(String method) { this->method = method; }
  void setUrl(String url) { this->url = url; }
  void setBody(String body) { this->body = body; }

  String getMethod() { return method; }
  String getUrl() { return url; }
  String getBody() { return body; }

private:
  String method = "";
  String url = "";
  String body = "";
};

#endif