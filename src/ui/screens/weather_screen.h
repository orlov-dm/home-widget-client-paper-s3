#pragma once

#include <Arduino.h>
#include <memory>

#include "screen_manager.h"
#include "../components/index.h"
#include "../views/weather_view.h"
#include "../../app/component_ids.h"
#include "../../icons/index.h"

class WeatherScreen : public View
{
public:
    WeatherScreen(const String &id) : View(id)
    {
        auto weatherView = std::make_unique<WeatherView>(ComponentID::WEATHER_VIEW);
        weatherView->setBackgroundColor(TFT_WHITE);
        weatherView->setSpacing(5);
        weatherView->setPadding(10);
        this->weatherView = weatherView.get();
        this->addChild(std::move(weatherView));
    };

    virtual ~WeatherScreen() {}

protected:
    void onEnter() override
    {
        this->requestData();
    }
    void onRefresh() override
    {
        this->requestData();
    }
    void requestData();

private:
    bool isRequestInProgress = false;
    WeatherView *weatherView = nullptr;
};