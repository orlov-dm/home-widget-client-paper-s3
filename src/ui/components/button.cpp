#include "button.h"

#include <M5Unified.h>

#include "../../icons/index.h"
#include "../../app/component_manager.h"

void Button::init()
{
    ComponentManager::getInstance().registerTouchableComponent(this);
}

void Button::destroy()
{
    ComponentManager::getInstance().unregisterTouchableComponent(this);
}

void Button::doRender()
{
    int32_t bg = this->isPressedState ? TFT_BLACK : TFT_WHITE;
    int32_t fg = this->isPressedState ? TFT_WHITE : TFT_BLACK;

    auto pos = this->getPosition();
    auto size = this->getSize();
    // Draw ONLY the button area
    M5.Display.fillRoundRect(pos.x, pos.y, size.w, size.h, 12, bg);
    M5.Display.drawRoundRect(pos.x, pos.y, size.w, size.h, 12, TFT_BLACK);

    M5.Display.setTextColor(fg);
    M5.Display.setTextSize(2);
    bool hasIcon = (this->icon != ICON_NONE);
    uint8_t textOffsetX = hasIcon ? 40 : 0;
    bool hasLabel = (this->label.length() != 0);

    if (hasIcon)
    {
        auto iconData = getIconData(this->icon);
        if (iconData)
            M5.Display.drawBitmap(pos.x + 5, pos.y + 5, iconData, 32, 28, fg, bg);
    }

    if (hasLabel)
    {
        int32_t textWidth = M5.Display.textWidth(this->label);
        int32_t textX = pos.x + textOffsetX + (size.w - textOffsetX - textWidth) / 2;
        int32_t textY = pos.y + (size.h / 2) - 8;

        M5.Display.setCursor(textX, textY);
        M5.Display.print(this->label);
    }
}

void Button::setPressed(bool isPressed)
{
    if (this->isPressedState != isPressed)
    {
        this->isPressedState = isPressed;
    }
}

void Button::touch()
{
    Serial.println("Button touched: " + this->getId());
    this->setPressed(true);
    this->render(true);     // this render is just for a quick visual update
    this->setNeedsRender(); // will be re-rendered with better quality later
    if (this->touchCallback)
    {
        this->touchCallback();
    }
}

void Button::release()
{
    Serial.println("Button released: " + this->getId());
    this->setPressed(false);
    this->render(true);     // this render is just for a quick visual update
    this->setNeedsRender(); // will be re-rendered with better quality later
    if (this->releaseCallback)
    {
        this->releaseCallback();
    }
}
