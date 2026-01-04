#include "datetime_utils.h"

#include <time.h>
#include <sys/time.h>
#include <Preferences.h>
#include <M5Unified.h>

#include "wifi_utils.h"

String TZ_UTC = "UTC";
String TZ_LOCAL = "EET-2EEST,M3.5.0/3,M10.5.0/4"; // Eastern European Time with DST rules

Preferences timePref;

// Check if the current time is valid (after year 2020)
bool isTimeValid()
{
    time_t now;
    time(&now);
    // If time is before 2020-01-01, it's not valid
    return now > 1577836800; // Unix timestamp for 2020-01-01 00:00:00 UTC
}

// Check if we should resync time (more than 24 hours since last sync)
bool shouldResyncTime()
{
    if (!isTimeValid())
    {
        Serial.println("Time is invalid, need to sync");
        return true;
    }

    timePref.begin("time", true); // Read-only mode
    uint32_t lastSync = timePref.getUInt("lastSync", 0);
    timePref.end();

    if (lastSync == 0)
    {
        Serial.println("No previous sync recorded, need to sync");
        return true;
    }

    time_t now;
    time(&now);
    uint32_t timeSinceSync = now - lastSync;

    Serial.printf("Time since last sync: %u seconds (%.1f hours)\n",
                  timeSinceSync, timeSinceSync / 3600.0);

    return timeSinceSync > TIME_SYNC_INTERVAL;
}

bool configureTime()
{
    // Find your TZ string here: https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
    Serial.println("Configuring NTP");
    if (!isWifiConnected())
    {
        WifiConnectionStatus wifiStatus = wifiConnect(true);
        if (wifiStatus != CONNECTED)
        {
            Serial.println("WiFi not connected, cannot sync time");
            return false;
        }
    }

    // Method 1: Automatic DST via TZ String (Recommended)
    configTzTime(TZ_LOCAL.c_str(), "pool.ntp.org", "time.nist.gov");

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

        // Save the sync timestamp to Preferences (persists across reboots)
        time_t now;
        time(&now);

        timePref.begin("time", false); // Read-write mode
        timePref.putUInt("lastSync", now);
        timePref.end();

        // Save UTC time to hardware RTC (BM8563) - persists through deep sleep
        struct tm utcTimeinfo;
        gmtime_r(&now, &utcTimeinfo); // Get UTC time

        m5::rtc_datetime_t rtcTime;
        rtcTime.date.year = utcTimeinfo.tm_year + 1900;
        rtcTime.date.month = utcTimeinfo.tm_mon + 1;
        rtcTime.date.date = utcTimeinfo.tm_mday;
        rtcTime.date.weekDay = utcTimeinfo.tm_wday;
        rtcTime.time.hours = utcTimeinfo.tm_hour;
        rtcTime.time.minutes = utcTimeinfo.tm_min;
        rtcTime.time.seconds = utcTimeinfo.tm_sec;

        M5.Rtc.setDateTime(rtcTime);
        Serial.println("Time saved to hardware RTC (BM8563) as UTC");

        Serial.printf("Current local time: %02d:%02d:%02d\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        Serial.printf("Saved sync timestamp: %u\n", (uint32_t)now);
        return true;
    }
    else
    {
        Serial.println("\nFailed to obtain time");
        return false;
    }
}

bool timeSetup()
{
    Serial.println("Setting up time...");

    // Restore UTC time from hardware RTC (BM8563) - persists through deep sleep
    m5::rtc_datetime_t rtcTime;
    if (M5.Rtc.getDateTime(&rtcTime))
    {
        // Convert RTC UTC time to unix timestamp
        struct tm timeinfo = {0};
        timeinfo.tm_year = rtcTime.date.year - 1900;
        timeinfo.tm_mon = rtcTime.date.month - 1;
        timeinfo.tm_mday = rtcTime.date.date;
        timeinfo.tm_hour = rtcTime.time.hours;
        timeinfo.tm_min = rtcTime.time.minutes;
        timeinfo.tm_sec = rtcTime.time.seconds;
        timeinfo.tm_isdst = 0; // UTC has no DST

        // Use mktime but offset by timezone since RTC stores UTC
        // First set TZ to UTC, convert, then restore
        setenv("TZ", TZ_UTC.c_str(), 1);
        tzset();
        time_t rtcTimestamp = mktime(&timeinfo);

        // Restore timezone
        setenv("TZ", TZ_LOCAL.c_str(), 1);
        tzset();

        // Set system time from RTC
        struct timeval tv;
        tv.tv_sec = rtcTimestamp;
        tv.tv_usec = 0;
        settimeofday(&tv, NULL);

        Serial.printf("Restored UTC time from hardware RTC: %04d-%02d-%02d %02d:%02d:%02d\n",
                      rtcTime.date.year,
                      rtcTime.date.month,
                      rtcTime.date.date,
                      rtcTime.time.hours,
                      rtcTime.time.minutes,
                      rtcTime.time.seconds);
        Serial.printf("Converted to timestamp: %u\n", (uint32_t)rtcTimestamp);
    }
    else
    {
        Serial.println("Failed to read from hardware RTC");
    }

    // Check if we need to sync time (for deep sleep wake-ups)
    if (!shouldResyncTime())
    {
        Serial.println("Time is already valid and recent, skipping NTP sync");
        time_t now;
        time(&now);
        struct tm timeinfo;
        localtime_r(&now, &timeinfo);
        Serial.printf("Using RTC time: %04d-%02d-%02d %02d:%02d:%02d\n",
                      timeinfo.tm_year + 1900,
                      timeinfo.tm_mon + 1,
                      timeinfo.tm_mday,
                      timeinfo.tm_hour,
                      timeinfo.tm_min,
                      timeinfo.tm_sec);
        return true;
    }

    // Need to sync from NTP
    Serial.println("Syncing time from NTP server...");

    return configureTime();
}

String timestampToDatetime(time_t utcTimestamp)
{
    struct tm timeinfo;
    localtime_r(&utcTimestamp, &timeinfo);

    String result;
    result.reserve(19); // Pre-allocate for efficiency

    if (timeinfo.tm_mday < 10)
        result += "0";
    result += String(timeinfo.tm_mday);
    result += ".";
    if (timeinfo.tm_mon + 1 < 10)
        result += "0";
    result += String(timeinfo.tm_mon + 1);
    result += ".";
    result += String(timeinfo.tm_year + 1900);
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

String formatTimestampToLocalTimeString(time_t utcTimestamp, const char *format)
{
    struct tm timeinfo;
    localtime_r(&utcTimestamp, &timeinfo);

    char buffer[64];
    strftime(buffer, sizeof(buffer), format, &timeinfo);
    return String(buffer);
}

time_t getUtcTime()
{
    return time(nullptr);
}
