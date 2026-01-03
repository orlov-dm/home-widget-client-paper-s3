#include <Arduino.h>
#include <M5Unified.h>

#include "api/api_client.h"
#include "api/get_transport_request.h"
#include "api/get_transport_response.h"
#include "utils/wifi_utils.h"
#include "utils/sleep_utils.h"
#include "utils/datetime_utils.h"
#include "utils/dev_utils.h"
#include "ui/components/index.h"
#include "ui/views/schedule_view.h"

Button btnRefresh;
StatusBar *statusBar = nullptr;
ScheduleView *scheduleView = nullptr;
WifiConnectionStatus wifiStatus = WifiConnectionStatus::UNSET;
ApiClient client;
const uint32_t AWAKE_DURATION = 120000; // 2 minutes in milliseconds (how long to stay awake)
uint32_t wakeupTime = 0;

bool isRequestInProgress = false;

void drawUI()
{
  M5.Display.startWrite();
  if (statusBar)
  {
    statusBar->draw();
  }
  drawButton(btnRefresh);
  if (scheduleView)
  {
    scheduleView->draw();
  }
  M5.Display.endWrite();
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

  if (scheduleView)
  {
    scheduleView->reset();
  }

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
    scheduleView->setScheduleData(scheduleEntries);

    delete response;

    String timeStr = timestampToDatetime(getUtcTime());
    statusBar->setValue((String("Data refreshed at ") + timeStr).c_str());
  }
  else
  {
    if (response)
    {
      Serial.print("API Request failed with status code: ");
      Serial.println(response->getStatusCode());
      Serial.print("Response body: ");
      Serial.println(response->getResponseBody());
      statusBar->setValue("API Request Failed: " + String(response->getErrorDescription()));
      delete response;
    }
    else
    {
      Serial.println("API Request failed: No response received");
    }
    statusBar->setValue("API Request Failed");
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

  statusBar = new StatusBar(
      M5.Display,
      0, 0, M5.Display.width(), 40, "Starting up...");

  btnRefresh.x = 20;
  btnRefresh.y = M5.Display.height() - 20 - 40;
  btnRefresh.w = 40;
  btnRefresh.h = 40;
  btnRefresh.icon = ICON_REFRESH;

  auto scheduleViewY = statusBar->getSize().h + statusBar->getPosition().y + 10;
  scheduleView = new ScheduleView(
      Position{0, scheduleViewY},
      Size{M5.Display.width(), btnRefresh.y - scheduleViewY - 20});

  // Draw initial UI to show immediate feedback
  drawUI();

  sleepSetup();

  // Setup WiFi with error handling
  statusBar->setValue("Connecting to WiFi...");
  drawUI();
  WifiConnectionStatus wifiStatus = wifiSetup();
  if (wifiStatus != WifiConnectionStatus::CONNECTED)
  {
    statusBar->setValue("WiFi Failed! Touch to retry");
    drawUI();

    // Wait for user to touch screen to retry
    while (wifiStatus != WifiConnectionStatus::CONNECTED)
    {
      M5.update();
      auto t = M5.Touch.getDetail();
      if (t.wasPressed())
      {
        statusBar->setValue("Retrying WiFi...");
        drawUI();
        wifiStatus = wifiSetup();
        if (wifiStatus != WifiConnectionStatus::CONNECTED)
        {
          statusBar->setValue("WiFi Failed! Touch to retry");
          drawUI();
        }
      }
      delay(100);
    }
    statusBar->setValue("WiFi Connected!");
    drawUI();
  }

  statusBar->setValue("Syncing time...");
  drawUI();

  // Setup time with error handling
  bool timeSuccess = timeSetup();
  if (!timeSuccess)
  {
    statusBar->setValue("Time sync failed! Retrying...");
    drawUI();
    delay(2000);

    // Retry time sync once
    timeSuccess = timeSetup();
    if (!timeSuccess)
    {
      statusBar->setValue("Time sync failed! Data may be inaccurate");
      drawUI();
      delay(3000);
    }
    statusBar->setValue("Loading data...");
    drawUI();
  }

  statusBar->setValue("Fetching data...");
  drawUI();

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
      statusBar->setValue("Auto sleep...");
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

    if (isButtonTouched(btnRefresh, t.x, t.y))
    {
      btnRefresh.pressed = true;
      btnRefresh.dirty = true;

      // Draw pressed state immediately for visual feedback
      drawUI();

      requestData();

      uiChanged = true;
    }
  }

  if (t.wasReleased())
  {
    if (btnRefresh.pressed)
    {
      btnRefresh.pressed = false;
      btnRefresh.dirty = true;
      uiChanged = true;
    }
  }

  if (uiChanged)
  {
    drawUI();
  }

  delay(100);
}