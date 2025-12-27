#include <Arduino.h>
#include <TimeLib.h>

const char DATETIME_SIZE = 20;

void timestampToDatetime(char * timeString, uint32_t timestamp) {
  sprintf(timeString, "%04d-%02d-%02d %02d:%02d:%02d", year(timestamp),
          month(timestamp), day(timestamp), hour(timestamp), minute(timestamp),
          second(timestamp));
}