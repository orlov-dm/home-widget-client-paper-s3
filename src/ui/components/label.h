#ifndef UI_COMPONENTS_LABEL_H
#define UI_COMPONENTS_LABEL_H
#include <Arduino.h>

struct Label
{
    int x, y, w, h;
    const char *value;
    bool dirty = true; // New flag: only redraw if this is true
};

void drawLabel(Label &l, size_t textSize = 2);

#endif // UI_COMPONENTS_LABEL_H