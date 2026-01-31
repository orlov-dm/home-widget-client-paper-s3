#pragma once

#include <Arduino.h>
#include <vector>

#include "get_forecast_request.h"
#include "get_forecast_response.h"
#include "../api_client.h"

class WeatherApi
{
public:
    static WeatherApi &getInstance()
    {
        static WeatherApi instance;
        return instance;
    }

    GetForecastResponse *getForecastData(GetForecastRequest *request)
    {
        return client.doRequest<GetForecastResponse>(request);
    }

private:
    WeatherApi() {}
    WeatherApi(const WeatherApi &) = delete;
    WeatherApi &operator=(const WeatherApi &) = delete;

    ApiClient &client = ApiClient::getInstance();
};