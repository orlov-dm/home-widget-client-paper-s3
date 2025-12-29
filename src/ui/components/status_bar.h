#ifndef UI_COMPONENTS_STATUS_BAR_H
#define UI_COMPONENTS_STATUS_BAR_H

#include <Arduino.h>
#include <M5GFX.h>

class StatusBar
{
public:
    StatusBar(M5GFX &display, int x, int y, int w, int h, const char *initialValue)
        : display(display), x(x), y(y), w(w), h(h)
    {
        this->value = String(initialValue);
    }

    void setValue(String newValue);
    void draw();

    int getHeight() const
    {
        return this->h;
    }

private:
    int x, y, w, h;
    String value = "";
    bool pressed = false;
    bool dirty = true;
    M5GFX &display;
};

#endif // UI_COMPONENTS_STATUS_BAR_H