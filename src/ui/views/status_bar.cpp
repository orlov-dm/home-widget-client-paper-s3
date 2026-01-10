#include "status_bar.h"

#include <M5Unified.h>

void StatusBar::setValue(String newValue)
{
    this->label->setValue(newValue);
    this->refreshBatteryLevel();
    this->setNeedsRender();
}

void StatusBar::refreshBatteryLevel()
{
    String batteryStr = String(M5.Power.getBatteryLevel()) + "%";
    this->batteryLabel->setValue(batteryStr);
}
