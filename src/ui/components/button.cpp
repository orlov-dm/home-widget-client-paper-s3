#include "button.h"

#include <M5Unified.h>

#include "../../icons/index.h"

void drawButton(Button &b)
{
    // Only draw if the state actually changed
    if (!b.dirty)
        return;

    uint16_t bg = b.pressed ? TFT_BLACK : TFT_WHITE;
    uint16_t fg = b.pressed ? TFT_WHITE : TFT_BLACK;

    // Draw ONLY the button area
    M5.Display.fillRoundRect(b.x, b.y, b.w, b.h, 12, bg);
    M5.Display.drawRoundRect(b.x, b.y, b.w, b.h, 12, TFT_BLACK);

    M5.Display.setTextColor(fg);
    M5.Display.setTextSize(2);
    bool hasIcon = (b.icon != ICON_NONE);
    int textOffsetX = hasIcon ? 40 : 0;
    bool hasLabel = (b.label.length() != 0);

    if (hasIcon)
    {
        auto iconData = getIconData(b.icon);
        if (iconData)
            M5.Display.drawBitmap(b.x + 5, b.y + 5, iconData, 32, 28, fg, bg);
    }

    if (hasLabel)
    {
        int32_t textWidth = M5.Display.textWidth(b.label);
        int32_t textX = b.x + textOffsetX + (b.w - textOffsetX - textWidth) / 2;
        int32_t textY = b.y + (b.h / 2) - 8;

        M5.Display.setCursor(textX, textY);
        M5.Display.print(b.label);
    }

    b.dirty = false; // Reset dirty flag
}

bool isButtonTouched(Button &b, int32_t x, int32_t y)
{
    return (x > b.x && x < b.x + b.w && y > b.y && y < b.y + b.h);
}