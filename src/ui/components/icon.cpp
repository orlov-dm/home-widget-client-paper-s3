#include "icon.h"

void Icon::doRender()
{
    const unsigned char *iconData = getIconData(this->iconName, this->iconSize);
    if (iconData == nullptr)
    {
        Serial.println("Icon data not found for the specified icon.");
        return;
    }
    const auto &pos = this->getPosition();
    const auto &size = this->getSize();
    M5.Display.drawBitmap(pos.x, pos.y, iconData, size.w, size.h, TFT_WHITE, TFT_BLACK);
}
