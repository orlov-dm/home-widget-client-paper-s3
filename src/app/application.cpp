#include "application.h"

#include <M5Unified.hpp>

#include "../utils/debug.h"
#include "../utils/sleep_utils.h"
#include "../utils/wifi_utils.h"
#include "../utils/datetime_utils.h"
#include "../utils/dev_utils.h"

#include "../api/api_client.h"

#include "../app/component_manager.h"
#include "../app/component_ids.h"

#include "../ui/events/event_manager.h"
#include "../ui/screens/screen_manager.h"
#include "../ui/components/button.h"
#include "../ui/views/schedule_view.h"
#include "../ui/screens/schedule_screen.h"
#include <ui/screens/weather_screen.h>

const uint32_t AWAKE_DURATION = 120000; // 2 minutes in milliseconds (how long to stay awake)

void Application::setup()
{
    DEBUG_BEGIN(9600);
    DEBUG_PRINTLN("BOOT");

    auto cfg = M5.config();
    M5.begin(cfg);

    wakeupTime = millis();

    M5.Display.begin();
    M5.Display.setEpdMode(epd_mode_t::epd_fast);
    M5.Display.setTextSize(2);

    sleepSetup();

    ScreenManager &screenManager = ScreenManager::getInstance();
    screenManager.init();

    WifiConnectionStatus wifiStatus = wifiSetup();
    if (wifiStatus != WifiConnectionStatus::CONNECTED)
    {
        screenManager.setStatus("WiFi Failed! Touch to retry");
        renderUI();

        // Wait for user to touch screen to retry
        while (wifiStatus != WifiConnectionStatus::CONNECTED)
        {
            if (EventManager::getInstance().detectAnyTouch())
            {
                screenManager.setStatus("Retrying WiFi...");
                renderUI();
                wifiStatus = wifiSetup();
                if (wifiStatus != WifiConnectionStatus::CONNECTED)
                {
                    screenManager.setStatus("WiFi Failed! Touch to retry");
                    renderUI();
                }
            }
            delay(100);
        }
    }

    // Setup time with error handling
    bool timeSuccess = timeSetup();
    if (!timeSuccess)
    {
        screenManager.setStatus("Time sync failed! Retrying...");
        renderUI();
        delay(2000);

        // Retry time sync once
        timeSuccess = timeSetup();
        if (!timeSuccess)
        {
            screenManager.setStatus("Time sync failed! Data may be inaccurate");
            renderUI();
            delay(3000);
        }
        screenManager.setStatus("Loading data...");
        renderUI();
    }

    screenManager.addScreen(std::make_unique<ScheduleScreen>(ComponentID::SCHEDULE_SCREEN), ScreenID::TRANSPORT_SCHEDULE);
    screenManager.addScreen(std::make_unique<WeatherScreen>(ComponentID::WEATHER_SCREEN), ScreenID::WEATHER);
    screenManager.showScreen(ScreenID::TRANSPORT_SCHEDULE);

    renderUI();
}

void Application::loop()
{
    M5.update();

    if (millis() - wakeupTime >= AWAKE_DURATION)
    {
        bool devMode = isDevMode();
        DEBUG_PRINTF("Dev mode check: %s\n", devMode ? "true" : "false");

        if (devMode)
        {
            DEBUG_PRINTLN("Development mode detected (USB connected) - skipping deep sleep");
            DEBUG_PRINTLN("Device will stay awake for debugging");
            return;
        }
        else
        {
            ScreenManager::getInstance().setStatus("Auto sleep...");
            renderUI();
            goToSleep();
        }
    }

    // Handle touch events first for responsive UI
    EventManager::getInstance().handleTouch(&wakeupTime);

    if (EventManager::getInstance().needsRender())
    {
        renderUI();
        EventManager::getInstance().setNeedsRender(false);
    }

    delay(50); // Reduced delay for more responsive touch
}

void Application::renderUI()
{
    M5.Display.setEpdMode(epd_mode_t::epd_text);
    try
    {
        M5.Display.startWrite();
        ScreenManager::getInstance().renderCurrentScreen();
        M5.Display.endWrite();
        M5.Display.display();
    }
    catch (...)
    {
        DEBUG_PRINTLN("Error during UI rendering");
    }
    M5.Display.setEpdMode(epd_mode_t::epd_fast);
}
