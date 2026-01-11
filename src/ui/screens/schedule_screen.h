#pragma once

#include <Arduino.h>
#include <memory>

#include "../views/schedule_view.h"
#include "../../app/component_ids.h"
#include "../../icons/index.h"
#include "screen_manager.h"

class ScheduleScreen : public View
{
public:
    ScheduleScreen(const String &id) : View(id)
    {
        auto scheduleView = std::make_unique<ScheduleView>(ComponentID::SCHEDULE_VIEW);
        scheduleView->setBackgroundColor(TFT_WHITE);
        scheduleView->setSpacing(5);
        scheduleView->setPadding(10);
        scheduleView->setSeparatorSize({0, 2});
        this->addChild(std::move(scheduleView));

        this->addChild(std::make_unique<Button>(
            "",
            Size{40, 40},
            ComponentID::REFRESH_BUTTON,
            ICON_REFRESH));
    };
};