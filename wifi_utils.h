#ifndef WIFI_UTILS_H
#define WIFI_UTILS_H

#include <Arduino.h>

enum WifiConnectionStatus
{
  CONNECTED,
  FAILED
};

boolean isWifiConnected();
WifiConnectionStatus wifiConnect();
void wifiDisconnect();

#endif