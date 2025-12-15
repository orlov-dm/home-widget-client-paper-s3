#include <WiFi.h>
#include "wifi_utils.h"
#include "secrets.h"

enum WifiConnectionStatus
{
  CONNECTED,
  FAILED
};

WifiConnectionStatus wifiConnect()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    return CONNECTED;
  }

  WiFi.begin(SECRET_WIFI_SSID, SECRET_WIFI_PASS);
  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20)
  {
    delay(500);
    timeout++;
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    return FAILED;
  }

  return CONNECTED;
}

void wifiDisconnect()
{
  WiFi.disconnect();
}