#ifndef UI_COMPONENTS_BUTTON_H
#define UI_COMPONENTS_BUTTON_H

struct Button
{
    int x, y, w, h;
    const char *label;
    bool pressed = false;
    bool dirty = true; // New flag: only redraw if this is true
};

void drawButton(Button &b);
bool isButtonTouched(Button &b, int x, int y);
#endif