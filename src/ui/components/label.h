#ifndef UI_COMPONENTS_LABEL_H
#define UI_COMPONENTS_LABEL_H
#include <Arduino.h>

struct Label
{
    int32_t x, y, w, h;
    String value;
    int8_t textSize;
    bool dirty;
};

void drawLabel(Label &l);

#endif // UI_COMPONENTS_LABEL_H