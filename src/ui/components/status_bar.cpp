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

    M5.Display.fillRect(this->x, this->y, this->w, this->h, TFT_LIGHTGREY);
    M5.Display.setTextColor(TFT_BLACK);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(this->x + 10, this->y + (this->h - 16) / 2); // Center vertically with padding
    M5.Display.print(this->value);
    M5.Display.setCursor(this->x + this->w - 60, this->y + (this->h - 16) / 2); // Right align with padding
    M5.Display.print(String(M5.Power.getBatteryLevel()) + "%");
    this->dirty = false;
}