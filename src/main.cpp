#include <Arduino.h>
#include <M5Unified.h>
#include <memory>

#include "api/api_client.h"
#include "api/get_transport_request.h"
#include "api/get_transport_response.h"
#include "utils/wifi_utils.h"
#include "utils/sleep_utils.h"
#include "utils/datetime_utils.h"
#include "utils/dev_utils.h"
#include "ui/views/status_bar.h"
#include "ui/views/schedule_view.h"
#include "ui/components/index.h"
#include "ui/screens/screen_manager.h"
#include "ui/screens/schedule_screen.h"
#include "ui/screens/const.h"
#include "app/component_ids.h"
#include "app/component_manager.h"

ScreenManager &screenManager = ScreenManager::getInstance();

Button *btnRefresh = nullptr;
ScheduleView *scheduleView = nullptr;
WifiConnectionStatus wifiStatus = WifiConnectionStatus::UNSET;
ApiClient client;
const uint32_t AWAKE_DURATION = 120000; // 2 minutes in milliseconds (how long to stay awake)
uint32_t wakeupTime = 0;

bool isRequestInProgress = false;

void drawUI()
{
  M5.Display.startWrite();
  ScreenManager::getInstance().renderCurrentScreen();
  M5.Display.endWrite();
  M5.Display.display();
}

void requestData()
{
  if (isRequestInProgress)
  {
    Serial.println("Request already in progress, skipping...");
    return;
  }

  isRequestInProgress = true;
  GetTransportRequest request;
  GetTransportResponse *response =
      client.doRequest<GetTransportResponse>(&request);

  if (response && response->isSuccess())
  {
    TransportTime *times = response->getTransportTimes();
    auto count = response->getTransportTimesCount();
    std::vector<ScheduleEntry> scheduleEntries;
    for (int i = 0; i < count; i++)
    {
      ScheduleEntry entry;
      entry.route = times[i].route;
      entry.expectedArriveTimestamp = times[i].expectedArriveTimestamp;
      scheduleEntries.push_back(entry);
    }

    if (scheduleView)
    {
      scheduleView->setScheduleData(scheduleEntries);
    }
    else
    {
      Serial.println("ERROR: scheduleView is null!");
    }

    delete response;

    String timeStr = timestampToDatetime(getUtcTime());
    screenManager.setStatus("Refresh time: " + timeStr);
  }
  else
  {
    if (response)
    {
      Serial.print("API Request failed with status code: ");
      Serial.println(response->getStatusCode());
      Serial.print("Response body: ");
      Serial.println(response->getResponseBody());
      screenManager.setStatus("API Request Failed: " + response->getErrorDescription());
      delete response;
    }
    else
    {
      Serial.println("API Request failed: No response received");
    }
    screenManager.setStatus("API Request Failed");
  }
  isRequestInProgress = false;
}

// ---------- SETUP ----------
void setup()
{
  Serial.begin(9600);
  Serial.println("BOOT");

  auto cfg = M5.config();
  M5.begin(cfg);

  wakeupTime = millis();

  M5.Display.begin();
  M5.Display.setEpdMode(epd_mode_t::epd_text);
  M5.Display.setTextSize(2);

  screenManager.init();
  screenManager.addScreen(std::make_unique<ScheduleScreen>(ComponentID::SCHEDULE_SCREEN), ScreenID::TRANSPORT_SCHEDULE);
  screenManager.showScreen(ScreenID::TRANSPORT_SCHEDULE);

  // temporary until event manager is implemented
  btnRefresh = ComponentManager::getInstance().findComponentById<Button>(String(ComponentID::REFRESH_BUTTON));
  scheduleView = ComponentManager::getInstance().findComponentById<ScheduleView>(String(ComponentID::SCHEDULE_VIEW));

  Serial.printf("btnRefresh: %p, scheduleView: %p\n", btnRefresh, scheduleView);

  sleepSetup();

  WifiConnectionStatus wifiStatus = wifiSetup();
  if (wifiStatus != WifiConnectionStatus::CONNECTED)
  {
    screenManager.setStatus("WiFi Failed! Touch to retry");
    drawUI();

    // Wait for user to touch screen to retry
    while (wifiStatus != WifiConnectionStatus::CONNECTED)
    {
      M5.update();
      auto t = M5.Touch.getDetail();
      if (t.wasPressed())
      {
        screenManager.setStatus("Retrying WiFi...");
        drawUI();
        wifiStatus = wifiSetup();
        if (wifiStatus != WifiConnectionStatus::CONNECTED)
        {
          screenManager.setStatus("WiFi Failed! Touch to retry");
          drawUI();
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
    drawUI();
    delay(2000);

    // Retry time sync once
    timeSuccess = timeSetup();
    if (!timeSuccess)
    {
      screenManager.setStatus("Time sync failed! Data may be inaccurate");
      drawUI();
      delay(3000);
    }
    screenManager.setStatus("Loading data...");
    drawUI();
  }

  requestData();
  drawUI();
}

// ---------- LOOP ----------
void loop()
{
  M5.update();

  if (millis() - wakeupTime >= AWAKE_DURATION)
  {
    bool devMode = isDevMode();
    Serial.printf("Dev mode check: %s\n", devMode ? "true" : "false");

    if (devMode)
    {
      Serial.println("Development mode detected (USB connected) - skipping deep sleep");
      Serial.println("Device will stay awake for debugging");
      return;
    }
    else
    {
      screenManager.setStatus("Auto sleep...");
      drawUI();
      delay(1000);

      goToSleep();
    }
  }

  auto t = M5.Touch.getDetail();
  bool uiChanged = false;

  if (t.wasPressed())
  {
    // Reset awake timer on any touch interaction
    wakeupTime = millis();

    if (btnRefresh->isTouched(t.x, t.y))
    {
      btnRefresh->setPressed(true);

      // Draw pressed state immediately for visual feedback
      drawUI();

      requestData();

      uiChanged = true;
    }
  }

  if (t.wasReleased())
  {
    if (btnRefresh->isPressed())
    {
      btnRefresh->setPressed(false);
      uiChanged = true;
    }
  }

  if (uiChanged)
  {
    drawUI();
  }

  delay(100);
}