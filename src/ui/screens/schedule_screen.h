#pragma once

#include <Arduino.h>
#include <memory>

#include "screen_manager.h"
#include "../components/index.h"
#include "../views/schedule_view.h"
#include "../../app/component_ids.h"
#include "../../icons/index.h"

class ScheduleScreen : public View
{
public:
    ScheduleScreen(const String &id) : View(id)
    {
        auto scheduleView = std::make_unique<ScheduleView>(ComponentID::SCHEDULE_VIEW);
        scheduleView->setBackgroundColor(TFT_WHITE);
        scheduleView->setSpacing(5);
        scheduleView->setPadding(10);
        this->scheduleView = scheduleView.get();
        this->addChild(std::move(scheduleView));

        auto btnRefresh = std::make_unique<Button>(
            "",
            Size{40, 40},
            ComponentID::REFRESH_BUTTON,
            Icon::ICON_REFRESH);
        Button *btnRefreshPtr = btnRefresh.get();   // Get raw pointer before move
        btnRefresh->onTouch([this, btnRefreshPtr]() // Capture the pointer
                            {
                                Serial.println("Refresh button pressed");
                                ScreenManager::getInstance().setStatus("Refreshing data...");
                                this->onRefresh(); 
                            });
        this->addChild(std::move(btnRefresh));
    };

    virtual ~ScheduleScreen() {}

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
    ScheduleView *scheduleView = nullptr;
};