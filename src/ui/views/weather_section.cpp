#include "weather_section.h"

#include <cmath>
#include <memory>

#include "../../icons/index.h"

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

    const auto &size = this->getSize();

    auto iconContainer = std::make_unique<View>(Size{0, 0}, LayoutDirection::Vertical);
    iconContainer->setName("IconContainer");
    auto weatherIconName = WeatherSection::weatherCodeToIconName(weatherData->weatherCode);
    if (weatherData->dataType == WeatherDataType::CURRENT)
    {
        auto weatherIcon = std::make_unique<Icon>(weatherIconName, IconSize::ICON_SIZE_256);
        weatherIcon->setAlignment(Alignment::CENTER);
        this->weatherIcon = weatherIcon.get();

        auto temperatureLabel = std::make_unique<Label>(WeatherSection::temperatureToString(weatherData->temperature), Size{0, 0}, TextSize::XLARGE);
        temperatureLabel->setTextAlignment(TextAlignment::TA_CENTER);
        this->temperatureLabel = temperatureLabel.get();

        auto descriptionLabel = std::make_unique<Label>(WeatherSection::weatherCodeToDescription(weatherData->weatherCode), Size{0, 0}, TextSize::LARGE);
        descriptionLabel->setTextAlignment(TextAlignment::TA_CENTER);
        this->descriptionLabel = descriptionLabel.get();

        auto feelsLikeLabel = std::make_unique<Label>("Feels like", Size{0, 0}, TextSize::MEDIUM);
        feelsLikeLabel->setTextAlignment(TextAlignment::TA_CENTER);
        this->feelsLikeLabel = feelsLikeLabel.get();
        auto feelsLikeValue = std::make_unique<Label>(WeatherSection::temperatureToString(weatherData->apparentTemperature), Size{0, 0}, TextSize::LARGE);
        feelsLikeValue->setTextAlignment(TextAlignment::TA_CENTER);
        this->feelsLikeValue = feelsLikeValue.get();

        auto feelsLikeContainer = std::make_unique<View>(Size{0, 0}, LayoutDirection::Horizontal);
        feelsLikeContainer->setSpacing(5);
        feelsLikeContainer->addChild(std::move(feelsLikeLabel));
        feelsLikeContainer->addChild(std::move(feelsLikeValue));

        String uvInfo = "UV Index " + String(weatherData->uvIndexValue) + " (" + weatherData->uvIndexDescription + ")";
        String windInfo = "Wind " + String(weatherData->windSpeed) + " km/h " + weatherData->windDirection;
        auto extraLabel = std::make_unique<Label>(uvInfo + "|" + windInfo, Size{0, 0}, TextSize::SMALL);
        extraLabel->setTextAlignment(TextAlignment::TA_CENTER);
        this->extraLabel = extraLabel.get();

        iconContainer->addChild(std::move(weatherIcon));
        this->addChild(std::move(iconContainer));
        this->addChild(std::move(temperatureLabel));
        this->addChild(std::move(descriptionLabel));
        this->addChild(std::move(feelsLikeContainer));
        this->addChild(std::move(extraLabel));
    }
    else if (weatherData->dataType == WeatherDataType::SOON)
    {
        auto weatherIcon = std::make_unique<Icon>(weatherIconName, IconSize::ICON_SIZE_128);
        weatherIcon->setAlignment(Alignment::CENTER);
        this->weatherIcon = weatherIcon.get();

        auto weatherDescriptionLabel = std::make_unique<Label>("In 4 hours", Size{0, 0}, TextSize::MEDIUM);
        weatherDescriptionLabel->setTextAlignment(TextAlignment::TA_CENTER);
        this->weatherDescriptionLabel = weatherDescriptionLabel.get();

        auto temperatureLabel = std::make_unique<Label>(WeatherSection::temperatureToString(weatherData->temperature), Size{0, 0}, TextSize::LARGE);
        temperatureLabel->setTextAlignment(TextAlignment::TA_CENTER);
        this->temperatureLabel = temperatureLabel.get();

        iconContainer->addChild(std::move(weatherIcon));
        this->addChild(std::move(iconContainer));
        this->addChild(std::move(weatherDescriptionLabel));
        this->addChild(std::move(temperatureLabel));
    }
    else if (weatherData->dataType == WeatherDataType::TOMORROW)
    {
        auto weatherIcon = std::make_unique<Icon>(weatherIconName, IconSize::ICON_SIZE_128);
        weatherIcon->setAlignment(Alignment::CENTER);
        this->weatherIcon = weatherIcon.get();

        auto weatherDescriptionLabel = std::make_unique<Label>("Tomorrow", Size{0, 0}, TextSize::MEDIUM);
        weatherDescriptionLabel->setTextAlignment(TextAlignment::TA_CENTER);
        this->weatherDescriptionLabel = weatherDescriptionLabel.get();

        String minTemperature = WeatherSection::temperatureToString(weatherData->temperatureMin);
        String maxTemperature = WeatherSection::temperatureToString(weatherData->temperatureMax);
        auto temperatureLabel = std::make_unique<Label>(minTemperature + "/" + maxTemperature, Size{0, 0}, TextSize::LARGE);
        temperatureLabel->setTextAlignment(TextAlignment::TA_CENTER);
        this->temperatureLabel = temperatureLabel.get();

        iconContainer->addChild(std::move(weatherIcon));
        this->addChild(std::move(iconContainer));
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

IconName WeatherSection::weatherCodeToIconName(const String &code)
{
    if (code == WeatherCodeSimplified::CLEAR)
        return IconName::ICON_WEATHER_SUN;
    else if (code == WeatherCodeSimplified::PARTLY_CLOUDY)
        return IconName::ICON_WEATHER_PARTLY_CLOUDY;
    else if (code == WeatherCodeSimplified::CLOUDY)
        return IconName::ICON_WEATHER_CLOUDY;
    else if (code == WeatherCodeSimplified::RAIN)
        return IconName::ICON_WEATHER_RAIN;
    else if (code == WeatherCodeSimplified::THUNDERSTORM)
        return IconName::ICON_WEATHER_THUNDERSTORM;
    else if (code == WeatherCodeSimplified::SNOW)
        return IconName::ICON_WEATHER_SNOW;
    else
        return IconName::ICON_NONE;
}
