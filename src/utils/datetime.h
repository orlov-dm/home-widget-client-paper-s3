#include <Arduino.h>

const char DATETIME_SIZE = 20;

String timestampToDatetime(time_t timestamp);

void configureTime();
