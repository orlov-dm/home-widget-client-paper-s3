#include "label.h"

#include <M5Unified.h>

void Label::init()
{
    Component::init();
    if (this->getSize().h == 0)
    {
        // Set default height based on text size (each text size increases font height by ~8 pixels)
        int defaultHeight = 8 * this->textSize + 10; // Add some padding
        this->setHeight(defaultHeight);
    }
}

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

    // Calculate horizontal position based on alignment
    int textWidth = M5.Display.textWidth(this->value);
    if (this->alignment == Alignment::LEFT)
    {
        M5.Display.setCursor(pos.x + 10, pos.y + yOffset);
    }
    else if (this->alignment == Alignment::CENTER)
    {
        int textX = pos.x + (size.w - textWidth) / 2;
        M5.Display.setCursor(textX, pos.y + yOffset);
    }
    else if (this->alignment == Alignment::RIGHT)
    {
        int textX = pos.x + size.w - textWidth - 10;
        M5.Display.setCursor(textX, pos.y + yOffset);
    }
    M5.Display.print(this->value);
}
