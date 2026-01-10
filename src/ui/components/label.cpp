#include "label.h"

#include <M5Unified.h>

void Label::doRender()
{
    auto pos = this->getPosition();
    auto size = this->getSize();

    M5.Display.setTextColor(TFT_BLACK);
    M5.Display.setTextSize(this->textSize);

    // Calculate vertical offset to center text based on text size
    // Each text size increases font height by ~8 pixels
    int fontHeight = 8 * this->textSize;
    int yOffset = (size.h - fontHeight) / 2;

    M5.Display.setCursor(pos.x + 10, pos.y + yOffset);
    M5.Display.print(this->value);
}
