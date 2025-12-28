#include "label.h"
#include <M5Unified.h>

void drawLabel(Label &l, size_t textSize)
{
    if (!l.dirty)
        return;

    // Draw ONLY the label area
    M5.Display.fillRect(l.x, l.y, l.w, l.h, TFT_WHITE);

    M5.Display.setTextColor(TFT_BLACK);
    M5.Display.setTextSize(textSize);
    M5.Display.setCursor(l.x + 10, l.y + 15);
    M5.Display.print(l.value);

    l.dirty = false; // Reset dirty flag
}