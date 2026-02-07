#include "nav_bar.h"

#include <memory>

#include "../components/button.h"
#include "../screens/screen_manager.h"

NavBar::NavBar(const String &id) : View(id, LayoutDirection::Horizontal)
{
    this->setPadding(2);
    this->setSpacing(10);

    auto scheduleViewBtn = std::make_unique<Button>("");
    scheduleViewBtn->setName("ScheduleViewButton");
    scheduleViewBtn->setIcon(IconName::ICON_BUS);
    scheduleViewBtn->onTouch([]()
                             {
                                 Serial.println("NavBar: Schedule View button pressed");
                                 ScreenManager::getInstance().showScreen(ScreenID::TRANSPORT_SCHEDULE); });

    auto weatherViewBtn = std::make_unique<Button>("");
    weatherViewBtn->setName("WeatherViewButton");
    weatherViewBtn->setIcon(IconName::ICON_WEATHER_MAIN);
    weatherViewBtn->onTouch([]()
                            {
                                Serial.println("NavBar: Weather View button pressed");
                                ScreenManager::getInstance().showScreen(ScreenID::WEATHER); });

    this->addChild(std::move(scheduleViewBtn));
    this->addChild(std::move(weatherViewBtn));
}