#ifndef UI_COMPONENTS_BUTTON_H
#define UI_COMPONENTS_BUTTON_H

#include <Arduino.h>

struct Button
{
    int32_t x, y, w, h;
    String label;
    bool pressed = false;
    bool dirty = true;
};

void drawButton(Button &b);
bool isButtonTouched(Button &b, int32_t x, int32_t y);
#endif