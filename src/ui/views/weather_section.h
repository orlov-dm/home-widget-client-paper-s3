#pragma once

#include <Arduino.h>

#include "../views/view.h"
#include "../components/index.h"
#include "../../api/weather/get_forecast_response.h"

class WeatherSection : public View
{
public:
    WeatherSection(const String &id, WeatherData *weatherData);

protected:
    String weatherCodeToDescription(const String &code);

    String temperatureToString(float temperature);

private:
    Label *weatherDescriptionLabel;
    Icon weatherIcon;
    Label *temperatureLabel;
    Label *descriptionLabel;
    Label *feelsLikeLabel;
    Label *feelsLikeValue;
    Label *extraLabel;
};