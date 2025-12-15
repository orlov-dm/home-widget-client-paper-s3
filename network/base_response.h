#ifndef BASE_RESPONSE_H
#define BASE_RESPONSE_H

#include <Arduino.h>
#include <ArduinoJson.h>

class BaseResponse {
public:
  int statusCode;
  String responseBody;
  JsonDocument data;

  virtual JsonDocument serialize() = 0;
  virtual BaseResponse decerialize(String json) = 0;
  virtual int getStatusCode() { return statusCode; }
  virtual String getResponseBody() { return responseBody; }
  virtual JsonDocument getData() { return data; }
};

#endif