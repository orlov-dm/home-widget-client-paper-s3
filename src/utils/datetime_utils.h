#ifndef DATETIME_UTILS_H
#define DATETIME_UTILS_H

#include <Arduino.h>

const char DATETIME_SIZE = 20;

bool timeSetup();

String timestampToDatetime(time_t timestamp);

bool configureTime();

#endif // DATETIME_H
