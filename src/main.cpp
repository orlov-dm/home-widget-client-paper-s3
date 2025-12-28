#include "api/api_client.h"
#include "api/get_transport_request.h"
#include "api/get_transport_response.h"
#include "utils/wifi_utils.h"
#include "utils/datetime.h"
#include "ui/components/index.h"
#include <M5Unified.h>

// ---------- UI ELEMENT STRUCT ----------

// ---------- UI ELEMENTS ----------
Button btnRefresh;
StatusBar *statusBar = nullptr;
WifiConnectionStatus wifiStatus = WifiConnectionStatus::UNSET;

ApiClient client;

bool isRequestInProgress = false;

// ---------- DRAWING FUNCTIONS ----------

void drawUI()
{
  // We do NOT call fillScreen here.
  // We rely on the specific draw functions to clear their own areas.

  M5.Display.startWrite(); // Start transaction for smoother update
  if (statusBar)
  {
    statusBar->draw();
  }
  drawButton(btnRefresh);
  M5.Display.endWrite(); // Commit the transaction
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
      char str[3];
      itoa(times[i].route, str, 10);
      Label routeLabel;
      routeLabel.x = startX;
      routeLabel.y = startY + labelHeight * i;
      routeLabel.w = 40;
      routeLabel.h = labelHeight;
      routeLabel.value = str;
      drawLabel(routeLabel, 3);

      auto expectedArriveTimestamp = times[i].expectedArriveTimestamp;
      int secondsDiff = expectedArriveTimestamp - utcTime;
      char secondsDiffString[32];
      snprintf(secondsDiffString, sizeof(secondsDiffString), "In %d min", secondsDiff / 60);
      Label timeLabel;
      timeLabel.x = startX + 40 + 10;
      timeLabel.y = startY + labelHeight * i;
      timeLabel.w = labelWidth;
      timeLabel.h = labelHeight;
      timeLabel.value = secondsDiffString;
      drawLabel(timeLabel);

      Serial.print("Route: ");
      Serial.println(times[i].route);
      Serial.print("Type: ");
      Serial.println(times[i].transportType);
      Serial.print("Expected Arrive Timestamp: ");
      Serial.println(times[i].expectedArriveTimestamp);
      Serial.print("Scheduled Arrive Timestamp: ");
      Serial.println(times[i].scheduledArriveTimestamp);
    }
    M5.Display.endWrite();

    delete response;

    char statusText[255];
    String timeStr = timestampToDatetime(utcTime);
    snprintf(statusText, sizeof(statusText), "Data refreshed at %s", timeStr.c_str());
    statusBar->setValue(statusText);
  }
  else
  {
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

  // CRITICAL E-INK SETTINGS
  M5.Display.begin();

  // Set update mode to FAST (Low quality, but fast and no flash)
  // Options: epd_quality (slow/clean), epd_fast (fast/ghosting), epd_fastest
  M5.Display.setEpdMode(epd_mode_t::epd_fast);

  M5.Display.fillScreen(TFT_WHITE); // Clear screen once at startup
  M5.Display.setTextSize(2);

  statusBar = new StatusBar(
      M5.Display,
      0, 0, M5.Display.width(), 40, "Starting up...");

  // Initialize buttons
  btnRefresh.x = 20;
  btnRefresh.y = statusBar->getHeight() + 20;
  btnRefresh.w = 200;
  btnRefresh.h = 60;
  btnRefresh.label = "Refresh";

  // Initialize WiFi BEFORE making any HTTP requests
  Serial.println("Connecting to WiFi...");
  wifiConnect(true);

  // Configure NTP time synchronization
  configureTime();

  requestData();
  drawUI();
}

// ---------- LOOP ----------
void loop()
{
  M5.update();

  auto t = M5.Touch.getDetail();
  bool uiChanged = false;

  if (t.wasPressed())
  {
    if (isButtonTouched(btnRefresh, t.x, t.y))
    {
      btnRefresh.pressed = true;
      btnRefresh.dirty = true;
      statusBar->setValue("Button Refresh Pressed");
      uiChanged = true;

      // configureTime();
      requestData();
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