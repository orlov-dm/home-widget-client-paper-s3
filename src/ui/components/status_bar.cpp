#include "status_bar.h"

#include <M5Unified.h>

void StatusBar::setValue(String newValue)
{
    if (this->value != newValue)
    {
        this->value = newValue;
        this->dirty = true;
    }
}

void StatusBar::draw()
{
    if (!this->dirty)
        return;

    this->display.fillRect(this->x, this->y, this->w, this->h, TFT_LIGHTGREY);
    this->display.setTextColor(TFT_BLACK);
    this->display.setTextSize(2);
    this->display.setCursor(this->x + 10, this->y + (this->h - 16) / 2); // Center vertically with padding
    this->display.print(this->value);

    this->display.setCursor(this->x + this->w - 60, this->y + (this->h - 16) / 2); // Right align with padding
    this->display.print(String(M5.Power.getBatteryLevel()) + "%");
    this->dirty = false;
}