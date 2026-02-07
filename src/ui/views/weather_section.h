#pragma once

#include <Arduino.h>

#include "../views/view.h"
#include "../components/index.h"
#include "../../api/weather/get_forecast_response.h"
#include "../../icons/index.h"

class WeatherSection : public View
{
public:
    WeatherSection(const String &id, WeatherData *weatherData);

protected:
    static String weatherCodeToDescription(const String &code);
    static IconName weatherCodeToIconName(const String &code);
    static String temperatureToString(float temperature);

private:
    Label *weatherDescriptionLabel;
    Icon *weatherIcon;
    Label *temperatureLabel;
    Label *descriptionLabel;
    Label *feelsLikeLabel;
    Label *feelsLikeValue;
    Label *extraLabel;
};