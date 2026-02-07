#pragma once

#include "refresh_icon.h"
#include "bus_icon.h"
#include "weather/main_icon.h"
#include "weather/sun.h"
#include "weather/partly_cloudy.h"
#include "weather/cloudy.h"
#include "weather/drizzle.h"
#include "weather/rain.h"
#include "weather/thunderstorm.h"
#include "weather/snow.h"
#include "weather/fog.h"

enum IconName
{
    ICON_NONE,
    ICON_REFRESH,
    ICON_BUS,
    ICON_WEATHER_MAIN,
    ICON_WEATHER_SUN,
    ICON_WEATHER_PARTLY_CLOUDY,
    ICON_WEATHER_CLOUDY,
    ICON_WEATHER_DRIZZLE,
    ICON_WEATHER_RAIN,
    ICON_WEATHER_THUNDERSTORM,
    ICON_WEATHER_SNOW,
    ICON_WEATHER_FOG
};

enum IconSize
{
    ICON_SIZE_32,
    ICON_SIZE_64,
    ICON_SIZE_128,
    ICON_SIZE_256
};

inline const unsigned char *getIconData(IconName icon, IconSize size = IconSize::ICON_SIZE_32)
{
    switch (icon)
    {
    case ICON_REFRESH:
        return iconRefresh;
    case ICON_BUS:
        return iconBus;
    case ICON_WEATHER_MAIN:
        return iconWeatherMain;
    case ICON_WEATHER_SUN:
        return iconsWeatherSun[size];
    case ICON_WEATHER_PARTLY_CLOUDY:
        return iconsWeatherPartlyCloudy[size];
    case ICON_WEATHER_CLOUDY:
        return iconsWeatherCloudy[size];
    case ICON_WEATHER_DRIZZLE:
        return iconsWeatherDrizzle[size];
    case ICON_WEATHER_RAIN:
        return iconsWeatherRain[size];
    case ICON_WEATHER_THUNDERSTORM:
        return iconsWeatherThunderstorm[size];
    case ICON_WEATHER_SNOW:
        return iconsWeatherSnow[size];
    case ICON_WEATHER_FOG:
        return iconsWeatherFog[size];
    default:
        return nullptr;
    }
}