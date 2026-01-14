#pragma once

#include <Arduino.h>
#include <api/api_client.h>
#include <ui/components/button.h>
#include <ui/views/schedule_view.h>

class Application
{
public:
    static Application &getInstance()
    {
        static Application instance;
        return instance;
    }

    void setup();
    void loop();
    void touchEvent();
    void renderUI();

private:
    Application() {}
    Application(const Application &) = delete;
    Application &operator=(const Application &) = delete;

    uint64_t wakeupTime = 0;

    bool isRequestInProgress = false;

    // TODO get rid of these pointers
    Button *btnRefresh = nullptr;
    ScheduleView *scheduleView = nullptr;
};