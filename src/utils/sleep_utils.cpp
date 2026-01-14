#include "sleep_utils.h"

#include <esp_sleep.h>
#include <M5Unified.h>

void sleepSetup()
{
    // Check wakeup reason
    String wakeupReason = getWakeupReason();
    Serial.printf("Wakeup reason: %s\n", wakeupReason);
}

void goToSleep(uint64_t sleepDurationSeconds)
{
    Serial.println("Preparing to sleep...");

    delay(100);

    if (sleepDurationSeconds == 0)
    {
        Serial.println("Going to sleep indefinitely (wake on button only)");
        Serial.flush();
        // Sleep forever - only button/touch will wake
        M5.Power.deepSleep(0);
    }
    else
    {
        Serial.printf("Going to sleep for %llu seconds\n", sleepDurationSeconds);
        Serial.flush();
        M5.Power.deepSleep(sleepDurationSeconds * 1000000ULL);
    }
}

String getWakeupReason()
{
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

    switch (wakeup_reason)
    {
    case ESP_SLEEP_WAKEUP_EXT0:
        return "Button";
    case ESP_SLEEP_WAKEUP_EXT1:
        return "External";
    case ESP_SLEEP_WAKEUP_TIMER:
        return "Timer";
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
        return "Touch";
    case ESP_SLEEP_WAKEUP_ULP:
        return "ULP";
    case ESP_SLEEP_WAKEUP_GPIO:
        return "GPIO";
    case ESP_SLEEP_WAKEUP_UART:
        return "UART";
    default:
        return "Power On";
    }
}