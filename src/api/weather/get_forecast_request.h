#pragma once

#include "../api_const.h"
#include "../base_request.h"

class GetForecastRequest : public BaseRequest
{
public:
  GetForecastRequest()
  {
    this->setMethod(POST);
    this->setUrl(URL_WEATHER_FORECAST);
    // If you need to send a body, set it here:
    // this->setBody("{\"stopId\":\"123\"}");
  }
};
