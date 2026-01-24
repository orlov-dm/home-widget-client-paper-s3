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
    scheduleViewBtn->setIcon(Icon::ICON_BUS);
    scheduleViewBtn->onTouch([]()
                             {
                                 Serial.println("NavBar: Schedule View button pressed");
                                 ScreenManager::getInstance().showScreen(ScreenID::TRANSPORT_SCHEDULE); });
    this->addChild(std::move(scheduleViewBtn));
}