#pragma once

#include "../base_response.h"
#include <Arduino.h>
#include <ArduinoJson.h>

/*
Server response structure:
interface GetWeatherForecastResponse {
    current: WeatherData | null;
    soon: WeatherData | null;
    tomorrow: WeatherData | null;
}

interface WeatherData {
    time: number;
    temperature?: number;
    temperatureMin?: number;
    temperatureMax?: number;
    apparentTemperature?: number;
    weatherCode: WeatherCodeSimplified | null;
    wind?: {
        speed: number;
        direction: Direction;
    };
    uvIndex?: {
        value: number;
        description: string;
    };
}

export enum WeatherCodeSimplified {
    Clear = 'clear',
    PartlyCloudy = 'partly_cloudy',
    Cloudy = 'cloudy',
    Fog = 'fog',
    Drizzle = 'drizzle',
    Rain = 'rain',
    Snow = 'snow',
    Thunderstorm = 'thunderstorm',
}
*/

namespace WeatherCodeSimplified
{
    const String CLEAR = "clear";
    const String PARTLY_CLOUDY = "partly_cloudy";
    const String CLOUDY = "cloudy";
    const String FOG = "fog";
    const String DRIZZLE = "drizzle";
    const String RAIN = "rain";
    const String SNOW = "snow";
    const String THUNDERSTORM = "thunderstorm";
};

enum WeatherDataType
{
    CURRENT,
    SOON,
    TOMORROW
};

struct WeatherData
{
    bool hasData() const { return time != 0; }
    uint32_t time;
    float temperature;
    float temperatureMin;
    float temperatureMax;
    float apparentTemperature;
    String weatherCode;
    float windSpeed;
    String windDirection;
    uint8_t uvIndexValue;
    String uvIndexDescription;
    WeatherDataType dataType;
};

class GetForecastResponse : public BaseResponse
{
public:
    GetForecastResponse(int statusCode, String responseBody)
        : BaseResponse(statusCode, responseBody)
    {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, responseBody);

        if (error)
        {
            Serial.print("JSON parsing failed: ");
            Serial.println(error.c_str());
            this->setErrorDescription(error.c_str());
            this->setIsValid(false);
            this->setIsSuccessValue(false);
            return;
        }

        bool success = doc["success"] | false;
        if (!success)
        {
            Serial.println("API indicates failure");
            this->setErrorDescription("API indicated failure");
            this->setIsSuccessValue(false);
            return;
        }

        this->setIsSuccessValue(true);
        // Check if data exists
        if (!doc["data"].is<JsonObject>())
        {
            Serial.println("JSON missing data object");
            this->setIsValid(false);
            this->setErrorDescription("Missing data object");
            return;
        }

        JsonObject dataObj = doc["data"].as<JsonObject>();

        if (success && dataObj["current"].isNull())
        {
            Serial.println("No data, but API indicates success, this could be valid");
            this->setIsValid(false);
            this->setErrorDescription("Missing current weather data");
            return;
        }

        // Check if current exists and is an object
        if (!dataObj["current"].is<JsonObject>())
        {
            Serial.println("JSON missing current object");
            this->setIsValid(false);
            this->setErrorDescription("Missing current object");
            return;
        }
        this->setIsValid(true);

        JsonObject current = dataObj["current"].as<JsonObject>();
        this->current.time = current["time"] | 0;
        this->current.temperature = current["temperature"] | 0.0f;
        this->current.temperatureMin = current["temperatureMin"] | 0.0f;
        this->current.temperatureMax = current["temperatureMax"] | 0.0f;
        this->current.apparentTemperature = current["apparentTemperature"] | 0.0f;
        this->current.weatherCode = current["weatherCode"] | "";
        this->current.windSpeed = current["wind"]["speed"] | 0.0f;
        this->current.windDirection = current["wind"]["direction"] | "";
        this->current.uvIndexValue = current["uvIndex"]["value"] | 0;
        this->current.uvIndexDescription = current["uvIndex"]["description"] | "";
        this->current.dataType = WeatherDataType::CURRENT;

        JsonObject soon = dataObj["soon"].as<JsonObject>();
        this->soon.time = soon["time"] | 0;
        this->soon.temperature = soon["temperature"] | 0.0f;
        this->soon.weatherCode = soon["weatherCode"] | "";
        this->soon.dataType = WeatherDataType::SOON;

        JsonObject tomorrow = dataObj["tomorrow"].as<JsonObject>();
        this->tomorrow.time = tomorrow["time"] | 0;
        this->tomorrow.temperatureMin = tomorrow["temperatureMin"] | 0.0f;
        this->tomorrow.temperatureMax = tomorrow["temperatureMax"] | 0.0f;
        this->tomorrow.weatherCode = tomorrow["weatherCode"] | "";
        this->tomorrow.dataType = WeatherDataType::TOMORROW;
    };

    WeatherData *getCurrentWeatherData() { return &current; }
    WeatherData *getSoonWeatherData() { return &soon; }
    WeatherData *getTomorrowWeatherData() { return &tomorrow; }

private:
    WeatherData current;
    WeatherData soon;
    WeatherData tomorrow;
};