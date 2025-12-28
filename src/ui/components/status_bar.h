#ifndef UI_COMPONENTS_STATUS_BAR_H
#define UI_COMPONENTS_STATUS_BAR_H

#include <cstring>
#include <M5Unified.h>
#include <M5GFX.h>

class StatusBar
{
public:
    StatusBar(M5GFX &display, int x, int y, int w, int h, const char *initialValue)
        : display(display), x(x), y(y), w(w), h(h)
    {
        strcpy(this->value, initialValue);
    }

    void setValue(const char *newValue);
    void draw();

    int getHeight() const
    {
        return this->h;
    }

private:
    int x, y, w, h;
    char value[255] = "";
    bool pressed = false;
    bool dirty = true;
    M5GFX &display;
};

#endif // UI_COMPONENTS_STATUS_BAR_H