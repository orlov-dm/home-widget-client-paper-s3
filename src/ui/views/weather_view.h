#pragma once

#include <Arduino.h>
#include <vector>

#include "../types.h"
#include "view.h"
#include "../components/button.h"

#include "../../api/weather/get_forecast_response.h"

class WeatherView : public View
{
public:
    WeatherView(const String &id);

    void setWeatherData(
        WeatherData *current,
        WeatherData *soon,
        WeatherData *tomorrow);

private:
    WeatherData currentData;
    WeatherData soonData;
    WeatherData tomorrowData;

    View *currentWeatherSection = nullptr;
    View *soonWeatherSection = nullptr;
    View *tomorrowWeatherSection = nullptr;
};
