#ifndef SLEEP_UTILS_H
#define SLEEP_UTILS_H

#include <Arduino.h>

void sleepSetup();
// Sleep duration in seconds
void goToSleep(uint64_t sleepDurationSeconds = 0);

// Get wakeup reason as string
String getWakeupReason();

#endif