#include "dev_utils.h"

#include <Arduino.h>

bool isDevMode()
{
#if defined(DEV_MODE)
    return true;
#endif

    return false;
}