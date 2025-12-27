#include "api/api_client.h"
#include "api/get_transport_request.h"
#include "api/get_transport_response.h"
#include "utils/wifi_utils.h"
#include "utils/datetime.h"
#include <M5Unified.h>

// ---------- UI ELEMENT STRUCT ----------
struct Button
{
  int x, y, w, h;
  const char *label;
  bool pressed = false;
  bool dirty = true; // New flag: only redraw if this is true
};

struct Label
{
  int x, y, w, h;
  const char *value;
  bool dirty = true; // New flag: only redraw if this is true
};

// ---------- UI ELEMENTS ----------
Button btnA;
Button btnB;

String statusText = "Touch a button";
bool textDirty = true; // Track if text needs redrawing

ApiClient client;

// ---------- DRAWING FUNCTIONS ----------
void drawButton(Button &b)
{
  // Only draw if the state actually changed
  if (!b.dirty)
    return;

  uint16_t bg = b.pressed ? TFT_BLACK : TFT_WHITE;
  uint16_t fg = b.pressed ? TFT_WHITE : TFT_BLACK;

  // Draw ONLY the button area
  M5.Display.fillRoundRect(b.x, b.y, b.w, b.h, 12, bg);
  M5.Display.drawRoundRect(b.x, b.y, b.w, b.h, 12, TFT_BLACK);

  M5.Display.setTextColor(fg);
  M5.Display.setTextSize(2);

  int textWidth = M5.Display.textWidth(b.label);
  int textX = b.x + (b.w - textWidth) / 2;
  int textY = b.y + (b.h / 2) - 8;

  M5.Display.setCursor(textX, textY);
  M5.Display.print(b.label);

  b.dirty = false; // Reset dirty flag
}

void drawStatus()
{
  if (!textDirty)
    return;

  // Clear ONLY the text area, not the whole screen
  M5.Display.fillRect(0, 0, 300, 40, TFT_WHITE);

  M5.Display.setTextColor(TFT_BLACK);
  M5.Display.setTextSize(2);
  M5.Display.setCursor(10, 15);
  M5.Display.print(statusText);

  textDirty = false;
}

void drawLabel(Label &l)
{
  if (!l.dirty)
    return;

  // Draw ONLY the button area
  M5.Display.fillRect(l.x, l.y, l.w, l.h, TFT_WHITE);

  M5.Display.setTextColor(TFT_BLACK);
  M5.Display.setTextSize(2);
  M5.Display.setCursor(l.x + 10, l.y + 15);
  M5.Display.print(l.value);

  l.dirty = false; // Reset dirty flag
}

void drawUI()
{
  // We do NOT call fillScreen here.
  // We rely on the specific draw functions to clear their own areas.

  M5.Display.startWrite(); // Start transaction for smoother update
  drawStatus();
  drawButton(btnA);
  drawButton(btnB);
  M5.Display.endWrite(); // Commit the transaction
}

// ---------- TOUCH LOGIC ----------
bool isTouched(Button &b, int x, int y)
{
  return (x > b.x && x < b.x + b.w && y > b.y && y < b.y + b.h);
}

// ---------- SETUP ----------
void setup()
{
  Serial.begin(9600);
  // while (!Serial)
  //   ;
  Serial.println("BOOT");

  // Initialize buttons
  btnA.x = 20;
  btnA.y = 60;
  btnA.w = 200;
  btnA.h = 60;
  btnA.label = "Button A";

  btnB.x = 20;
  btnB.y = 140;
  btnB.w = 200;
  btnB.h = 60;
  btnB.label = "Button B";

  auto cfg = M5.config();
  M5.begin(cfg);

  // CRITICAL E-INK SETTINGS
  M5.Display.begin();

  // Set update mode to FAST (Low quality, but fast and no flash)
  // Options: epd_quality (slow/clean), epd_fast (fast/ghosting), epd_fastest
  M5.Display.setEpdMode(epd_mode_t::epd_fast);

  M5.Display.fillScreen(TFT_WHITE); // Clear screen once at startup
  M5.Display.setTextSize(2);

  // Initialize WiFi BEFORE making any HTTP requests
  Serial.println("Connecting to WiFi...");
  WifiConnectionStatus wifiStatus = wifiConnect();
  if (wifiStatus == CONNECTED)
  {
    Serial.println("WiFi connected!");
    statusText = "WiFi connected";
  }
  else
  {
    Serial.println("WiFi connection failed!");
    statusText = "WiFi failed";
  }

  // Force initial draw
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
    if (isTouched(btnA, t.x, t.y))
    {
      btnA.pressed = true;
      btnA.dirty = true;

      // Check WiFi connection before making request
      if (!isWifiConnected())
      {
        Serial.println("WiFi not connected, attempting reconnect...");
        WifiConnectionStatus wifiStatus = wifiConnect();
        if (wifiStatus != CONNECTED)
        {
          Serial.println("Failed to reconnect WiFi");
          statusText = "WiFi error";
          textDirty = true;
          uiChanged = true;
          btnA.pressed = false;
          btnA.dirty = true;
          if (uiChanged)
          {
            drawUI();
          }
          return;
        }
      }

      GetTransportRequest request;
      GetTransportResponse *response =
          client.doRequest<GetTransportResponse>(&request);

      if (response && response->isSuccess())
      {
        TransportTime *times = response->getTransportTimes();
        int count = response->getTransportTimesCount();

        int startX = 20;
        int startY = 220;
        int labelHeight = 40;
        int labelWidth = 300;

        M5.Display.startWrite();
        M5.Display.fillRect(startX, startY, M5.Display.width() - startX, M5.Display.height() - startY, TFT_WHITE);

        for (int i = 0; i < count; i++)
        {
          char timeString[DATETIME_SIZE];
          timestampToDatetime(timeString, times[i].expectedArriveTimestamp);
          Label timeLabel;
          timeLabel.x = startX;
          timeLabel.y = startY + labelHeight * i;
          timeLabel.w = labelWidth;
          timeLabel.h = labelHeight;
          timeLabel.value = timeString;
          drawLabel(timeLabel);

          char str[3];
          itoa(times[i].route, str, 10);
          Label routeLabel;
          routeLabel.x = startX + labelWidth + 20;
          routeLabel.y = startY + labelHeight * i;
          routeLabel.w = labelWidth;
          routeLabel.h = labelHeight;
          routeLabel.value = str;
          drawLabel(routeLabel);

          Serial.println(timeString);
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
      }

      textDirty = true;
      uiChanged = true;
    }
    if (isTouched(btnB, t.x, t.y))
    {
      btnB.pressed = true;
      btnB.dirty = true;
      statusText = "Button B pressed";
      textDirty = true;
      uiChanged = true;
    }
  }

  if (t.wasReleased())
  {
    if (btnA.pressed)
    {
      btnA.pressed = false;
      btnA.dirty = true;
      uiChanged = true;
    }
    if (btnB.pressed)
    {
      btnB.pressed = false;
      btnB.dirty = true;
      uiChanged = true;
    }
  }

  if (uiChanged)
  {
    drawUI();
  }

  delay(10);
}