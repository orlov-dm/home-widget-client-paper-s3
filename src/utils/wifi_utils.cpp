#include "wifi_utils.h"

#include <WiFi.h>
#include <time.h>
#include <TimeLib.h>

#include "../secrets.h"

WifiConnectionStatus wifiSetup()
{
  Serial.println("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WifiConnectionStatus status = wifiConnect(true);
  Serial.printf("WiFi Status: %d\n", status);
  return status;
}

boolean isWifiConnected()
{
  return WiFi.status() == WL_CONNECTED;
}

WifiConnectionStatus wifiConnect(bool needWait)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    return CONNECTED;
  }

  WiFi.begin(SECRET_WIFI_SSID, SECRET_WIFI_PASS);
  if (!needWait)
  {
    return CONNECTING;
  }
  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20)
  {
    delay(500);
    timeout++;
  }

  auto status = WiFi.status();
  switch (status)
  {
  case WL_CONNECTED:
    return CONNECTED;
  case WL_CONNECT_FAILED:
  case WL_NO_SSID_AVAIL:
  case WL_CONNECTION_LOST:
  case WL_DISCONNECTED:
    return FAILED;
  default:
    return CONNECTING;
  }
}

void wifiDisconnect()
{
  WiFi.disconnect();
}
