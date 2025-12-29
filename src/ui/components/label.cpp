#include "label.h"

#include <M5Unified.h>

void drawLabel(Label &l, size_t textSize)
{
    if (!l.dirty)
        return;

    M5.Display.fillRect(l.x, l.y, l.w, l.h, TFT_WHITE);

    M5.Display.setTextColor(TFT_BLACK);
    M5.Display.setTextSize(textSize);
    
    // Calculate vertical offset to center text based on text size
    // Each text size increases font height by ~8 pixels
    int fontHeight = 8 * textSize;
    int yOffset = (l.h - fontHeight) / 2;
    
    M5.Display.setCursor(l.x + 10, l.y + yOffset);
    M5.Display.print(l.value);

    l.dirty = false; // Reset dirty flag
}