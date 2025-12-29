#ifndef BASE_RESPONSE_H
#define BASE_RESPONSE_H

#include <Arduino.h>

class BaseResponse
{
public:
  BaseResponse(int statusCode, String responseBody)
      : statusCode(statusCode), responseBody(responseBody) {}

  int getStatusCode() { return statusCode; }
  String getResponseBody() { return responseBody; }
  String getErrorDescription() { return errorDescription; }

protected:
  int statusCode;
  String responseBody;
  String errorDescription;
};

#endif