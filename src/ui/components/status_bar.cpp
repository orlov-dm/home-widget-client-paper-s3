#include "status_bar.h"

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

    Serial.println("Drawing StatusBar: " + String(this->value));
    Serial.println("Will clear rect at (" + String(this->x) + ", " + String(this->y) + ") with size " + String(this->w) + "x" + String(this->h));
    this->display.fillRect(this->x, this->y, this->w, this->h, TFT_LIGHTGREY);
    this->display.setTextColor(TFT_BLACK);
    this->display.setTextSize(2);
    this->display.setCursor(this->x + 10, this->y + (this->h - 16) / 2); // Center vertically with padding
    this->display.print(this->value);
    this->dirty = false;
}