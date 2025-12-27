#ifndef BASE_RESPONSE_H
#define BASE_RESPONSE_H

#include <Arduino.h>

class BaseResponse {
public:
  BaseResponse(int statusCode, String responseBody)
      : statusCode(statusCode), responseBody(responseBody) {}

  int getStatusCode() { return statusCode; }
  String getResponseBody() { return responseBody; }

protected:
  int statusCode;
  String responseBody;
};

#endif