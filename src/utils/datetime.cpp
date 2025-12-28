#include "wifi_utils.h"
#include <time.h>

String timestampToDatetime(time_t utcTimestamp)
{
    struct tm timeinfo;
    localtime_r(&utcTimestamp, &timeinfo);

    String result;
    result.reserve(19); // Pre-allocate for efficiency
    result += String(timeinfo.tm_year + 1900);
    result += "-";
    if (timeinfo.tm_mon + 1 < 10)
        result += "0";
    result += String(timeinfo.tm_mon + 1);
    result += "-";
    if (timeinfo.tm_mday < 10)
        result += "0";
    result += String(timeinfo.tm_mday);
    result += " ";
    if (timeinfo.tm_hour < 10)
        result += "0";
    result += String(timeinfo.tm_hour);
    result += ":";
    if (timeinfo.tm_min < 10)
        result += "0";
    result += String(timeinfo.tm_min);
    result += ":";
    if (timeinfo.tm_sec < 10)
        result += "0";
    result += String(timeinfo.tm_sec);

    return result;
}

void configureTime()
{
    // Find your TZ string here: https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
    const char *TZ_INFO = "EET-2EEST,M3.5.0/3,M10.5.0/4";
    Serial.print("Configuring NTP");
    if (!isWifiConnected())
    {
        wifiConnect(true);
    }

    // Method 1: Automatic DST via TZ String (Recommended)
    configTzTime(TZ_INFO, "pool.ntp.org", "time.nist.gov");

    // Wait for sync
    struct tm timeinfo;
    int retries = 0;

    // getLocalTime is a built-in helper that handles the wait for you
    while (!getLocalTime(&timeinfo) && retries < 15)
    {
        Serial.print(".");
        delay(1000);
        retries++;
    }

    if (retries < 15)
    {
        Serial.println("\nTime synchronized successfully!");

        // // Sync TimeLib with system time
        // time_t nowSec = mktime(&timeinfo);
        // setTime(nowSec);

        Serial.printf("Current time: %02d:%02d:%02d\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        Serial.printf(
            "%04d-%02d-%02d %02d:%02d:%02d\n",
            timeinfo.tm_year + 1900,
            timeinfo.tm_mon + 1,
            timeinfo.tm_mday,
            timeinfo.tm_hour,
            timeinfo.tm_min,
            timeinfo.tm_sec);
    }
    else
    {
        Serial.println("\nFailed to obtain time");
    }
}