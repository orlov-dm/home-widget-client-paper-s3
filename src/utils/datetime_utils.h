#ifndef DATETIME_UTILS_H
#define DATETIME_UTILS_H

#include <Arduino.h>

const char DATETIME_SIZE = 20;
const uint32_t TIME_SYNC_INTERVAL = 86400; // Resync every 24 hours (in seconds)

bool timeSetup();

String timestampToDatetime(time_t timestamp);

bool configureTime();

bool isTimeValid();

bool shouldResyncTime();

time_t getUtcTime();

#endif // DATETIME_H
