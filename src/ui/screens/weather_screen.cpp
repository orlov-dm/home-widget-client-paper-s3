#include "weather_screen.h"

#include <api/weather/weather_api.h>
#include <utils/datetime_utils.h>

void WeatherScreen::requestData()
{
    if (isRequestInProgress)
    {
        Serial.println("Request already in progress, skipping...");
        return;
    }

    auto &screenManager = ScreenManager::getInstance();
    auto &weatherApi = WeatherApi::getInstance();

    isRequestInProgress = true;
    GetForecastRequest request;
    auto response = weatherApi.getForecastData(&request);

    if (response && response->isSuccess())
    {
        WeatherData *currentWeather = response->getCurrentWeatherData();
        WeatherData *soonWeather = response->getSoonWeatherData();
        WeatherData *tomorrowWeather = response->getTomorrowWeatherData();

        this->weatherView->setWeatherData(currentWeather, soonWeather, tomorrowWeather);

        delete response;

        String timeStr = timestampToDatetime(getUtcTime());
        screenManager.setStatus("Refresh time: " + timeStr);
    }
    else
    {
        if (response)
        {
            Serial.print("API Request failed with status code: ");
            Serial.println(response->getStatusCode());
            Serial.print("Response body: ");
            Serial.println(response->getResponseBody());
            screenManager.setStatus("API Request Failed: " + response->getErrorDescription());
            delete response;
        }
        else
        {
            Serial.println("API Request failed: No response received");
            screenManager.setStatus("API Request Failed");
        }
    }
    isRequestInProgress = false;
}