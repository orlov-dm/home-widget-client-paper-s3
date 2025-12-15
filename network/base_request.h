#ifndef BASE_REQUEST_H
#define BASE_REQUEST_H

#include <Arduino.h>
#include <ArduinoJson.h>

class BaseRequest
{
public:
  String method;
  String url;
  String body;

  virtual JsonDocument serialize() = 0;
  virtual String getMethod() { return method; }
  virtual String getUrl() { return url; }
  virtual String getBody() { return body.toString(); }
};

#endif