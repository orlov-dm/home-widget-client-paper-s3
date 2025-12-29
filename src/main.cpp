#include <Arduino.h>
#include <M5Unified.h>

#include "api/api_client.h"
#include "api/get_transport_request.h"
#include "api/get_transport_response.h"
#include "utils/wifi_utils.h"
#include "utils/sleep_utils.h"
#include "utils/datetime_utils.h"
#include "ui/components/index.h"

const uint8_t MAX_SCHEDULE_LABELS = 10;
Button btnRefresh;
StatusBar *statusBar = nullptr;
WifiConnectionStatus wifiStatus = WifiConnectionStatus::UNSET;
Label routeLabels[MAX_SCHEDULE_LABELS];
Label timeLabels[MAX_SCHEDULE_LABELS];
ApiClient client;
const uint32_t AWAKE_DURATION = 60000; // 1 minute in milliseconds (how long to stay awake)
uint32_t wakeupTime = 0;

bool isRequestInProgress = false;

void drawLabels()
{
  for (int i = 0; i < MAX_SCHEDULE_LABELS; i++)
  {
    if (routeLabels[i].value.length() == 0 || timeLabels[i].value.length() == 0)
    {
      continue;
    }
    drawLabel(routeLabels[i], 3);
    drawLabel(timeLabels[i]);
  }
}

void drawUI()
{
  M5.Display.startWrite();
  if (statusBar)
  {
    statusBar->draw();
  }
  drawButton(btnRefresh);
  drawLabels();
  M5.Display.endWrite();
}

void clearLabels()
{
  for (int i = 0; i < MAX_SCHEDULE_LABELS; i++)
  {
    routeLabels[i] = Label();
    timeLabels[i] = Label();
  }
}

void requestData()
{
  if (isRequestInProgress)
  {
    Serial.println("Request already in progress, skipping...");
    return;
  }

  // Wait for sync
  struct tm timeinfo;
  int retries = 0;
  // getLocalTime is a built-in helper that handles the wait for you
  while (!getLocalTime(&timeinfo) && retries < 15)
  {
    Serial.print(".");
    delay(1000);
    retries++;
  }

  if (retries < 15)
  {
    Serial.println("\nTime synchronized successfully!");
    Serial.printf("Current time: %02d:%02d:%02d\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    Serial.printf(
        "%04d-%02d-%02d %02d:%02d:%02d\n",
        timeinfo.tm_year + 1900,
        timeinfo.tm_mon + 1,
        timeinfo.tm_mday,
        timeinfo.tm_hour,
        timeinfo.tm_min,
        timeinfo.tm_sec);
    Serial.printf("Epoch time: %ld\n", time(nullptr));
  }

  isRequestInProgress = true;
  GetTransportRequest request;
  GetTransportResponse *response =
      client.doRequest<GetTransportResponse>(&request);

  clearLabels();

  if (response && response->isSuccess())
  {
    TransportTime *times = response->getTransportTimes();
    int countToShow = min(response->getTransportTimesCount(), 10); // Show max 10 entries
    int startX = 20;
    int startY = statusBar->getHeight() + btnRefresh.h + 40;
    int labelHeight = 40;
    int labelWidth = 300;

    M5.Display.startWrite();
    M5.Display.fillRect(startX, startY, M5.Display.width() - startX, M5.Display.height() - startY, TFT_WHITE);

    time_t utcTime = time(nullptr);
    for (int i = 0; i < countToShow; i++)
    {
      String routeStr = String(times[i].route);
      Label routeLabel;
      routeLabel.x = startX;
      routeLabel.y = startY + labelHeight * i;
      routeLabel.w = 40;
      routeLabel.h = labelHeight;
      routeLabel.value = routeStr;
      routeLabels[i] = routeLabel;

      auto expectedArriveTimestamp = times[i].expectedArriveTimestamp;
      int secondsDiff = expectedArriveTimestamp - utcTime;
      String secondsDiffString = "In " + String(secondsDiff / 60) + " min";
      Label timeLabel;
      timeLabel.x = startX + 40 + 10;
      timeLabel.y = startY + labelHeight * i;
      timeLabel.w = labelWidth;
      timeLabel.h = labelHeight;
      timeLabel.value = secondsDiffString;
      timeLabels[i] = timeLabel;
    }
    M5.Display.endWrite();

    delete response;

    char statusText[255];
    String timeStr = timestampToDatetime(utcTime);
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

  // M5.Display.fillScreen(TFT_WHITE); // Clear screen once at startup
  M5.Display.setTextSize(2);

  statusBar = new StatusBar(
      M5.Display,
      0, 0, M5.Display.width(), 40, "Starting up...");

  btnRefresh.x = 20;
  btnRefresh.y = statusBar->getHeight() + 20;
  btnRefresh.w = 200;
  btnRefresh.h = 60;
  btnRefresh.label = "Refresh";

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

  requestData();
  drawUI();
}

// ---------- LOOP ----------
void loop()
{
  M5.update();

  if (millis() - wakeupTime >= AWAKE_DURATION)
  {
    statusBar->setValue("Auto sleep...");
    drawUI();
    delay(1000);

    goToSleep();
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