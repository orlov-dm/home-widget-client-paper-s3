#pragma once

#include "refresh_icon.h"
#include "bus_icon.h"

enum Icon
{
    ICON_NONE,
    ICON_REFRESH,
    ICON_BUS,
};

inline const unsigned char *getIconData(Icon icon)
{
    switch (icon)
    {
    case ICON_REFRESH:
        return iconRefresh;
    case ICON_BUS:
        return iconBus;
    default:
        return nullptr;
    }
}