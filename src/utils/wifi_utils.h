#pragma once

#include <Arduino.h>

enum WifiConnectionStatus
{
  UNSET,
  CONNECTED,
  CONNECTING,
  FAILED
};

WifiConnectionStatus wifiSetup();

boolean isWifiConnected();
WifiConnectionStatus wifiConnect(bool needWait = true);
void wifiDisconnect();