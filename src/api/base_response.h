#pragma once

#include <Arduino.h>

class BaseResponse
{
public:
  BaseResponse(int statusCode, String responseBody)
      : statusCode(statusCode), responseBody(responseBody) {}

  int getStatusCode() { return statusCode; }
  String getResponseBody() { return responseBody; }
  String getErrorDescription() { return errorDescription; }
  bool isSuccess() { return isValid && isSuccessValue && getStatusCode() >= 200 && getStatusCode() < 300; }

protected:
  bool getIsValid() { return isValid; }
  void setIsValid(bool valid) { isValid = valid; }

  void setIsSuccessValue(bool success) { isSuccessValue = success; }
  void setErrorDescription(const String &desc) { errorDescription = desc; }

private:
  int statusCode;
  String responseBody;
  String errorDescription;
  bool isValid = true;
  bool isSuccessValue = false;
};