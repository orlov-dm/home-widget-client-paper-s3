#ifndef WIFI_UTILS_H
#define WIFI_UTILS_H

#include <Arduino.h>

enum WifiConnectionStatus
{
  UNSET,
  CONNECTED,
  CONNECTING,
  FAILED
};

boolean isWifiConnected();
WifiConnectionStatus wifiConnect(bool needWait = true);
void wifiDisconnect();

#endif