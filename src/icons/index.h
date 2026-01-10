#pragma once

#include "refresh_icon.h"

enum Icon
{
    ICON_NONE,
    ICON_REFRESH,
};

inline const unsigned char *getIconData(Icon icon)
{
    switch (icon)
    {
    case ICON_REFRESH:
        return iconRefresh;
    default:
        return nullptr;
    }
}