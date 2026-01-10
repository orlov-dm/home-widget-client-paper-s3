#pragma once

#include <Arduino.h>

const char DATETIME_SIZE = 20;
const uint32_t TIME_SYNC_INTERVAL = 86400; // Resync every 24 hours (in seconds)

bool timeSetup();

String timestampToDatetime(time_t timestamp);
String formatTimestampToLocalTimeString(time_t timestamp, const char *format);

bool configureTime();

bool isTimeValid();

bool shouldResyncTime();

time_t getUtcTime();
