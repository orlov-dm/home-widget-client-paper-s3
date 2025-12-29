#ifndef UI_COMPONENTS_LABEL_H
#define UI_COMPONENTS_LABEL_H
#include <Arduino.h>

struct Label
{
    int32_t x, y, w, h;
    String value;
    bool dirty = true;
};

void drawLabel(Label &l, size_t textSize = 2);

#endif // UI_COMPONENTS_LABEL_H