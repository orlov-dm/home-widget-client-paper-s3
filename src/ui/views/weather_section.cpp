#include "weather_section.h"

#include <cmath>
#include <memory>

WeatherSection::WeatherSection(const String &id, WeatherData *weatherData) : View(id)
{
    this->setSpacing(40);
    if (weatherData == nullptr || !weatherData->hasData())
    {
        // No valid data, show placeholder
        auto descriptionLabel = std::make_unique<Label>("No data", Size{0, 30}, TextSize::MEDIUM);
        this->descriptionLabel = descriptionLabel.get();
        this->addChild(std::move(descriptionLabel));
        return;
    }

    if (weatherData->dataType == WeatherDataType::CURRENT)
    {
        // this->weatherIcon = new Icon("weather_icon", Position{0, 0}, Size{50, 50}, IconType::WEATHER, weatherData->weatherCode); // TODO: Implement weather icons
        auto temperatureLabel = std::make_unique<Label>(this->temperatureToString(weatherData->temperature), Size{0, 0}, TextSize::XLARGE);
        temperatureLabel->setAlignment(Alignment::CENTER);
        this->temperatureLabel = temperatureLabel.get();

        auto descriptionLabel = std::make_unique<Label>(this->weatherCodeToDescription(weatherData->weatherCode), Size{0, 0}, TextSize::LARGE);
        descriptionLabel->setAlignment(Alignment::CENTER);
        this->descriptionLabel = descriptionLabel.get();

        auto feelsLikeLabel = std::make_unique<Label>("Feels like", Size{0, 0}, TextSize::MEDIUM);
        feelsLikeLabel->setAlignment(Alignment::CENTER);
        this->feelsLikeLabel = feelsLikeLabel.get();
        auto feelsLikeValue = std::make_unique<Label>(this->temperatureToString(weatherData->apparentTemperature), Size{0, 0}, TextSize::LARGE);
        feelsLikeValue->setAlignment(Alignment::CENTER);
        this->feelsLikeValue = feelsLikeValue.get();

        auto feelsLikeContainer = std::make_unique<View>(Size{0, 0}, LayoutDirection::Horizontal);
        feelsLikeContainer->setSpacing(5);
        feelsLikeContainer->addChild(std::move(feelsLikeLabel));
        feelsLikeContainer->addChild(std::move(feelsLikeValue));

        String uvInfo = "UV Index " + String(weatherData->uvIndexValue) + " (" + weatherData->uvIndexDescription + ")";
        String windInfo = "Wind " + String(weatherData->windSpeed) + " km/h " + weatherData->windDirection;
        auto extraLabel = std::make_unique<Label>(uvInfo + "|" + windInfo, Size{0, 0}, TextSize::SMALL);
        extraLabel->setAlignment(Alignment::CENTER);
        this->extraLabel = extraLabel.get();

        this->addChild(std::move(temperatureLabel));
        this->addChild(std::move(descriptionLabel));
        this->addChild(std::move(feelsLikeContainer));
        this->addChild(std::move(extraLabel));
    }
    else if (weatherData->dataType == WeatherDataType::SOON)
    {
        auto weatherDescriptionLabel = std::make_unique<Label>("In 4 hours", Size{0, 0}, TextSize::MEDIUM);
        weatherDescriptionLabel->setAlignment(Alignment::CENTER);
        this->weatherDescriptionLabel = weatherDescriptionLabel.get();

        // this->weatherIcon = new Icon("weather_icon_soon", Position{0, 0}, Size{50, 50}, IconType::WEATHER, weatherData->weatherCode); // TODO: Implement weather icons
        auto temperatureLabel = std::make_unique<Label>(this->temperatureToString(weatherData->temperature), Size{0, 0}, TextSize::LARGE);
        temperatureLabel->setAlignment(Alignment::CENTER);
        this->temperatureLabel = temperatureLabel.get();

        this->addChild(std::move(weatherDescriptionLabel));
        this->addChild(std::move(temperatureLabel));
    }
    else if (weatherData->dataType == WeatherDataType::TOMORROW)
    {
        // this->weatherIcon = new Icon("weather_icon_tomorrow", Position{0, 0}, Size{50, 50}, IconType::WEATHER, weatherData->weatherCode); // TODO: Implement weather icons
        auto weatherDescriptionLabel = std::make_unique<Label>("Tomorrow", Size{0, 0}, TextSize::MEDIUM);
        weatherDescriptionLabel->setAlignment(Alignment::CENTER);
        this->weatherDescriptionLabel = weatherDescriptionLabel.get();

        String minTemperature = this->temperatureToString(weatherData->temperatureMin);
        String maxTemperature = this->temperatureToString(weatherData->temperatureMax);
        auto temperatureLabel = std::make_unique<Label>(minTemperature + "/" + maxTemperature, Size{0, 0}, TextSize::LARGE);
        temperatureLabel->setAlignment(Alignment::CENTER);
        this->temperatureLabel = temperatureLabel.get();

        this->addChild(std::move(weatherDescriptionLabel));
        this->addChild(std::move(temperatureLabel));
    }
}

String WeatherSection::weatherCodeToDescription(const String &code)
{
    if (code == WeatherCodeSimplified::CLEAR)
        return "Clear";
    else if (code == WeatherCodeSimplified::PARTLY_CLOUDY)
        return "Partly Cloudy";
    else if (code == WeatherCodeSimplified::CLOUDY)
        return "Cloudy";
    else if (code == WeatherCodeSimplified::RAIN)
        return "Rain";
    else if (code == WeatherCodeSimplified::THUNDERSTORM)
        return "Thunderstorm";
    else if (code == WeatherCodeSimplified::SNOW)
        return "Snow";
    else
        return "Unknown";
}

String WeatherSection::temperatureToString(float temperature)
{
    return String(
        static_cast<int>(std::round(temperature)));
}
