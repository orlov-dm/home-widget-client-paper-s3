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