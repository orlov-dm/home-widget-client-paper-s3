#include "weather_view.h"
#include "weather_section.h"
#include "../../utils/debug.h"

WeatherView::WeatherView(const String &id) : View(id)
{
    this->setSeparatorSize({0, 2});
}

void WeatherView::setWeatherData(WeatherData *current, WeatherData *soon, WeatherData *tomorrow)
{
    this->resetChildren();

    this->currentData = *current;
    this->soonData = *soon;
    this->tomorrowData = *tomorrow;

    DEBUG_PRINTLN("Weather data updated in WeatherView");
    DEBUG_PRINTLN("Current Temperature: " + String(current->temperature));
    DEBUG_PRINTLN("Soon Temperature: " + String(soon->temperature));
    DEBUG_PRINTLN("Tomorrow Min Temperature: " + String(tomorrow->temperatureMin));

    auto currentSection = std::make_unique<WeatherSection>("current_weather_section", &this->currentData);
    auto soonSection = std::make_unique<WeatherSection>("soon_weather_section", &this->soonData);
    auto tomorrowSection = std::make_unique<WeatherSection>("tomorrow_weather_section", &this->tomorrowData);

    this->addChild(std::move(currentSection));
    auto bottomRow = std::make_unique<View>(
        Size{0, this->getSize().h / 4}, LayoutDirection::Horizontal);
    bottomRow->setSpacing(10);
    bottomRow->setSeparatorSize({2, 0});
    bottomRow->addChild(std::move(soonSection));
    bottomRow->addChild(std::move(tomorrowSection));

    this->addChild(std::move(bottomRow));

    this->setNeedsRender();
}
